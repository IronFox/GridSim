#include "obj.h"


/******************************************************************

E:\include\geometry_converter\obj.cpp

******************************************************************/

namespace Converter
{





	void			ObjConverter::parseObjPoint(Buffer<TVec3<Def::FloatType> >&target, char*start, char*end)
	{
		BYTE section(0);
		while (*start == ' ')
			start++;
		TVec3<Def::FloatType>&v = target.append();
		while (start < end && section < 3)
		{
			char*seg_end = start;
			while (seg_end < end && *seg_end != ' ')
				seg_end++;
			(*seg_end) = 0;
			Def::FloatType	f = 0;
			convert(start,f);
				v.v[section] = f;

			section++;
			start = seg_end+1;
		}
		for (BYTE k = section; k < 3; k++)
			v.v[k] = 0;
	}

	void			ObjConverter::parseObjPoint(Buffer<TVec2<Def::FloatType> >&target, char*start, char*end)
	{
		BYTE section(0);
		while (*start == ' ')
			start++;
		TVec2<Def::FloatType>&v = target.append();
		while (start < end && section < 2)
		{
			char*seg_end = start;
			while (seg_end < end && *seg_end != ' ')
				seg_end++;
			(*seg_end) = 0;
			Def::FloatType	f = 0;
			convert(start,f);
				v.v[section] = f;

			section++;
			start = seg_end+1;
		}
		for (BYTE k = section; k < 2; k++)
			v.v[k] = 0;
	}

	void			ObjConverter::parseObjFaceVertex(char*start, char*end, int&v, int&n, int&t)
	{
		char*s = start;
		int		index[3];
		bool	set[3]={false,false,false};
		BYTE section(0);
		while (start < end && section < 3)
		{
			char*seg_end = start;
			while (seg_end < end && *seg_end != '/')
				seg_end++;
			(*seg_end) = 0;
			set[section] = convertToInt(start,index[section]);
			section++;
			start = seg_end+1;
		}
		
		v = set[0]?index[0]:0;
		t = set[1]?index[1]:0;
		n = set[2]?index[2]:0;
		
		if ((index_t)abs(v) > vertexBuffer.fillLevel())
		{
			broken_vertex_links++;
			v = 0;
		}
		if ((index_t)abs(n) > normal_buffer.fillLevel())
		{
			broken_normal_links++;
			n = 0;
		}
		if ((index_t)abs(t) > texcoord_buffer.fillLevel())
		{
			broken_texcoord_links++;
			t = 0;
		}
	}


	void			ObjConverter::parseObjFace(char*start, char*end)
	{
		
		static Buffer<int>	ibuffer;
		ibuffer.reset();
		
		while (start < end)
		{
			char*seg_end = start;
			while (seg_end < end && *seg_end != ' ')
				seg_end++;
			(*seg_end) = 0;
			int v,n,t;
			parseObjFaceVertex(start,seg_end,v,n,t);
			v = positiveIndex(v,vertexBuffer.fillLevel());
			if (!ibuffer.fillLevel() || ibuffer[ibuffer.fillLevel()-3]!= v)
			{
				ASSERT1__(v != 0,v);
				ibuffer<<v<<positiveIndex(n,normal_buffer.fillLevel())<<positiveIndex(t,texcoord_buffer.fillLevel()/2);
			}
			start = seg_end+1;
		}
		if (ibuffer.fillLevel()%3)
			FATAL__("bad buffer state ("+String(ibuffer.fillLevel())+")");
		if (ibuffer.fillLevel()==9)
		{
			TObjFace&face = current_group->face_buffer.append();
			face.smooth_group = current_smooth_group;
			face.material = current_material;
			face.v[0] = ibuffer[0];
			face.n[0] = ibuffer[1];
			face.t[0] = ibuffer[2];
			face.v[1] = ibuffer[3];
			face.n[1] = ibuffer[4];
			face.t[1] = ibuffer[5];
			face.v[2] = ibuffer[6];
			face.n[2] = ibuffer[7];
			face.t[2] = ibuffer[8];
			face.v[3] = 0;
			face.n[3] = 0;
			face.t[3] = 0;
		}
		elif (ibuffer.fillLevel()==12)
		{
			TObjFace&face = current_group->face_buffer.append();
			face.smooth_group = current_smooth_group;
			face.material = current_material;
			face.v[0] = ibuffer[0];
			face.n[0] = ibuffer[1];
			face.t[0] = ibuffer[2];
			face.v[1] = ibuffer[3];
			face.n[1] = ibuffer[4];
			face.t[1] = ibuffer[5];
			face.v[2] = ibuffer[6];
			face.n[2] = ibuffer[7];
			face.t[2] = ibuffer[8];
			face.v[3] = ibuffer[9];
			face.n[3] = ibuffer[10];
			face.t[3] = ibuffer[11];
		}
		elif (ibuffer.fillLevel()>12)
		{
			count_t vertices = ibuffer.fillLevel()/3;
			TVec3<Def::FloatType>	normal={0,0,0},
										zero={0,0,0};
			TMatrix4<Def::FloatType>	matrix,
										in_matrix;
							//dim[6],
			TVec3<Def::FloatType>			*v = getVertex(ibuffer[0]);
			for (index_t i = 1; i < vertices-1; i++)
			{
				_oAddTriangleNormal(*v,*getVertex(ibuffer[i*3]),*getVertex(ibuffer[i*3+3]),normal);
			}
			Vec::normalize0(normal);
			Mat::makeAxisSystem(zero,normal,1,matrix);
			Mat::invertSystem(matrix,in_matrix);
			
			Array<TVec2<Def::FloatType> >	tfield(vertices);
			for (index_t i = 0; i < vertices; i++)
			{
				TVec3<float> temp;
				Mat::rotate(in_matrix,*(getVertex(ibuffer[i*3])),temp);
				Vec::def(tfield[i],temp.x,temp.z);
				//_v2(tfield+i*2,getVertex(ibuffer[i*3])[0],getVertex(ibuffer[i*3])[1]);
			}
			
			Mesh<TDef<Def::FloatType> >	triangulated;
			if (_oTriangulate(tfield,triangulated))
				for (index_t i = 0; i < triangulated.triangle_field.length(); i++)
				{
					TObjFace&face = current_group->face_buffer.append();
					face.smooth_group = current_smooth_group;
					face.material = current_material;
					
					index_t	i0 = triangulated.triangle_field[i].v0-triangulated.vertex_field,
							i1 = triangulated.triangle_field[i].v1-triangulated.vertex_field,
							i2 = triangulated.triangle_field[i].v2-triangulated.vertex_field;
					
					face.v[0] = ibuffer[i0*3];
					face.n[0] = ibuffer[i0*3+1];
					face.t[0] = ibuffer[i0*3+2];
					face.v[1] = ibuffer[i1*3];
					face.n[1] = ibuffer[i1*3+1];
					face.t[1] = ibuffer[i1*3+2];
					face.v[2] = ibuffer[i2*3];
					face.n[2] = ibuffer[i2*3+1];
					face.t[2] = ibuffer[i2*3+2];
					face.v[3] = 0;
					face.n[3] = 0;
					face.t[3] = 0;
				}
			else
			{
				logMessage("Warning: Triangulation failed for: (n"+Vec::toString(normal)+") "+String(tfield.length()));
				for (index_t i = 1; i < vertices-1; i++)
				{
					TObjFace&face = current_group->face_buffer.append();
					face.smooth_group = current_smooth_group;
					face.material = current_material;
					
					index_t	i0 = 0,
							i1 = i,
							i2 = i+1;
					
					face.v[0] = ibuffer[i0*3];
					face.n[0] = ibuffer[i0*3+1];
					face.t[0] = ibuffer[i0*3+2];
					face.v[1] = ibuffer[i1*3];
					face.n[1] = ibuffer[i1*3+1];
					face.t[1] = ibuffer[i1*3+2];
					face.v[2] = ibuffer[i2*3];
					face.n[2] = ibuffer[i2*3+1];
					face.t[2] = ibuffer[i2*3+2];
					face.v[3] = 0;
					face.n[3] = 0;
					face.t[3] = 0;
				}
			}
		}
		else
			sendMessage("broken face encountered ("+String(ibuffer.fillLevel()/3)+" segment(s) defined)");

		
		
		
		/*
		String p = "f(";
		for (BYTE k = 0; k < 4; k++)
			p += " "+(face->v[k]?_toString(face->v[k]->p):"NULL");
		p+=")";
		console->printDirect(p);*/
	}

	void			ObjConverter::loadObjColor(const String&line,TVec4<Def::FloatType>&target)
	{
		target.alpha = 1.0;
		Array<String>	segments;
		explode(' ',line,segments);
		if (segments.count() != 3)
			return;
		convert(segments[0].c_str(),target.red);
		convert(segments[1].c_str(),target.green);
		convert(segments[2].c_str(),target.blue);
	}


	ObjTexture*		ObjConverter::loadObjTexture(const String&color_map, const String&alpha_map)
	{
		if (!color_map.length() && !alpha_map.length())
			return NULL;
		String key = color_map+"|"+alpha_map;
		ObjTexture*result;
		if (texture_field.query(key,result))
			return result;
		current_callback->hideConverterProgressBar();
		current_callback->dropLastConverterProgressUpdate();
		sendMessage("Loading '"+key+"'...");
		
		Image	color_image,alpha_image;
		String error;

		bool	color = color_map.length()!=0;
		if (color)
			try
			{
				Magic::loadFromFile(color_image,color_map);
	//			color_image.free();
			}
			catch (const Exception&except)
			{
				error = String(except.getType())+": "+except.what();
				color = false;
			}
			catch (const std::exception&except)
			{
				error = except.what();
				color = false;
			}
			catch (...)
			{
				error = "?";
				color = false;
			}
		
		if (color_map.length() && !color)
		{
			logMessage("Loading of '"+color_map+"' failed ("+error+")");
			sendMessage("Warning: Unable to load '"+color_map+"' ("+error+")!");
		}
		
		bool alpha = alpha_map.length()!=0;
		if (alpha)
			try
			{
				Magic::loadFromFile(alpha_image,alpha_map);
			}
			catch (const Exception&except)
			{
				error = String(except.getType())+": "+except.what();
				alpha = false;
			}
			catch (const std::exception&except)
			{
				error = except.what();
				alpha = false;
			}
			catch (...)
			{
				error = "?";
				alpha = false;
			}
		
		if (alpha_map.length() && !alpha)
		{
			logMessage("Loading of '"+alpha_map+"' failed ("+error+")");
			sendMessage("Warning: Unable to load '"+alpha_map+"' ("+error+")!");
		}
		if (!color && !alpha)
		{
			sendMessage("Parsing");
			return NULL;
		}
		result = texture_field.define(key);
		result->data.name = str2name(FileSystem::extractFileName(color_map)+FileSystem::extractFileName(alpha_map));
		while (texture_name_table64.isSet(result->data.name))
			incrementName(result->data.name);
		texture_name_table64.set(result->data.name);
		
		if (color && alpha)
			color_image.appendAlpha(&alpha_image);
		else
			if (alpha)
			{
				color_image = alpha_image;
				color_image.setChannels(4,Image::CA_REPEAT);
				color_image.setChannel(0,255);
				color_image.setChannel(1,255);
				color_image.setChannel(2,255);
			}
		Array<BYTE>	buffer;
		sendMessage("Compressing...");
		//color_image.free();
		size_t out_size = TextureCompression::compress(color_image,buffer);
		color_image.free();
		if (!out_size)
			logMessage("Compression failed of '"+key+"' ("+TextureCompression::getError()+")");
		
		result->data.face_field.setSize(1);
		result->data.face_field[0].importFrom(buffer.pointer(),(Arrays::count_t)out_size);
		result->data.updateHash();
		
		sendMessage("Parsing");
		
		
		return result;

	}

	ObjTexture*		ObjConverter::loadObjNormalMap(const String&normal_map)
	{
		if (!normal_map.length())
			return NULL;
		String key = normal_map;
		ObjTexture*result;
		if (texture_field.query(key,result))
			return result;
		current_callback->hideConverterProgressBar();
		current_callback->dropLastConverterProgressUpdate();
		sendMessage("Loading '"+key+"'...");
		Image	image;
		try
		{
			Magic::loadFromFile(image,normal_map);
		}
		catch (...)
		{
			return NULL;
		}
		image.setContentType(PixelType::ObjectSpaceNormal);
		
		{
			TVec3<> normal;
			for (UINT32 x = 0; x < image.getWidth(); x++)
				for (UINT32 y = 0; y < image.getHeight(); y++)
				{
					image.getNormal(x,y,normal);
					swp(normal.y,normal.z);
					normal.z*=-1;
					image.setNormal(x,y,normal);
				}
		}
		
		result = texture_field.define(key);
		result->data.name = str2name(FileSystem::extractFileName(normal_map));
		while (texture_name_table64.isSet(result->data.name))
			incrementName(result->data.name);
		texture_name_table64.set(result->data.name);
		
		Array<BYTE>	buffer;
		sendMessage("Compressing...");
		size_t out_size = TextureCompression::compress(image,buffer);

		if (!out_size)
			logMessage("Compression failed of '"+key+"' ("+TextureCompression::getError()+")");
		
		result->data.face_field.setSize(1);
		result->data.face_field[0].importFrom(buffer.pointer(),(Arrays::count_t)out_size);
		result->data.updateHash();
		
		sendMessage("Parsing");
		
		
		return result;

	}


	String			ObjConverter::parseMap(CFSFolder&folder, const String&line, TVec3<>&scale)
	{
		static Tokenizer::Configuration	config(" "," \r\n","","","\'\"","",'\\',true,true,true);
		static StringList	segments;
		Tokenizer::tokenize(line,config,segments);
		Tokenizer::dequote(segments,config);
		/*Array<String>	segments;
		explode(' ',line,segments);*/
		
		//sendMessage("extracting map '"+implode('#',segments)+"'");
		for (index_t i = 1; i < segments.count(); i++)
		{
			String&seg = segments[i];
			if (seg.firstChar() == '-')
			{
				if (seg == "-mm")
					i++;
				elif (seg == "-s")
				{
					if (i+1 < segments.count())
						convert(segments[i+1].c_str(),scale.x);
					if (i+2 < segments.count())
						convert(segments[i+2].c_str(),scale.y);
					if (i+3 < segments.count())
						convert(segments[i+3].c_str(),scale.z);
					i+=2;
				}
				elif (seg == "-o" || seg == "-t")
					i+=2;
				i++;
			}
			else
			{
				const CFSFile*f = folder.findFile(seg);
				if (f)
				{
					sendMessage("parsed map '"+f->getLocation()+"'");
					return f->getLocation();
				}
				
				sendMessage("unable to locate image '"+seg+"'");
				return "";
			}
		}
		return "";
	}

	void			ObjConverter::loadObjMaterialLibrary(const CFSFile*file)
	{
		if (!file)
			return;
		CFSFolder	folder(file->getFolder());
		if (!folder.validLocation())
		{
			FATAL__("unable to locate material folder");
			return;
		}
		StringFile	in(file->getLocation());
		if (!in.isActive())
			return;
		ObjMaterial*material(NULL);
		String line,
				color_map,
				transparency_map,
				bump_map;
		
		while (in >> line)
		{
			line.trimThis();
			switch (line.firstChar())
			{
				case 'b':
					if (line.beginsWith("bump ") && material)
					{
						bump_map = parseMap(folder,line,material->scale);
						logMessage("bump map set to '"+bump_map+"'");
					}
				break;
				case 'n':
					if (line.beginsWith("newmtl"))
					{
						if (color_map.length() || transparency_map.length() || bump_map.length())
						{
							if (material)
							{
								material->texture = loadObjTexture(color_map,transparency_map);
								material->normal_map = loadObjNormalMap(bump_map);
								if (bump_map.length() && material->normal_map)
								{
									sendMessage("bump map loaded '"+bump_map+"'");
								}
								
								count_t layers = (material->texture != NULL) + (material->normal_map!=NULL);
								sendMessage("material '"+material->name+"' has "+String(layers)+" layer(s)");
								material->info.layer_field.setSize(layers);
								for (index_t i = 0; i < layers; i++)
								{
									CGS::TLayer&layer = material->info.layer_field[i];
									layer.combiner = 0x2100;
									layer.cube_map = false;
									layer.mirror_map = false;
									layer.clamp_x = false;
									layer.clamp_y = false;
								}
							}
						}
						color_map = "";
						transparency_map = "";
						bump_map = "";
						material = material_list.append();
						material->name = line.subString(7);
						material_table.set(material->name,material);
						logMessage("created material '"+material->name+"'");
					}
				break;
				case 'm':
					if (material)
					{
						if (line.beginsWithCaseIgnore("map_Kd "))
						{
							color_map = parseMap(folder,line,material->scale);
							logMessage("color map set to '"+color_map+"'");
						}
						elif (line.beginsWithCaseIgnore("map_d "))
						{
							transparency_map = parseMap(folder,line,material->scale);
							logMessage("transparency map set to '"+transparency_map+"'");
						}
						else
							logMessage("Warning: trying to set unknown map type '"+line.firstWord()+"'");
					}
					else
						logMessage("Warning: trying to set map without material");
				break;
				case 'K':
					if (material)
						switch (line.get(1))
						{
							case 'a':
								loadObjColor(line.c_str()+3,material->info.ambient);
								Vec::addVal(material->info.ambient,0.1);
								Vec::clamp(material->info.ambient,0,1);
							break;
							case 'd':
								loadObjColor(line.c_str()+3,material->info.diffuse);
								Vec::clamp(material->info.diffuse,0,1);
							break;
							case 's':
								loadObjColor(line.c_str()+3,material->info.specular);
							break;
						}
				break;
				case 'N':
					if (line.get(1) == 's' && material && convert(line.c_str()+3,material->info.shininess))
					{
						material->info.shininess /= 10.0;
						material->info.shininess = clamped(material->info.shininess,0.3,1);
					}
				break;
			}
		}
		
		if (color_map.length() || transparency_map.length() || bump_map.length())
		{
			if (material)
			{
				material->texture = loadObjTexture(color_map,transparency_map);
				material->normal_map = loadObjNormalMap(bump_map);
				if (bump_map.length() && material->normal_map)
				{
					sendMessage("bump map loaded '"+bump_map+"'");
				}
				
				count_t layers = (material->texture != NULL) + (material->normal_map!=NULL);
				material->info.layer_field.setSize(layers);
				for (index_t i = 0; i < layers; i++)
				{
					CGS::TLayer&layer = material->info.layer_field[i];
					layer.combiner = 0x2100;
					layer.cube_map = false;
					layer.mirror_map = false;
					layer.clamp_x = false;
					layer.clamp_y = false;
				}
			}
		}
		
	}

	void			ObjConverter::parseObjLine(char*start, char*end)
	{
		if (end==start || *start == '#')
			return;
		switch (*start)
		{
			case 'f':
				if (current_group && end-start>2)
				{
					if (!current_material)
					{
						current_material = material_list.append();
						current_material->name = "nil";
					}
					parseObjFace(start+2,end);
				}
			break;
			case 'v':
				if (end-start>3)
				{
					if (start[1] == ' ')
						parseObjPoint(vertexBuffer,start+2,end);
					elif (start[1] == 'n')
						parseObjPoint(normal_buffer,start+3,end);
					elif (start[1] == 't')
						parseObjPoint(texcoord_buffer,start+3,end);
				}
			break;
			case 'g':
			{
				String name = end-start>2?start+2:"";
				if (!object_name_table.query(name,current_group))
				{
					current_group = group_buffer.append();
					object_name_table.set(name,current_group);
					name.trimThis().replace(' ','_');
					if (!name.length())
						name = "unnamend";
					
					current_group->name = name;
					while (object_name_table64.isSet(current_group->name))
						current_group->name+='0';
					logMessage("starting new group '"+current_group->name+"'");
					object_name_table64.set(current_group->name);
				}
				else
					logMessage("continuing existing group '"+current_group->name+"'");
				if (current_material)
					logMessage("using material '"+current_material->name+"'");
			}
				// current_smooth_group = current_group->face_buffer.define(current_smooth_group_index);
			break;
			case 's':
			{
				unsigned	group;
				if (convert(start+2,group))
				{
					if (!smooth_group_table.query(group,current_smooth_group))
					{
						current_smooth_group = smooth_groups++;
						smooth_group_table.set(group,current_smooth_group);
						logMessage("smooth group not previously encountered ("+String(group)+") - mapped to "+String(current_smooth_group));
					}
					else
						logMessage("smooth group reencountered ("+String(group)+") - mapped to "+String(current_smooth_group));
				}


				// if (current_group && convertToUnsigned(start+2,current_smooth_group_index))
						// current_smooth_group = current_group->face_buffer.define(current_smooth_group_index);
			}
			break;
			case 'u':
				if (!strncmp(start,"usemtl",6) && end-start>6)
					if (!material_table.query(start+7,current_material))
						logMessage("unable to find material '"+String(start+7)+"'");
					else
						logMessage("using material '"+String(start+7)+"'");
			break;
			case 'm':
				if (!strncmp(start,"mtllib",6) && end-start>6)
					loadObjMaterialLibrary(object_system.findFile(start+7));
			break;
		}
	}

	void			ObjConverter::resetImport()
	{
		vertexBuffer.reset();
		normal_buffer.reset();
		texcoord_buffer.reset();
		group_buffer.clear();
		current_group = NULL;
		current_material = NULL;
		//current_smooth_group = NULL;
		// current_smooth_group_index = 0;
		material_table.clear();
		material_list.clear();
		texture_field.clear();
		object_name_table.clear();
		object_name_table64.clear();
		texture_name_table64.clear();
		broken_vertex_links = 0;
		broken_normal_links = 0;
		broken_texcoord_links = 0;
		
		current_smooth_group = 0;
		smooth_groups = 1;
		smooth_group_table.clear();
		smooth_group_table.set(0,0);
	}








	bool	ObjConverter::read(CGS::Geometry<>&target, const String&filename)
	{
		object_system = FileSystem::workingDirectory();
		if (!object_system.moveTo(FileSystem::extractFilePath(filename)))
		{
			last_error = "Unable to move to file folder";
			return false;
		}
		FILE*f = fopen(filename.c_str(),"rb");
		if (!f)
		{
			last_error = "File not readable";
			return false;
		}
		
		sendMessage("Reading data...");

		fseek(f,0,SEEK_END);
		
		unsigned long fsize = (unsigned long)ftell(f);
		

		resetImport();
		//current_material = material_list.append();
		//current_material->name = "nil";
		fseek(f,0,SEEK_SET);
		
		Array<char>	file_content(fsize+1);
		
		if (!fread(file_content.pointer(), fsize, 1, f))
		{
			fclose(f);
			last_error = "File not readable";
			return false;
		}
		fclose(f);
		
		file_content[fsize] = 0;
		
		sendMessage("Parsing");
		
		char	*line = file_content.pointer(),
				*end = file_content+fsize;
		
		count_t	update_step =fsize/100,
				at = 0;
		if (!update_step)
			update_step = 1;
		while (line != end)
		{
			if ((line-file_content)/update_step != at)
			{
				at = (line-file_content)/update_step;
				setProgress((float)(line-file_content)/(float)fsize);
			}
				
			char*read = line;
			while (*read != '\r' && *read != '\n')
				read++;
			*read = 0;
			parseObjLine(line,read);
			read++;
			if (*read == '\n')
				(*read++) = 0;
			line = read;
		}
		
		file_content.free();

		count_t total = 0;
		for (UINT32 i = 0; i < group_buffer.count(); i++)
			total += group_buffer[i]->face_buffer.count();

		group_buffer.reset();
		while (ObjFaceGroup*group = group_buffer.each())
			if (!group->face_buffer.fillLevel())
			{
				logMessage("Group '"+group->name+"' has no faces. Erasing...");
				group_buffer.erase();
			}
		
		#define	VINDEX(INDEX, FROM)	((INDEX) < 0?(int)FROM.fillLevel()+(int)(INDEX):((int)(INDEX)-1))
		
		
		#define GETC(FROM,INDEX,TO)\
				{\
					if (INDEX)\
					{\
						if ((INDEX) < 0)\
							TO = FROM[(int)(FROM.fillLevel())+(INDEX)];\
						else\
							TO = FROM[(INDEX-1)];\
					}\
				}
				
		#define GETN(vindex,nindex,vertex)\
				{\
					GETC(vertexBuffer,vindex,vertex.p);\
					GETC(normal_buffer,nindex,vertex.n);\
				}
					
		#define GETNT(vindex,nindex,tindex,vertex)\
				{\
					GETC(vertexBuffer,vindex,vertex.p);\
					GETC(normal_buffer,nindex,vertex.n);\
					GETC(texcoord_buffer,tindex,vertex.t);\
				}
			
		
		
		bool must_normalize=true;
			//must_normalize_texcoords=false;
		if (normal_buffer.fillLevel())
		{
			must_normalize = false;
			for (unsigned i = 0; i < 10 && !must_normalize; i++)
			{
				index_t index = (i*normal_buffer.fillLevel())/10;
				must_normalize = fabs(Vec::dot(normal_buffer[index])-1.0f) > 0.01;
			}
		}
/*		if (texcoord_buffer.fillLevel()/3)
		{
			for (unsigned i = 0; i < 10 && !must_normalize_texcoords; i++)
			{
				unsigned index = (i*texcoord_buffer.fillLevel()/3)/10;
				float x = texcoord_buffer[index*3],
						y = texcoord_buffer[index*3+1];
				must_normalize_texcoords = x < 0 || y < 0 || x > 1 || y > 1;
			}
		
		}
		if (must_normalize_texcoords)
		{
			sendMessage("Provided texcoords are out of range.");
			sendMessage("Normalizing texcoords");
			float	min_x = texcoord_buffer[0],
					min_y = texcoord_buffer[1],
					max_x = texcoord_buffer[0],
					max_y = texcoord_buffer[1];
			
			unsigned total = texcoord_buffer.fillLevel()/3;
			update_step = total/100;
			for (unsigned i = 1; i < total; i++)
			{
				if (!(i%update_step))
					setProgress((float)i/(float)total/2.0);
				float	x = texcoord_buffer[i*3],
						y = texcoord_buffer[i*3+1];
				if (x < min_x)
					min_x = x;
				if (x > max_x)
					max_x = x;
				if (y < min_y)
					min_y = y;
				if (y > max_y)
					max_y = y;
			}
			float	step_x = max_x-min_x,
					step_y = max_y-min_y;
			for (unsigned i = 0; i < total; i++)
			{
				if (!(i%update_step))
					setProgress(0.5+(float)i/(float)total/2.0);
				texcoord_buffer[i*3] = (texcoord_buffer[i*3]-min_x)/step_x;
				texcoord_buffer[i*3+1] = (texcoord_buffer[i*3+1]-min_y)/step_y;
			}
			current_callback->hideConverterProgressBar();
			sendMessage("Normalized. Texcoord rectangle was "+String(min_x)+", "+String(min_y)+" - "+String(max_x)+", "+String(max_y));
			
		}*/
		
		if (must_normalize)
		{
			sendMessage("Provided normals are non-existing or not properly generated.");
			sendMessage("Generating normals");
			normal_buffer.setSize(vertexBuffer.fillLevel()*smooth_groups,true);
			normal_buffer.fill(Vector<>::null);
			index_t index = 0;
			update_step = total/100;
			if (!update_step)
				update_step = 1;
			for (index_t i = 0; i < group_buffer.count(); i++)
			{
				ObjFaceGroup*group = group_buffer[i];
				logMessage("calculating normals of group '"+group->name+"'");
				for (index_t j = 0; j < group->face_buffer.fillLevel(); j++)
				{
					TObjFace&face = group->face_buffer[j];
					if (!(index++%update_step))
						setProgress((float)index/(float)total);
					int		i0 = VINDEX(face.v[0],vertexBuffer),
							i1 = VINDEX(face.v[1],vertexBuffer),
							i2 = VINDEX(face.v[2],vertexBuffer),
							i3 = VINDEX(face.v[3],vertexBuffer);
					//ASSERT_LESS__((index_t)i0,vertexBuffer.size());
					//ASSERT_LESS__((index_t)i1,vertexBuffer.size());
					//ASSERT_LESS__((index_t)i2,vertexBuffer.size());
					//if (face.v[3] != 0)
					//	ASSERT_LESS__((index_t)i3,vertexBuffer.size());
					TVec3<>	*t0 = vertexBuffer+i0,
							*t1 = vertexBuffer+i1,
							*t2 = vertexBuffer+i2,
							*t3 = face.v[3]?vertexBuffer+i3:NULL,
							normal;
					_oTriangleNormal(*t0,*t1,*t2,normal);
					if (face.v[3])
						_oAddTriangleNormal(*t0,*t2,*t3,normal);
					Vec::add(normal_buffer[i0*smooth_groups+face.smooth_group],normal);
					Vec::add(normal_buffer[i1*smooth_groups+face.smooth_group],normal);
					Vec::add(normal_buffer[i2*smooth_groups+face.smooth_group],normal);
					if (t3)
						Vec::add(normal_buffer[i3*smooth_groups+face.smooth_group],normal);
					face.n[0] = i0*smooth_groups+face.smooth_group+1;
					face.n[1] = i1*smooth_groups+face.smooth_group+1;
					face.n[2] = i2*smooth_groups+face.smooth_group+1;
					face.n[3] = face.v[3]?i3*smooth_groups+face.smooth_group+1:0;
				}
			}
			for (index_t i = 0; i < vertexBuffer.fillLevel()*smooth_groups; i++)
				if (Vec::dot(normal_buffer[i])>0)
					Vec::normalize(normal_buffer[i]);
		}
				

		current_callback->hideConverterProgressBar();
		sendMessage("Extracted "+String(vertexBuffer.fillLevel())+" vertices, "+String(total)+" faces, "+String(group_buffer.count())+" groups, and "+String(material_list.count())+" materials");
		if (broken_vertex_links)
			sendMessage("Broken vertex links encountered: "+String(broken_vertex_links));
		if (broken_normal_links)
			sendMessage("Broken normal links encountered: "+String(broken_normal_links));
		if (broken_texcoord_links)
			sendMessage("Broken texcoord links encountered: "+String(broken_texcoord_links));
		

		update_step = total/100;
		if (!update_step)
			update_step = 1;
		UINT32 index = 0;


		sendMessage("Mapping");
		
//	target.object_field.



		target.object_field.setSize(group_buffer.count());
		
		for (index_t i = 0; i < group_buffer.count(); i++)
		{
			ObjFaceGroup*group = group_buffer[i];
			logMessage("mapping group '"+group->name+"'");
			group->target = target.object_field+i;
			
			target.object_field[i].name = group->name;
			target.object_table.set(group->name).set(1,i);
			for (index_t j = 0; j < group->face_buffer.fillLevel(); j++)
			{
				const TObjFace&face = group->face_buffer[j];
				if (!(index++%update_step))
					setProgress((float)index/(float)total);
				ObjMaterial*m = face.material;
				if (!m->selected_section || m->selected_section->target != group)
				{
					m->selected_section = m->sections.append();
					m->selected_section->target = group;
				}
				ObjMaterialSection*s = m->selected_section;
				if (m->info.layer_field.length())
				{
					ObjVertexNT	v(0.0);
					if (face.v[3]!=0)
					{
						
						TObjQuadT&q = s->tex_quads.append();
						GETNT(face.v[0],face.n[0],face.t[0],v);
						q.v[0] = s->tex_vertices.add(v);
						GETNT(face.v[1],face.n[1],face.t[1],v);
						q.v[1] = s->tex_vertices.add(v);
						GETNT(face.v[2],face.n[2],face.t[2],v);
						q.v[2] = s->tex_vertices.add(v);
						GETNT(face.v[3],face.n[3],face.t[3],v);
						q.v[3] = s->tex_vertices.add(v);
						
						
					}
					else
					{

						TObjTriangleT&q = s->tex_triangles.append();
						GETNT(face.v[0],face.n[0],face.t[0],v);
						q.v[0] = s->tex_vertices.add(v);
						GETNT(face.v[1],face.n[1],face.t[1],v);
						q.v[1] = s->tex_vertices.add(v);
						GETNT(face.v[2],face.n[2],face.t[2],v);
						q.v[2] = s->tex_vertices.add(v);
						

					}
				}
				else
				{
					ObjVertexN	v(0.0);
					if (face.v[3]!=0)
					{
						TObjQuad&q = s->quads.append();
						GETN(face.v[0],face.n[0],v);
						q.v[0] = s->vertices.add(v);
						GETN(face.v[1],face.n[1],v);
						q.v[1] = s->vertices.add(v);
						GETN(face.v[2],face.n[2],v);
						q.v[2] = s->vertices.add(v);
						GETN(face.v[3],face.n[3],v);
						q.v[3] = s->vertices.add(v);
					}
					else
					{
						TObjTriangle&q = s->triangles.append();
						GETN(face.v[0],face.n[0],v);
						q.v[0] = s->vertices.add(v);
						GETN(face.v[1],face.n[1],v);
						q.v[1] = s->vertices.add(v);
						GETN(face.v[2],face.n[2],v);
						q.v[2] = s->vertices.add(v);
					}
				}
			}
			logMessage("group '"+group->name+"' mapped");
			
		}
		
		Array<ObjTexture*>	exp;
		texture_field.exportTo(exp);
		
		target.local_textures.entry_field.setSize(exp.length());
		target.texture_resource = &target.local_textures;
		for (index_t i = 0; i < exp.length(); i++)
		{
			CGS_POINT
			target.local_textures.entry_field[i].adoptData(exp[i]->data);
			exp[i]->target = target.local_textures.entry_field+i;
			if (target.local_textures.entry_field[i].face_field.length())
				logMessage(target.local_textures.entry_field[i].face_field[0].size());
			CGS_POINT
		}
		
		index = 0;
		UINT32 last_step = 0;
		current_callback->hideConverterProgressBar();
		
		material_list.reset();
		while (ObjMaterial*material = material_list.each())
			if (!material->sections)
			{
				logMessage("Material '"+material->name+"' has no sections. erasing...");
				material_list.erase();
			}
		
		count_t render_vertices(0);
		sendMessage("Writing materials");
		target.material_field.setSize(material_list.count());
		for (index_t i = 0; i < material_list.count(); i++)
		{
			ObjMaterial*material = material_list[i];
			logMessage("Writing material '"+material->name+"'");
			CGS::MaterialA<>&mt = target.material_field[i];
			mt.data.coord_layers = unsigned(material->info.layer_field.length());
			mt.name = material->name;
			mt.info.adoptData(material->info);
			if (mt.info.layer_field.length())
			{
				mt.info.layer_field[0].source = material->texture?material->texture->target:material->normal_map->target;
				if (mt.info.layer_field.length()>1)
					mt.info.layer_field[1].source = material->normal_map->target;
			}
			mt.data.object_field.setSize(material->sections);
			for (index_t j = 0; j < mt.data.object_field.length(); j++)
			{
				ObjMaterialSection*s = material->sections[j];
				logMessage(" Writing section '"+s->target->name+"'");
				CGS::RenderObjectA<>&ro = mt.data.object_field[j];
				ro.target = s->target->target;
				ro.tname = s->target->target->name;
				CGS::IndexContainerA<>&chunk = ro.ipool;
				if (s->vertices.count())
				{
					Array<ObjVertexN>	vertex_field;
					s->vertices.exportToField(vertex_field);

					render_vertices += s->vertices.count();
					ro.vpool.setSize(s->vertices,0,CGS::HasNormalFlag);
					for (UINT32 k = 0; k < ro.vpool.vcnt; k++)
					{
						const ObjVertexN&v = vertex_field[k];
						Vec::ref3(ro.vpool.vdata+k*6) = v.p;
						Vec::ref3(ro.vpool.vdata+k*6+3) = v.n;

						if (must_normalize && vabs(Vec::dot(v.n)-1.0) > 0.01)
							sendMessage("Conversion error: Normal of vertex "+String(k)+" of render object "+String(j)+" of material "+String(i)+" is not normalized ("+Vec::toString(v.n)+")");
						//s->vertices[k]->index = k;
					}
					chunk.idata.setSize(s->triangles.count()*3+s->quads.count()*4);
					chunk.triangles = UINT32(s->triangles.count());
					chunk.quads = UINT32(s->quads.count());
					UINT32*pi = chunk.idata.pointer();
					for (index_t k = 0; k < s->triangles.count(); k++)
					{
						const TObjTriangle&t = s->triangles[k];
						(*pi++) = t.v[0];
						(*pi++) = t.v[1];
						(*pi++) = t.v[2];
					}
					for (index_t k = 0; k < s->quads.count(); k++)
					{
						const TObjQuad&t = s->quads[k];
						(*pi++) = t.v[0];
						(*pi++) = t.v[1];
						(*pi++) = t.v[2];
						(*pi++) = t.v[3];
					}
					if (pi != chunk.idata+chunk.idata.length())
						FATAL__("triangle fill exception");
					index += static_cast<UINT32>(s->triangles.count()+s->quads.count());
				}
				else
				{
					Array<ObjVertexNT>	vertex_field;
					s->tex_vertices.exportToField(vertex_field);

					render_vertices += s->tex_vertices.count();
					ro.vpool.setSize(s->tex_vertices,1,CGS::HasNormalFlag);
					for (UINT32 k = 0; k < ro.vpool.vcnt; k++)
					{
						const ObjVertexNT&v = vertex_field[k];
						Vec::ref3(ro.vpool.vdata+k*8) =v.p;
						Vec::ref3(ro.vpool.vdata+k*8+3) =v.n;
						Vec::ref2(ro.vpool.vdata+k*8+6) =v.t;
						if (must_normalize && vabs(Vec::dot(v.n)-1.0) > 0.01)
							sendMessage("Conversion error: Normal of vertex "+String(k)+" of render object "+String(j)+" of material "+String(i)+" is not normalized ("+Vec::toString(v.n)+")");
						//s->tex_vertices[k]->index = k;
					}
					chunk.idata.setSize(s->tex_triangles.count()*3+s->tex_quads.count()*4);
					chunk.triangles = UINT32(s->tex_triangles.count());
					chunk.quads = UINT32(s->tex_quads.count());
					UINT32*pi = chunk.idata.pointer();

					for (index_t k = 0; k < s->tex_triangles.count(); k++)
					{
						const TObjTriangleT&t = s->tex_triangles[k];
						(*pi++) = t.v[0];
						(*pi++) = t.v[1];
						(*pi++) = t.v[2];
					}
					for (index_t k = 0; k < s->tex_quads.count(); k++)
					{
						const TObjQuadT&t = s->tex_quads[k];
						(*pi++) = t.v[0];
						(*pi++) = t.v[1];
						(*pi++) = t.v[2];
						(*pi++) = t.v[3];
					}
					if (pi != chunk.idata+chunk.idata.length())
						FATAL__("triangle fill exception");
					
					index += UINT32(s->tex_triangles.count()+s->tex_quads.count());
				}
				if (index/update_step>last_step)
				{
					setProgress((float)index/(float)total);
					last_step = index/UINT32(update_step);
				}
			}
		
		}
		
		index = 0;
		update_step = total/100;
		if (!update_step)
			update_step = 1;
		
		count_t vertices = render_vertices;//vertexBuffer.fillLevel()/3;	//3 floats per vertex
		Array<UINT32>	history(vertexBuffer.fillLevel());
		
		//PointerTable<bool>	inversion_field;	//map of all objects with negative volume

		static const UINT32 invalid_index = (UINT32)-1;
		Buffer<TPoolTriangle>	triangle_buffer;
		Buffer<TPoolQuad>		quad_buffer;
		
		sendMessage("Creating geometric hull");
		for (index_t i = 0; i < group_buffer.count(); i++)
		{
			logMessage(__LINE__);
			CGS::SubGeometryA<>&child = target.object_field[i];
		
			history.fill(invalid_index);
			triangle_buffer.reset();
			quad_buffer.reset();
		
			VertexHashTable<PoolVertex>	pool;

			//Sorted<List::Vector<PoolVertex>,OperatorSort>	pool;
			logMessage(__LINE__);
			

			ObjFaceGroup*group = group_buffer[i];
		
			for (index_t j = 0; j < group->face_buffer.fillLevel(); j++)
			{
				const TObjFace&face = group->face_buffer[j];
				if (!(index++%update_step))
					setProgress((float)index/(float)total);
				if (face.v[3]!=0)
				{
					TPoolQuad&q = quad_buffer.append();
					for (BYTE k = 0; k < 4; k++)
					{
						count_t	vi = face.v[k]<0?vertices-face.v[k]:face.v[k]-1;
						ASSERT3__(vi < vertices,vi,vertices,face.v[k]);
						UINT32	v = history[vi];
						if (v == invalid_index)
							v = history[vi] = pool.add(vertexBuffer[vi]);
						q.v[k] = v;
					}
				}
				else
				{
					TPoolTriangle&q = triangle_buffer.append();
					for (BYTE k = 0; k < 3; k++)
					{
						index_t	vi = face.v[k]<0?vertexBuffer.fillLevel()-face.v[k]:face.v[k]-1;
						UINT32 v = history[vi];
						if (v == invalid_index)
							v = history[vi] = pool.add(vertexBuffer[vi]);
						q.v[k] = v;
					}
				}
			}
			
			logMessage(__LINE__);
			
			
			child.vs_hull_field.setSize(1);
			Mesh<CGS::SubGeometryA<>::VsDef>	&vs_hull = child.vs_hull_field[0];
			vs_hull.vertex_field.setSize(pool.count());
			
			Mat::eye(child.meta.system);
			child.meta.volume = 1.0;
			child.meta.density = 1.0;
			Vec::clear(child.meta.center);
			logMessage(__LINE__);

			Array<PoolVertex>	vertex_field;
			pool.exportToField(vertex_field);
			
			if (vertex_field.length())
			{
				Box<Def::FloatType> dim(vertex_field.first().p,vertex_field.first().p);
				//pool[0]->index = 0;

				vs_hull.vertex_field[0].position = dim.min();
			
				for (index_t j = 1; j < pool.count(); j++)
				{
					vs_hull.vertex_field[j].position = vertex_field[j].p;
					//pool[j]->index = j;
					_oDetDimension(vs_hull.vertex_field[j].position,dim);
				}

				logMessage("extracted dimensions of '"+child.name+"': "+dim.ToString());
				
				dim.GetCenter(child.meta.system.w.xyz);
				for (index_t j = 0; j < vs_hull.vertex_field.length(); j++)
					Vec::sub(vs_hull.vertex_field[j].position,child.meta.system.w.xyz);
					
			}
			logMessage(__LINE__);
			
			vs_hull.triangle_field.setSize(triangle_buffer.count());
			for (index_t j = 0; j < triangle_buffer.count(); j++)
			{
				const TPoolTriangle&t = triangle_buffer[j];
				vs_hull.triangle_field[j].v0 = vs_hull.vertex_field+t.v[0];
				vs_hull.triangle_field[j].v1 = vs_hull.vertex_field+t.v[1];
				vs_hull.triangle_field[j].v2 = vs_hull.vertex_field+t.v[2];
			}
			logMessage(__LINE__);
			
			vs_hull.quad_field.setSize(quad_buffer.count());
			for (index_t j = 0; j < quad_buffer.count(); j++)
			{
				const TPoolQuad&q = quad_buffer[j];
				ASSERT3__(q.v[0]<vs_hull.vertex_field.length(),j,q.v[0],vs_hull.vertex_field.length());
				ASSERT3__(q.v[1]<vs_hull.vertex_field.length(),j,q.v[1],vs_hull.vertex_field.length());
				ASSERT3__(q.v[2]<vs_hull.vertex_field.length(),j,q.v[2],vs_hull.vertex_field.length());
				ASSERT3__(q.v[3]<vs_hull.vertex_field.length(),j,q.v[3],vs_hull.vertex_field.length());
				vs_hull.quad_field[j].v0 = vs_hull.vertex_field+q.v[0];
				vs_hull.quad_field[j].v1 = vs_hull.vertex_field+q.v[1];
				vs_hull.quad_field[j].v2 = vs_hull.vertex_field+q.v[2];
				vs_hull.quad_field[j].v3 = vs_hull.vertex_field+q.v[3];
			}
			logMessage(__LINE__);
			
			//vs_hull.generateEdges();
			if (!vs_hull.valid())
			{
				logMessage(vs_hull.errorStr());
				vs_hull.correct();
				ASSERT1__(vs_hull.valid(),vs_hull.errorStr());
			}

			
			child.meta.volume = vs_hull.getVolume(false);
			if (child.meta.volume < 0)
			{
				child.meta.volume *= -1;
				//vs_hull.invert();				//<- no good. volume calculation failes sometimes if hull is not closed
				//inversion_field.set(&child,true);
			}
			logMessage(__LINE__);
			
		}
		
		sendMessage("Relocating/inverting render objects");
		
		update_step = render_vertices/100;
		if (!update_step)
			update_step = 1;
		index = 0;
		
		for (index_t i = 0; i < target.material_field.length(); i++)
		{
			for (index_t j = 0; j < target.material_field[i].data.object_field.length(); j++)
			{
				CGS::RenderObjectA<>&robj = target.material_field[i].data.object_field[j];
				unsigned vsize = robj.vpool.vsize();
				Def::FloatType*p = robj.vpool.vdata.pointer();
				for (UINT32 k = 0; k < robj.vpool.vcnt; k++)
				{
					Vec::sub(Vec::ref3(p),robj.target->meta.system.w.xyz);
					if (!(index++%update_step))
						setProgress((float)index/(float)render_vertices);
					p += vsize;
				}
				/*if (inversion_field.isSet(robj.target))
					robj.invert();*/
			}
		}

		
		target.root_system.loadIdentity(false);
		target.system_link = &target.root_system.matrix;
		target.resetSystems();
		
		
		sendMessage("Done");
		
		/*
			Buffer<Def::FloatType>				vertexBuffer,normal_buffer,texcoord_buffer;
			CFSFolder							object_system;
			Vector<ObjFaceGroup>				group_buffer;
			ObjFaceGroup						*current_group;
			ArrayList<TObjFace,512> 			*current_smooth_group;
			unsigned							current_smooth_group_index;
			Vector<CGS::Material<>	>		material_list;
			HashTable<CGS::Material<>*>	material_table;
			CGS::Material<>*				current_material;
			HashContainer<CGS::Texture>		texture_field;
		*/		
		
		resetImport();

		return true;
	
	}



	
	void	ObjConverter::exportTexture(const CGS::TextureA*texture, const String&outname, StringFile&fout)
	{
	if (!texture)
		return;
	Image out;
	
	String string;
	if (texture_names.query(texture,string))
	{
		sendMessage("  Texture already exported.");
		if (TextureCompression::extractContentType(texture->face_field[0]) == PixelType::Color)
		{
			fout << "map_Kd "<<string<<nl;

			if (alpha_names.query(texture,string))
				fout << "map_d "<<string<<nl;
		}
		else
			fout << "bump "<<string<<nl;
		return;
	}
	
	
	if (!texture->face_field.length())
		return;

	String tex_name = name2str(texture->name).trimThis();
	if (!tex_name.length())
	{
		sendMessage("Failed to export texture. Target texture has no name");
		return;
	}
	
	String base_name = FileSystem::extractFilePath(outname);
	if (base_name.length())
		base_name+=FOLDER_SLASH;
	base_name+=tex_name;
	
	
	if (TextureCompression::decompress(texture->face_field[0].pointer(),texture->face_field[0].size(),out))	//that i would ever have to use const_cast... stupid bz2 lib :S
	{
		if (out.getContentType() == PixelType::Color && out.getChannels() > 3)
		{
			alpha_names.set(texture,tex_name+".alpha.bmp");
			Image alpha;
			out.extractChannels(3,1,alpha);
			try
			{
				bitmap.saveToFile(alpha,base_name+".alpha.bmp");
				sendMessage("  Alpha component of '"+tex_name+"' written to '"+base_name+".alpha.bmp'");
				fout << "map_d " <<  tex_name<<".alpha.bmp"<<nl;
			}
			catch (...)
			{
				sendMessage("  Failed to write alpha of '"+tex_name+"' to '"+base_name+".alpha.bmp'");
			}
		}
		if (out.getChannels() != 3)
			out.setChannels(3);
		
		if (out.getContentType() != PixelType::Color)
		{
			TVec3<> normal;
			for (UINT32 x = 0; x < out.getWidth(); x++)
				for (UINT32 y = 0; y < out.getHeight(); y++)
				{
					out.getNormal(x,y,normal);
					swp(normal.y,normal.z);
					normal.y*=-1;
					out.setNormal(x,y,normal);
				}
		}
		
		try
		{
			bitmap.saveToFile(out,base_name+".bmp");
			texture_names.set(texture,tex_name+".bmp");
			if (out.getContentType() == PixelType::Color)
			{
				sendMessage("  Color component of '"+tex_name+"' written to '"+base_name+".bmp'");
				fout << "map_Kd "<<tex_name<<".bmp"<<nl;
			}
			else
			{
				sendMessage("  Normal map '"+tex_name+"' written to '"+base_name+".bmp'");
				fout << "bump "<<tex_name<<".bmp"<<nl;
			}
		}
		catch (const std::exception&except)
		{
			sendMessage("  Failed to write texture '"+tex_name+"' to '"+base_name+".bmp' ("+except.what()+")");
		}
		catch (...)
		{
			sendMessage("  Failed to write texture '"+tex_name+"' to '"+base_name+".bmp'");
		}
	}
	else
		sendMessage("  Failed to extract texture '"+tex_name+"'");
	
	for (index_t j = 1; j < texture->face_field.length(); j++)
		if (TextureCompression::decompress(texture->face_field[j].pointer(),texture->face_field[j].size(),out))
		{
			if (out.getContentType() == PixelType::Color && out.getChannels() > 3)
			{
				Image alpha;
				out.extractChannels(3,1,alpha);
				try
				{
					bitmap.saveToFile(alpha,base_name+String(j)+".alpha.bmp");
					sendMessage("  Alpha component of layer "+String(j)+" '"+tex_name+"' written to '"+base_name+String(j)+".alpha.bmp'");
				}
				catch (...)
				{
					sendMessage("  Failed to write alpha layer "+String(j)+" '"+tex_name+"' to '"+base_name+String(j)+".alpha.bmp'");
				}
			}
			if (out.getChannels() != 3)
				out.setChannels(3);
			if (out.getContentType() != PixelType::Color)
			{
				TVec3<> normal;
				for (UINT32 x = 0; x < out.getWidth(); x++)
					for (UINT32 y = 0; y < out.getHeight(); y++)
					{
						out.getNormal(x,y,normal);
						swp(normal.y,normal.z);
						normal.y*=-1;
						out.setNormal(x,y,normal);
					}
			}
			
			try
			{
				bitmap.saveToFile(out,base_name+String(j)+".bmp");
				sendMessage("  Color component of layer "+String(j)+" '"+tex_name+"' written to '"+base_name+String(j)+".bmp'");
			}
			catch (...)
			{
				sendMessage("  Failed to write color layer "+String(j)+" '"+tex_name+"' to '"+base_name+String(j)+".bmp'");
			}
		}
		else
			sendMessage("  Failed to extract face "+String(j)+" of texture '"+tex_name+"'");
	}


	
	void	ObjConverter::writeObject(const CGS::Geometry<>&geometry, const Array<PointerContainer<Array<TFace> > >&conversion_table,const CGS::SubGeometryA<>&object,StringFile&out)
	{
		/*console->print("  "+name2str(object.name).Trim()+"...");
		console->update();*/
		out << "g "<<object.name.trim()<<nl;
		
		Array<TFace>*face_field;
		for (UINT32 i = 0; i < conversion_table.length(); i++)
			if (conversion_table[i].query(&object,face_field))
			{
				out << "usemtl "<<geometry.material_field[i].name<<nl;
				TFace*field = face_field->pointer();
				for (UINT32 j = 0; j < face_field->length(); j++)
				{
					out << "f";
					for (BYTE k = 0; k < 4; k++)
						if (field[j].v.v[k])
							out << " "<<field[j].v.v[k]->index <<"/"<<(field[j].t.v[k]?String(field[j].t.v[k]->index):"0")<< "/"<<(field[j].n.v[k]?String(field[j].n.v[k]->index):"");
					out << nl;
				}
			}
		for (UINT32 i = 0; i < object.child_field.length(); i++)
			writeObject(geometry,conversion_table,object.child_field[i],out);
	}
	
	
	
	bool	ObjConverter::write(const CGS::Geometry<>&geometry, const String&parameter)
	{
#if 1
		String filename = parameter;
		if (!filename.endsWith(".obj"))
			filename+=".obj";
			
		
		String material_lib_file = FileSystem::extractFilePathName(filename)+".mtl";
		
		StringFile	out;
		
		texture_names.clear();
		alpha_names.clear();
		
	
		if (!out.create(material_lib_file))
		{
			last_error = "Unable to create out file '"+material_lib_file+"'";
			return false;
		}
		
		
		out << "# Material library generated for "<<filename<<nl;
		out << "#"<<nl;
		
		sendMessage(" Exporting materials...");
		
		for (unsigned i = 0; i < geometry.material_field.length(); i++)
		{
			const CGS::MaterialA<>&m = geometry.material_field[i];

			
			out << "newmtl "<<m.name<<nl;
			out << "Ka "<<m.info.ambient.red-0.1<<" "<<m.info.ambient.green-0.1<<" "<<m.info.ambient.blue-0.1<<nl;
			out << "Kd "<<m.info.diffuse.red<<" "<<m.info.diffuse.green<<" "<<m.info.diffuse.blue<<nl;
			out << "Ks "<<m.info.specular.red<<" "<<m.info.specular.green<<" "<<m.info.specular.blue<<nl;
			out << "Ns "<<m.info.shininess*10.0<<nl;	//x10?
			
			String texture_name, alpha_name;
			if (m.info.layer_field.length())
				exportTexture(m.info.layer_field[0].source,filename, out);
			
			out << nl;
		}
		
		out.close();
		
		// Log	log("export.log",true);
		
		sendMessage(" Mapping vertices");
		
		Array<PointerContainer<Array<TFace> > >	conversion_table(geometry.material_field.length());
		
		List::BinaryTree<IndexedPoolVertex,OperatorSort>	vertex_field, normal_field, texcoord_field;
		
		for (unsigned i = 0; i < geometry.material_field.length(); i++)
		{
			float step_size = 1.0f/(float)geometry.material_field.length();
			setProgress((float)i*step_size);

			const CGS::MaterialA<>&m = geometry.material_field[i];
			//console->print("  "+m.name);
			//console->update();

			
			for (index_t j = 0; j < m.data.object_field.length(); j++)
			{
				setProgress(((float)i + (float)j/(float)m.data.object_field.length())*step_size);
				const CGS::RenderObjectA<>&robj = m.data.object_field[j];
				if (!robj.target)
				{
					sendMessage("Warning: Render object target '"+robj.tname.trim()+"' not linked");
					sendMessage(" Mapping vertices");
					continue;
				}
				//console->print("   <"+name2str(robj.target->name).Trim()+">");
				//console->update();
				// log << j << nl;
			
				if (!robj.ipool.idata.length())
					continue;

				Array<PoolVertex*>	vlink_map(robj.vpool.vcnt),
										nlink_map(robj.vpool.vcnt),
										tlink_map(robj.vpool.vlyr>0?robj.vpool.vcnt:0);
				unsigned vsize = robj.vpool.vsize();
				const Def::FloatType*p = robj.vpool.vdata.pointer();
				TVec3<Def::FloatType>	transformed;
				for (unsigned k = 0; k < robj.vpool.vcnt; k++)
				{
					Mat::transform(robj.target->path,Vec::ref3(p),transformed);
					vlink_map[k] = vertex_field.add(transformed);
					Mat::rotate(robj.target->path,Vec::ref3(p+3),transformed);
					nlink_map[k] = normal_field.add(transformed);
					if (robj.vpool.vlyr>0)
						tlink_map[k] = texcoord_field.add(IndexedPoolVertex(p[6],p[7],(Def::FloatType)0));
					p += vsize;
				}
				
				// log << "inserted"<<nl;

				const CGS::IndexContainerA<>&chunk = robj.ipool;
				Array<TFace>*field_pntr = conversion_table[i].define(robj.target),
							&field = *field_pntr;
				field.setSize(chunk.countFaces());
				UINT32	index_offset(0),
						entry_index(0);
				
				for (UINT32 l = 0; l < chunk.triangles; l++)
				{
					Vec::def(field[entry_index].v,	vlink_map[chunk.idata[index_offset]],vlink_map[chunk.idata[index_offset+1]],vlink_map[chunk.idata[index_offset+2]],(TFace*)NULL);
					Vec::def(field[entry_index].n,	nlink_map[chunk.idata[index_offset]],nlink_map[chunk.idata[index_offset+1]],nlink_map[chunk.idata[index_offset+2]],(TFace*)NULL);
					if (robj.vpool.vlyr>0)
						Vec::def(field[entry_index].t,	tlink_map[chunk.idata[index_offset]],tlink_map[chunk.idata[index_offset+1]],tlink_map[chunk.idata[index_offset+2]],(TFace*)NULL);
					else
						Vec::clear(field[entry_index].t);
					entry_index++;
					index_offset+=3;
				}
				// log << "triangles mapped"<<nl;
			

				
				// log << "mapping quads"<<nl;
				for (UINT32 l = 0; l < chunk.quads; l++)
				{
					Vec::def(field[entry_index].v,	vlink_map[chunk.idata[index_offset]],vlink_map[chunk.idata[index_offset+1]],vlink_map[chunk.idata[index_offset+2]],vlink_map[chunk.idata[index_offset+3]]);
					Vec::def(field[entry_index].n,	nlink_map[chunk.idata[index_offset]],nlink_map[chunk.idata[index_offset+1]],nlink_map[chunk.idata[index_offset+2]],nlink_map[chunk.idata[index_offset+3]]);
					if (robj.vpool.vlyr>0)
						Vec::def(field[entry_index].t,	tlink_map[chunk.idata[index_offset]],tlink_map[chunk.idata[index_offset+1]],tlink_map[chunk.idata[index_offset+2]],tlink_map[chunk.idata[index_offset+3]]);
					else
						Vec::clear(field[entry_index].t);
					entry_index++;
					index_offset+=4;
				}
				// log << "quads mapped"<<nl;
			
				
					
				
				
				if (entry_index != field.length())
				{
					// log << "exception encountered"<<nl;
					last_error = " Critical exception encountered";
					last_error += "\n  Outfield should be at offset "+String(field.length())+" but is at "+String(entry_index);
					last_error += "\n  If the latter is greater then you're lucky to even see this";
					return false;
				}
			}
		}
		
		current_callback->hideConverterProgressBar();
		sendMessage(" Mapped "+String(vertex_field.count())+" vertices, "+String(normal_field.count())+" normals, and "+String(texcoord_field.count())+" texcoords");
		sendMessage(" Exporting vertices");


		if (!out.create(filename))
		{
			last_error = "Unable to create out file '"+filename+"'";
			return false;
		}
		out << "# Object generated for "<<filename<<nl;
		out << "#"<<nl;
		out << "mtllib ./"<<FileSystem::extractFileName(filename)<<".mtl"<<nl;
		
		out << "g"<<nl;
		index_t	index=1;
		count_t	progress=0,
				total = vertex_field.count()+normal_field.count()+texcoord_field.count(),
				update_step = total/100;
		if (!update_step)
			update_step = 1;

		vertex_field.reset();
		while (IndexedPoolVertex*vtx = vertex_field.each())
		{
			if (!(progress++%update_step))
				setProgress((float)progress/(float)total);
			out << "v "<<vtx->p.x<<" "<<vtx->p.y<<" "<<vtx->p.z<<nl;
			vtx->index = index++;
		}
		index = 1;
		normal_field.reset();
		while (IndexedPoolVertex*vtx = normal_field.each())
		{
			if (!(progress++%update_step))
				setProgress((float)progress/(float)total);
			out << "vn "<<vtx->p.x<<" "<<vtx->p.y<<" "<<vtx->p.z<<nl;
			vtx->index = index++;
		}
		index = 1;
		texcoord_field.reset();
		while (IndexedPoolVertex*vtx = texcoord_field.each())
		{
			if (!(progress++%update_step))
				setProgress((float)progress/(float)total);
			out << "vt "<<vtx->p.x<<" "<<vtx->p.y<<" "<<vtx->p.z<<nl;
			vtx->index = index++;
		}
		out << "# Mapped "<<vertex_field.count()<<" vertices, "<<normal_field.count()<<" normals, and "<<texcoord_field.count()<<" texcoords"<<nl;
		out << "#"<<nl;
		
		sendMessage(" Exporting faces");
		for (index_t i = 0; i < geometry.object_field.length(); i++)
		{
			setProgress((float)i/(float)geometry.object_field.length());
			writeObject(geometry,conversion_table,geometry.object_field[i],out);
		}
		setProgress(1.0f);
		out.close();
		
		vertex_field.clear();
		normal_field.clear();
		texcoord_field.clear();
		conversion_table.free();
		return true;
#endif
		//return false;
	}


	ObjConverter 	obj_converter;
}
