#ifndef token_parserH
#define token_parserH
#include "tokenizer.h"
#include "../container/binarytree.h"
#include "../container/lvector.h"
#include "../container/hashtable.h"

class TokenParser;


inline void	helloFunction();

inline void	helloFunction2(int a, float b);

inline void	helloFunction3()	throw()
{

}

inline void	helloFunction4()
{


};


template <typename x>
	inline void	helloFunction5()
	{


	};

template <>
	inline void	helloFunction5<float>()
	{

	};

namespace Parser
{
	class Connection;
	class Node;


	enum operation_t
	{
		PO_NONE,
		PO_ABORT,
		PO_FINISH
	};
	enum condition_t
	{
		PC_NEVER,
		PC_LEVEL_ZERO
	};
	enum block_type_t
	{
		BT_NONE,
		BT_TOGGLE,
		BT_BLOCK,
		BT_UNBLOCK
	};

	struct Sequence
	{
		String					state;
		index_t					begin,
								end;

								Sequence():begin(InvalidIndex),end(InvalidIndex)
								{}

		Sequence&				setState(const String&new_state)
								{
									state = new_state;
									return *this;
								}
		Sequence&				setRange(index_t begin_, index_t end_)
								{
									begin = begin_;
									end = end_;
									return *this;
								}
		Sequence&				setEnd(index_t end_)
								{
									end = end_;
									return *this;
								}

		void					swap(Sequence&other)
								{
									state.swap(other.state);
									swp(begin,other.begin);
									swp(end,other.end);
								}
	};

	struct Stream : public Buffer<Sequence,0,Strategy::Swap>
	{
	public:
			typedef Buffer<Sequence,0,Strategy::Swap>	Super;

			HashTable<Buffer<index_t,0>,Strategy::Swap>	sequence_map;	//!< Maps state names to sequences


			void				swap(Stream&other)
								{
									Super::swap(other);
									sequence_map.swap(other.sequence_map);
								}
			void				adoptData(Stream&other)
								{
									Super::adoptData(other);
									sequence_map.adoptData(other.sequence_map);
								}
	};
	//typedef Buffer<Sequence,0,Strategy::Swap> 	Stream;


	class Connection
	{
	public:
			unsigned			ident;
			unsigned			block_ident;
			char				level_change;
			block_type_t		block_change;
			Node				*target;

								Connection();
			Connection*			increaseLevel();
			Connection*			decreaseLevel();
			Connection*			block(unsigned ident);
			Connection*			unblock(unsigned ident);
	};


	class Node
	{
	public:
			IndexTable<Connection>	connections;
			Connection				otherwise,
									conditioned;
			TokenParser				*super;
			operation_t				operation;
			condition_t				condition;
			String					name;

									Node();

			void					swap(Node&other)
									{
										connections.swap(other.connections);
										swp(otherwise,other.otherwise);
										swp(conditioned,other.conditioned);
										swp(super,other.super);
										swp(operation,other.operation);
										swp(condition,other.condition);
										name.swap(other.name);
									}

	};



	typedef Buffer<Stream,0,Strategy::Swap>		Result;
}


class TokenParser
{
private:
		HashContainer<Parser::Node>					nodes;
		List::Vector<Parser::Node>					detour_nodes;
		//Buffer<Parser::Connection,0>				connections;
		IndexTable<Parser::Connection>				root;
		
		Array<Parser::Node*>						node_export;


#if 0
		int test;
#endif /*0*/
		
		friend class Parser::Node;
public:
		Parser::Node*		createStatus(const String&name, Parser::operation_t op=Parser::PO_NONE);
		Parser::Connection&	define(const String&name, unsigned ident0);
		void				define(const String&name, unsigned ident0, unsigned ident1);
		void				define(const String&name, unsigned ident0, unsigned ident1, unsigned ident2);
		void				define(const String&name, unsigned ident0, unsigned ident1, unsigned ident2, unsigned ident3);
		Parser::Connection&	route(const String&name_from, const String&name_to);
		Parser::Connection&	route(const String&name_from, unsigned ident0, const String&name_to);
		void				route(const String&name_from, unsigned ident0, unsigned ident1, const String&name_to);
		void				route(const String&name_from, unsigned ident0, unsigned ident1, unsigned ident2, const String&name_to);
		void				route(const String&name_from, unsigned ident0, unsigned ident1, unsigned ident2, unsigned ident3, const String&name_to);
		Parser::Connection&	conditionedRoute(const String&name_from, const String&name_to, Parser::condition_t condition);
		void				globalBlock(unsigned ident, unsigned block_ident, Parser::block_type_t type);
		void				globalDetour(unsigned begin_symbol, unsigned end_symbol, const String&state_name);
		String				stateToStr(const String&name);

		void				parseTokenSequence(const TokenList&tokens, Parser::Result&out, index_t begin = 0, index_t end = InvalidIndex)	const;
};









#endif
