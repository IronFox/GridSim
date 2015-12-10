#include "../global_root.h"
#include "expression.h"




namespace Expression
{
	static Tokenizer::Configuration		parser_config;
											
	
			
	Expression::Init	Expression::init;
											
	Expression::Init::Init()
	{
		parser_config.main_separator = "";
		parser_config.recursion_up = "(";
		parser_config.recursion_down = ")";
		parser_config.operators = "+ - * /","'\"","\\";
		parser_config.recursion_break = true;
		parser_config.string_break = true;
	}

	count_t			Variable::counter=1;
	
	float		Entity::adjust(float value)	const
	{
		if (inverted)
			value = 1.0f/value;
		if (negated)
			value *= -1;
		return value;
	}
	
	bool		Element::equals(const Entity*other)	const
	{
		return other && other->type == Type::Element && ((const Element*)other)->reference == reference && (reference!=NULL || value == ((const Element*)other)->value);
	}
	

	String		Element::ToString(bool show_markers)	const
	{
		if (component)
			return *this + '.'+component;
		return *this;
	}
	
	Entity*	Element::clone()	const
	{
		return SHIELDED(new Element(*this));
	}
	
	float		Element::evaluate()	const
	{
		if (reference)
			return adjust(reference->value);
		return adjust(value);
	}
	
	
	void		Element::parse(const String&string, VarContainer&var_container)
	{
		std::cout << ">>>element.parse()"<<std::endl;
		if (!convert(string.c_str(),value))
		{
			reference = var_container.define(string);
			reference->name = string;
			
			if (index_t p = string.GetIndexOf('.'))
			{
				(*(String*)this) = string.subString(0,p-1);
				component = string.get(p);
			}
			else
				(*(String*)this) = string;
		}
		else
			(*(String*)this) = string;
		std::cout << ">>>element.parse() done"<<std::endl;
	}
	
	
	bool		Group::equals(const Entity*other)	const
	{
		if (!other || other->type != type)
			return false;
		if (members.count() != ((const Group*)other)->members.count())
			return false;
		static Array<bool>	flag_array;
		if (members > flag_array.length())
		{
			flag_array.setSize(members);
		}
		flag_array.Fill(false);
			
		for (unsigned i = 0; i < members; i++)
		{
			const Entity*e = members[i];
			bool found = false;
			//std::cout << "  trying to match "<<e->signedToString()<<std::endl;
			for (unsigned j = 0; j < members && !found; j++)
			{
				if (flag_array[j])
				{
					//std::cout<< "   flag set. continueing"<<std::endl;
					continue;
				}
				const Entity*o = ((const Group*)other)->members[j];
				found = (e->inverted == o->inverted) && (e->negated == o->negated) && e->equals(o);
				if (found)
				{
					//std::cout << "   equal to "<<o->signedToString()<<". setting flag"<<std::endl;
					flag_array[j] = true;
				}
			}
			if (!found)
			{
				//std::cout << "  not found"<<std::endl;
				return false;
			}
		}
		return true;
	}
	
	bool		Group::simplify()
	{
		bool result = false;
		
		for (unsigned i = 0; i < members; i++)
			if (members[i]->isGroup())
				result |= ((Group*)members[i])->simplify();
		
		if (type == Type::SumGroup)
		{
			for (unsigned i = 0; i+1 < members; i++)
			{
				Entity*e = members[i];
				//std::cout << "looking for matches of #"<<i<<"/"<<members<<" '"<<e->signedToString()<<"'"<<std::endl;
				int counter=e->negated?-1:1;
				for (unsigned j = i+1; j < members; j++)
				{
					if (e->inverted == members[j]->inverted && e->equals(members[j]))
					{
						//std::cout << " entry #"<<j<<"/"<<members<<" '"<<members[j]->signedToString()<<"' matches"<<std::endl;
						if (members[j]->negated)
							counter --;
						else
							counter ++;
						members.erase(j--);
						result |= true;
					}
				}
				if (!counter)
				{
					//std::cout << " counter hit 0. erasing"<<std::endl;
					members.erase(i--);
				}
				elif (counter != 1)
				{
					if (counter == -1)
					{
						//std::cout << " counter is -1. negating"<<std::endl;
						e->negated = true;
					}
					else
					{
						if (members > 1)
						{
							if (e->type == Type::ProductGroup)
							{
								if (counter < 0)
								{
									e->negated = true;
									counter *= -1;
								}
								else
									e->negated = false;
								((Group*)e)->members.insert(index_t(0),SHIELDED(new Element(counter)));
							}
							else
							{
								//std::cout << " inserting product group for counter "<<counter<<std::endl;
								members.drop(i);
								Group*sub = (Group*)members.insert(i,SHIELDED(new Group(Type::ProductGroup)));
								
								if (counter < 0)
								{
									sub->negated = true;
									counter *= -1;
								}
								else
									e->negated = false;
								sub->members.append(SHIELDED(new Element(counter)));
								sub->members.append(e);
							}
						}
						else
						{
							type = Type::ProductGroup;
							members.insert(index_t(0),SHIELDED(new Element(counter)));
						}
					}
				}
				else
				{	
					//std::cout << "counter is 1. un-negating"<<std::endl;

					e->negated = false;
				}
			}
		}
		else
		{
			for (index_t i = 0; i+1 < members; i++)
			{
				Entity*e = members[i];
				for (index_t j = i+1; j < members; j++)
				{
					if (e->inverted != members[j]->inverted && e->equals(members[j]))
					{
						bool neg = e->negated ^ members[j]->negated;
						members.erase(j);
						members.erase(i);
						Element*cnst = (Element*)members.insert(i,SHIELDED(new Element(1)));
						cnst->negated = neg;
						result |= true;
						break;
					}
				}
			}
		}
		return result;
	}
	
	
	bool		Group::expandNext()
	{
		if (type == Type::SumGroup)	//allready sum-group => little to do
		{
			//std::cout << "expanding sum-group with "<<members<<" element(s)"<<std::endl;
			bool changed = false;
			for (index_t i = 0; i < members; i++)
			{
				if (!members[i]->isGroup())
					continue;
				Group*sub_group = (Group*)members[i];
				if (sub_group->expandNext() && !sub_group->inverted)
				{
					ASSERT_EQUAL__(sub_group->type,Type::SumGroup);	//must now be sum or expansion failed
					changed = true;
					members.drop(i);	//drop group
					for (index_t j = 0; j < sub_group->members; j++)
					{
						//sub_group->members[j]->inverted ^= sub_group->inverted;
						sub_group->members[j]->negated ^= sub_group->negated;
						members.insert(i+j,sub_group->members[j]);
					}
					sub_group->members.flush();
					DISCARD(sub_group);
				}
			}
			//std::cout << "done expanding sum-group. group now has "<<members<<" element(s)"<<std::endl;
			return changed;
		}
		
		//std::cout << "expanding product-group with "<<members<<" element(s)"<<std::endl;
		
		for (index_t i = 0; i < members; i++)
		{
			if (!members[i]->isGroup())
				continue;
			//std::cout << " expanding sub-group #"<<i<<std::endl;
			Group*sub_group = (Group*)members[i];
			sub_group->expandNext();
		}
		
		//std::cout << "sub-groups expanded"<<std::endl;
		
		List::Vector<Entity>	pockets;
		
		for (unsigned i = 0; i < members; i++)
		{
			if (members[i]->type != Type::SumGroup || members[i]->inverted)
				continue;
			//std::cout << " now expanding sub-group #"<<i<<std::endl;
			Group*sub_group = (Group*)members[i];
			
			for (index_t k = 0; k < sub_group->members; k++)
			{
				//std::cout << "  creating pocket #"<<k<<std::endl;
				Group*pocket = (Group*)pockets.append(SHIELDED(new Group(Type::ProductGroup)));
				for (index_t j = 0; j < i; j++)
					pocket->members.append(members[j]->clone());
				Entity*factor = sub_group->members[k]->clone();
				pocket->negated = factor->negated ^ sub_group->negated;
				
				if (factor->type != Type::Element || ((Element*)factor)->reference || ((Element*)factor)->value != 1)
				{
					factor->negated = false;
					//factor->inverted ^= sub_group->inverted;
					pocket->members.append(factor);
				}
				else
					DISCARD(factor);
				for (unsigned j = i+1; j < members; j++)
					pocket->members.append(members[j]->clone());
				
				if (pocket->members == 1)
				{
					pockets.drop(pockets-1);
					Entity*inner = pocket->members.drop(index_t(0));
					//inner->inverted ^= pocket->inverted;
					inner->negated ^= pocket->negated;
					pockets.append(inner);
					DISCARD(pocket);
				}
			}
			break;	//only process one at a time
		}
		//std::cout << "created "<<pockets<<" pocket(s)"<<std::endl;
		if (!pockets)	//no inner sum-groups, can't expand. let's check for product groups
		{
			for (index_t i = 0; i < members; i++)
			{
				if (members[i]->type != Type::ProductGroup)
					continue;
				//std::cout << " now expanding sub-group #"<<i<<std::endl;
				Group*sub_group = (Group*)members.drop(i);
				for (index_t j = 0; j < sub_group->members; j++)
				{
					Entity*inner = sub_group->members[j];
					inner->inverted ^= sub_group->inverted;
					if (!j)
						inner->negated ^= sub_group->negated;
					members.insert(i+j,inner);
				}
				sub_group->members.flush();
				DISCARD(sub_group);
			}
		
			return false;
		}
		members.adoptData(pockets);
		type = Type::SumGroup;
		//expand();
		return true;
	}
	
	float		Group::evaluate()	const
	{
		if (type == Type::SumGroup)
		{
			float rs = 0;
			
			for (index_t i = 0; i < members; i++)
				rs += members[i]->evaluate();
			
			return adjust(rs);
		}
		
		float rs = 1;
		
		for (index_t i = 0; i < members; i++)
			rs *= members[i]->evaluate();
		
		return adjust(rs);
	}
	
	count_t	Group::countOccurrences(const Variable*variable) const
	{
		count_t rs = 0;
		for (index_t i = 0; i < members; i++)
			if (members[i]->type == Type::Element && ((Element*)members[i])->reference == variable)
				rs++;
		return rs;
	}
	
	index_t			Group::find(const Variable*variable)	const
	{
		for (index_t i = 0; i < members; i++)
			if (members[i]->type == Type::Element && ((Element*)members[i])->reference == variable)
				return i;
		return InvalidIndex;
	}
	
	
	
	bool		Expression::factorOut(const String&variable_name)
	{
		Variable*var = variables.lookup(variable_name);
		if (!var)
			return false;
		
		index_t max = 0;
		for (index_t i = 0; i < members; i++)
		{
			if (members[i]->type == Type::ProductGroup)
			{
				Group*group = (Group*)members[i];
				max = vmax(max,group->countOccurrences(var));
			}
			elif (members[i]->type == Type::Element && ((Element*)members[i])->reference == var)
				max = vmax(max,1);
		}
		bool result = false;
		for (index_t i = max; i != 0; i--)
			result |= Group::factorOut(var,i);
		
		return result;
	}
	
	bool		Group::factorOut(const Variable*variable, index_t power)
	{
		std::cout << "attempting to factor out "<<variable->name<<" at power "<<power<<std::endl;
		count_t num_groups = 0;
		for (index_t i = 0; i < members; i++)
		{
			if (members[i]->type == Type::ProductGroup)
			{
				Group*group = (Group*)members[i];
				if (group->countOccurrences(variable) == power)
					num_groups++;
			}
			elif (members[i]->type == Type::Element && ((Element*)members[i])->reference == variable && power == 1)
				num_groups++;
		}
		if (num_groups <= 1)
			return false;
		std::cout << "detected "<<num_groups<<" group(s) that match the specified power"<<std::endl;
	
		Group	*result_group = SHIELDED(new Group(Type::ProductGroup)),
				*sum_group = (Group*)result_group->members.append(SHIELDED(new Group(Type::SumGroup)));
		Element*reference = NULL;
		for (index_t i = 0; i < members; i++)
		{
			if (members[i]->type == Type::ProductGroup)
			{
				Group*group = (Group*)members[i];
				if (group->countOccurrences(variable) != power)
					continue;
				std::cout << "processing group #"<<i<<"/"<<members<<std::endl;
				for (index_t j = 0; j < group->members; j++)
					if (!group->members[j]->inverted && group->members[j]->type == Type::Element && ((Element*)group->members[j])->reference == variable)
					{
						Element*instance = (Element*)group->members.drop(j--);
						group->negated ^= instance->negated;
					
						if (!reference)
							reference = instance;
						else
							DISCARD(instance);
					}
				
				if (group->members)
				{
					members.drop(i--);
					
					if (group->members > 1)
						sum_group->members.append(group);
					else
					{
						Entity*inner = group->members.drop(index_t(0));
						inner->negated = group->negated;
						inner->inverted = group->inverted;
						sum_group->members.append(inner);
						DISCARD(group);
					}
				}
				else
				{
					Element*element = (Element*)sum_group->members.append(SHIELDED(new Element()));
					(*(String*)element) = "1";
					element->value = 1;
					element->negated = group->negated;
					element->inverted = group->inverted;
					members.erase(i--);
				}
			}
			elif (members[i]->type == Type::Element && ((Element*)members[i])->reference == variable && power == 1)
			{
				Element*element = (Element*)members.drop(i--);
				if (!reference)
				{
					reference = element;
					Element*one = (Element*)sum_group->members.append(SHIELDED(new Element()));
					(*(String*)one) = "1";
					one->value = 1;
					one->negated = element->negated;
					one->inverted = element->inverted;
				}
				else
				{
					(*(String*)element) = "1";
					element->value = 1;
					element->reference = NULL;
					sum_group->members.append(element);
				}
			}
		}
		if (reference)
		{
			result_group->members.append(reference);
			reference->negated = false;
			reference->inverted = false;
			
			if (members)
				members.append(result_group);
			else
			{
				members.adoptData(result_group->members);
				DISCARD(result_group);
				type = Type::ProductGroup;
			}
			
			for (index_t i = 1; i < power; i++)
				result_group->members.append(reference->clone());
			
			
			return true;
		}
		DISCARD(result_group);
		return false;
	}
	
	
	Entity*	Group::clone()	const
	{
		Group*result = SHIELDED(new Group(type));
		result->inverted = inverted;
		result->negated = negated;
		for (index_t i = 0; i < members; i++)
			result->members.append(members[i]->clone());
		return result;
	}
	
			
	String		Group::ToString(bool show_markers)	const
	{
		String result;
		if (show_markers)
			result = '<';
		for (index_t i = 0; i < members; i++)
		{
			bool encapsule = false;
			if (i)
			{
				if (type == Type::ProductGroup)
				{
					if (!members[i]->inverted)
						result += '*';
				}
				else
					if (!members[i]->negated)
						result += " + ";
			}
			
			if (members[i]->inverted)
			{
				if (!i)
					result += '1';
				result += '/';
			}
			if (members[i]->negated)
				if (i && type == Type::SumGroup)
					result += " - ";
				else
					result += "-";
					
				
			if (members[i]->type == Type::SumGroup)
				result += '(';
			result += members[i]->ToString(show_markers);
			if (members[i]->type == Type::SumGroup)
				result += ')';
		}
		if (show_markers)
			result += '>';
		return result;
	}
	
	bool		Group::formDeterminants()
	{
		bool changed = false;
		for (index_t i = 0; i < members; i++)
		{
			if (members[i]->isGroup())
				changed |= ((Group*)members[i])->formDeterminants();
		}
		
		if (type != Type::SumGroup)
			return changed;
			
		for (index_t i = 0; i+1 < members; i++)
			if (!members[i]->inverted && members[i]->type == Type::ProductGroup && ((Group*)members[i])->members == 2)
			{
				Group*group = (Group*)members[i];
				if (group->members.first()->type != Type::Element || group->members.last()->type != Type::Element)
					continue;
				Element*x0 = (Element*)group->members.first(),
						*y1 = (Element*)group->members.last();
				if (x0->component == 'y')
					swp(x0,y1);
				if (x0->component != 'x' || y1->component != 'y' || x0->inverted || y1->inverted || x0->negated != y1->negated)
					continue;
				
				for (index_t j = i+1; j < members; j++)
					if (!members[j]->inverted && members[j]->type == Type::ProductGroup && ((Group*)members[j])->members == 2)
					{
						Group*other = (Group*)members[j];
						if (other->members.first()->type != Type::Element || other->members.last()->type != Type::Element)
							continue;
						Element*x1 = (Element*)other->members.first(),
								*y0 = (Element*)other->members.last();
						if (x1->component == 'y')
							swp(x1,y0);
						if (x1->component != 'x' || y0->component != 'y' || x1->inverted || y0->inverted || x1->negated != y0->negated)
							continue;
						if ((x0->negated^group->negated) == (x1->negated^other->negated))
						{
							//std::cout << "group "<<group->ToString()<<" sign matches "<<other->ToString()<<std::endl;
							continue;
						}
						if ((*(String*)x0 != *(String*)y0) || (*(String*)y1 != *(String*)x1) || (*(String*)x0 == *(String*)x1))
							continue;
						
						members.drop(j);
						members.drop(i);
						
						Determinant2*d = (Determinant2*)members.insert(i,SHIELDED(new Determinant2()));
						//d->negated = x0->negated ^ x1->negated;
						if (!(x1->negated^other->negated))
						{
							//swp(x0,x1);
							//swp(y0,y1);
							d->negated = true;
						}
						if (*(String*)x0 > *(String*)x1)
						{
							d->negated = !d->negated;
							swp(x0,x1);
							swp(y0,y1);
						}
						x0->negated = false;
						x1->negated = false;
						y0->negated = false;
						y1->negated = false;
						d->x0 = x0;
						d->y0 = y0;
						d->x1 = x1;
						d->y1 = y1;
						group->members.flush();
						other->members.flush();
						DISCARD(group);
						DISCARD(other);
						changed = true;
						break;
					}
			}
		return changed;
	}
	
	bool		Group::formDeterminant3()
	{
		for (index_t i = 0; i < members; i++)
			if (!members[i]->inverted && members[i]->type == Type::Determinant2)
			{
				Determinant2*d0 = (Determinant2*)members[i];
				if (!d0->x0 || !d0->x1)
					continue;
				for (index_t j = 0; j < members; j++)
					if (i != j && !members[j]->inverted && members[j]->type == Type::Determinant2)
					{
						Determinant2*d1 = (Determinant2*)members[j];
						if (!d1->x0 || !d1->x1)
							continue;
						if (!d0->x0->equals(d1->x0) || d0->x1->equals(d1->x1))
							continue;
						for (index_t k = 0; k < members; k++)
							if (i != k && j != k && !members[k]->inverted && members[k]->type == Type::Determinant2)
							{
								Determinant2*d2 = (Determinant2*)members[k];
								
								if (!d0->x1->equals(d2->x0) || !d1->x1->equals(d2->x1))
									continue;
								
								std::cout << "found matching 2x2 determinant expressions: "<<d0->signedToString()<<", "<<d1->signedToString()<<", "<<d2->signedToString()<<std::endl;
								
								if (i > j)
									swp(i,j);
								if (i > k)
									swp(i,k);
								if (j > k)
									swp(j,k);
									
								members.drop(k);
								members.drop(j);
								members.drop(i);
								
								ASSERT__(d0->x0->equals(d1->x0));
								ASSERT__(d0->x1->equals(d2->x0));
								ASSERT__(d1->x1->equals(d2->x1));
								
								float test = d0->evaluate() + d1->evaluate() + d2->evaluate();
								
								Determinant2x3*det = (Determinant2x3*)members.insert(i,SHIELDED(new Determinant2x3()));
								det->x0 = d0->x0;
								det->y0 = d0->y0;
								det->x1 = d0->x1;
								det->y1 = d0->y1;
								det->x2 = d2->x1;
								det->y2 = d2->y1;
								
								d0->x0 = NULL;
								d0->y0 = NULL;
								d0->x1 = NULL;
								d0->y1 = NULL;
								d2->x1 = NULL;
								d2->y1 = NULL;
								
								bool neg[3] = {d2->negated,!d1->negated,d0->negated};
								
								if (neg[0] && neg[1] && neg[2])
									det->negated = true;
								else
									for (BYTE l = 0; l < 3; l++)
										if (neg[l])
										{
											det->x[l]->negated = true;
											det->y[l]->negated = true;
											det->negated = !det->negated;
										}
								

								
								
								std::cout << "new expression is "<<det->ToString(true)<<". discarding unused expressions"<<std::endl;
								if (test != det->evaluate())
								{
									std::cout << "result mismatch. expected "<<test<<" but got "<<det->evaluate()<<std::endl;
								}
								
								DISCARD(d0);
								DISCARD(d1);
								DISCARD(d2);
								return true;
							}
					}
			}
		return false;
	}
	
	
	bool		Group::formDeterminants3()
	{
		bool changed = false;
		for (index_t i = 0; i < members; i++)
		{
			if (members[i]->isGroup())
				changed |= ((Group*)members[i])->formDeterminants3();
		}
		
		if (type != Type::SumGroup)
			return changed;	
		//attempt to combine each three 2x2 determinants into one 3x3 const determinants

		while (formDeterminant3())	changed = true;

		return changed;
	}
	
	
	bool		Group::parse(const String&expression, VarContainer&var_container)
	{
		std::cout << ">>>group.parse()"<<std::endl;
		std::cout << ">>>members.clear()"<<std::endl;
		members.clear();
		type = Type::SumGroup;
		
		std::cout << ">>>Tokenizer::tokenize()"<<std::endl;
		StringList	items;
		if (!Tokenizer::tokenize(expression,parser_config,items))
		{
			std::cout << ">>>Tokenizer::tokenize() done -> false"<<std::endl;
			flush(std::cout);
			return false;
		}
		std::cout << ">>>Tokenizer::tokenize() done -> true"<<std::endl;
		flush(std::cout);
		
		Group*last_product_group = NULL;
		bool	inverted = false,
				negated = false;
		
		for (index_t i = 0; i < items; i++)
		{
			std::cout << ">>>item #"<<i<<"/"<<items<<".parse()"<<std::endl;
			const String&item = items[i];
			
			if (item == '+' || item == '-')
			{
				last_product_group = NULL;
				negated = item == '-';
				std::cout << ">>>>secondary operator"<<std::endl;
			}
			elif (item == '*' || item == '/')
			{
				std::cout << ">>>>primary operator"<<std::endl;
				
				if (!last_product_group)
				{
					Entity*pre = members.drop(members-1);
					last_product_group = (Group*)members.append(SHIELDED(new Group(Type::ProductGroup)));
					last_product_group->inverted = pre?pre->inverted:false;
					last_product_group->negated = pre?pre->negated:false;
					last_product_group->members.append(pre);
					pre->inverted = false;
					pre->negated = false;
				}
				inverted = item == '/';
				std::cout << ">>>>primary operator done"<<std::endl;
			}
			elif (item.beginsWith('('))
			{
				std::cout << ">>>>item is group: '"<<item<<"'"<<std::endl;
				Group*sub_group = SHIELDED(new Group(Type::ProductGroup));
				if (!sub_group->parse(item.subString(1,item.length()-2),var_container))
				{
					DISCARD(sub_group);
					return false;
				}
				sub_group->inverted = inverted;
				sub_group->negated = negated;
				if (last_product_group)
					last_product_group->members.append(sub_group);
				else
					members.append(sub_group);
				negated = false;
				inverted = false;
				std::cout << ">>>>item done"<<std::endl;
			}
			else
			{
				std::cout << ">>>>other item"<<std::endl;
				Element*element = SHIELDED(new Element());
				element->parse(item,var_container);
				element->inverted = inverted;
				element->negated = negated;
				if (last_product_group)
					last_product_group->members.append(element);
				else
					members.append(element);
				negated = false;
				inverted = false;
				std::cout << ">>>>other item done"<<std::endl;
					
			}
		}
		if (members == 1 && members.first()->type == Type::ProductGroup)
		{
			std::cout << ">>>>collapsing"<<std::endl;
			Group*group = (Group*)members.drop(index_t(0));
			members.adoptData(group->members);
			
			type = Type::ProductGroup;
			for (unsigned i = 0; i < members; i++)
			{
				if (!i)
					members[i]->negated ^= group->negated;
				members[i]->inverted ^= group->inverted;
			}
			
			DISCARD(group);
			std::cout << ">>>>collapsing done"<<std::endl;
		}
	
		std::cout << ">>>group.parse() done"<<std::endl;
		return true;
	}
	
	bool				Expression::parse(const String&expression)
	{
		std::cout << "<<<variables.clear()"<<std::endl;
		variables.clear();
		return Group::parse(expression,variables);
	}
	
	String				Determinant2::ToString(bool show_markers)	const
	{
		if (!x0 || !x1)
			return "||";
		if (show_markers)
			return "|"+x0->ToString()+","+y0->ToString()+" "+x1->ToString()+","+y1->ToString()+"|";
		return "|"+*x0+" "+*x1+"|";
	}
	
	Entity*			Determinant2::clone()		const
	{
		Determinant2*result = SHIELDED(new Determinant2(*this));
		result->x0 = x0?(Element*)x0->clone():NULL;
		result->x1 = x1?(Element*)x1->clone():NULL;
		result->y0 = y0?(Element*)y0->clone():NULL;
		result->y1 = y1?(Element*)y1->clone():NULL;
		return result;
	}
	
	float				Determinant2::evaluate()	const
	{
		if (!x0 || !y0 || !x1 || !y1)
			return 0;
		return adjust(x0->evaluate()*y1->evaluate() - y0->evaluate()*x1->evaluate());
	}
	
	bool				Determinant2::equals(const Entity*other)	const
	{
		if (!other || other->type != Type::Determinant2)
			return false;
		Determinant2*d = (Determinant2*)other;
		return	   ((!x0 && !d->x0) || x0->equals(d->x0))
				&& ((!x1 && !d->x1) || x1->equals(d->x1))
				&& ((!y0 && !d->y0) || y0->equals(d->y0))
				&& ((!y1 && !d->y1) || y1->equals(d->y1));
	}
	
	String				Determinant3Const::ToString(bool show_markers)	const
	{
		if (!x0 || !x1 || !x2)
			return "||";
		if (show_markers)
			return "|"+String(c0)+","+x0->ToString()+","+y0->ToString()+" "+String(c1)+","+x1->ToString()+","+y1->ToString()+" "+String(c2)+","+x2->ToString()+","+y2->ToString()+"|";
		return "|"+String(c0)+","+*x0+" "+String(c1)+","+*x1+" "+String(c2)+","+*x2+"|";
	}
	
	Entity*			Determinant3Const::clone()		const
	{
		Determinant3Const*result = SHIELDED(new Determinant3Const(*this));
		result->x0 = x0?(Element*)x0->clone():NULL;
		result->x1 = x1?(Element*)x1->clone():NULL;
		result->x2 = x2?(Element*)x2->clone():NULL;
		result->y0 = y0?(Element*)y0->clone():NULL;
		result->y1 = y1?(Element*)y1->clone():NULL;
		result->y2 = y2?(Element*)y2->clone():NULL;
		return result;
	}
	
	float				Determinant3Const::evaluate()	const
	{
		if (!x0 || !y0 || !x1 || !y1 || !x2 || !y2)
			return 0;
		return	 adjust(
					 c0*(x1->evaluate()*y2->evaluate() - x2->evaluate()*y1->evaluate())
					-c1*(x0->evaluate()*y2->evaluate() - x2->evaluate()*y0->evaluate())
					+c2*(x0->evaluate()*y1->evaluate() - x1->evaluate()*y0->evaluate())
				);
	}
	
	bool				Determinant3Const::equals(const Entity*other)	const
	{
		if (!other || other->type != Type::Determinant3Const)
			return false;
		Determinant3Const*d = (Determinant3Const*)other;
		return	   ((!x0 && !d->x0) || x0->equals(d->x0))
				&& ((!x1 && !d->x1) || x1->equals(d->x1))
				&& ((!x2 && !d->x2) || x2->equals(d->x2))
				&& ((!y0 && !d->y0) || y0->equals(d->y0))
				&& ((!y1 && !d->y1) || y1->equals(d->y1))
				&& ((!y2 && !d->y2) || y2->equals(d->y2))
				&& c0 == d->c0
				&& c1 == d->c1
				&& c2 == d->c2;
	}
	
	String				Determinant2x3::ToString(bool show_markers)	const
	{
		if (!x0 || !x1 || !x2)
			return "||";
		if (show_markers)
			return "|"+x0->signedToString()+","+y0->signedToString()+" "+x1->signedToString()+","+y1->signedToString()+" "+x2->signedToString()+","+y2->signedToString()+"|";
		return "|"+String(x0->negated?"-":"")+*x0+" "+(x1->negated?"-":"")+*x1+" "+(x2->negated?"-":"")+*x2+"|";
	}
	
	Entity*			Determinant2x3::clone()		const
	{
		Determinant2x3*result = SHIELDED(new Determinant2x3(*this));
		result->x0 = x0?(Element*)x0->clone():NULL;
		result->x1 = x1?(Element*)x1->clone():NULL;
		result->x2 = x2?(Element*)x2->clone():NULL;
		result->y0 = y0?(Element*)y0->clone():NULL;
		result->y1 = y1?(Element*)y1->clone():NULL;
		result->y2 = y2?(Element*)y2->clone():NULL;
		return result;
	}
	
	float				Determinant2x3::evaluate()	const
	{
		if (!x0 || !y0 || !x1 || !y1 || !x2 || !y2)
			return 0;
		return	 adjust(
					 (x1->evaluate()*y2->evaluate() - x2->evaluate()*y1->evaluate())
					-(x0->evaluate()*y2->evaluate() - x2->evaluate()*y0->evaluate())
					+(x0->evaluate()*y1->evaluate() - x1->evaluate()*y0->evaluate())
				);
	}
	
	bool				Determinant2x3::equals(const Entity*other)	const
	{
		if (!other || other->type != Type::Determinant2x3)
			return false;
		Determinant2x3*d = (Determinant2x3*)other;
		return	   ((!x0 && !d->x0) || x0->equals(d->x0))
				&& ((!x1 && !d->x1) || x1->equals(d->x1))
				&& ((!x2 && !d->x2) || x2->equals(d->x2))
				&& ((!y0 && !d->y0) || y0->equals(d->y0))
				&& ((!y1 && !d->y1) || y1->equals(d->y1))
				&& ((!y2 && !d->y2) || y2->equals(d->y2));
	}
	
	
}
