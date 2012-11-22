#include "../global_root.h"
#include "token_parser.h"



#ifdef LOG_PARSING
	#include <io/log.h>
#endif

namespace Parser
{
	Connection::Connection():ident(0),level_change(0),block_change(BT_NONE),target(NULL)
	{}


	Connection*  Connection::increaseLevel()
	{
		level_change = 1;
		return this;
	}

	Connection*  Connection::decreaseLevel()
	{
		level_change = -1;
		return this;
	}



	Connection*  Connection::block(unsigned ident)
	{
		block_change=BT_BLOCK;
		block_ident=ident;
		return this;
	}

	Connection*  Connection::unblock(unsigned ident)
	{
		block_change=BT_UNBLOCK;
		block_ident=ident;
		return this;
	}





	Node::Node():super(NULL),operation(PO_NONE),condition(PC_NEVER)
	{
		otherwise.target = this;
	}



}


Parser::Node*        TokenParser::createStatus(const String&name, Parser::operation_t op)
{
    Parser::Node*node = nodes.define(name);
	node->name = name;
    node->operation = op;
    return node;
}

Parser::Connection&  TokenParser::define(const String&name, unsigned ident0)
{
    Parser::Node*node = nodes.get(name,NULL);
    if (!node)
        FATAL__("addressed node '"+name+"' does not exist in the network");
    Parser::Connection&connection = root.set(ident0);
    connection.target = node;
    return connection;
}

void TokenParser::define(const String&name, unsigned ident0, unsigned ident1)
{
    Parser::Node*node = nodes.get(name,NULL);
    if (!node)
        FATAL__("addressed node '"+name+"' does not exist in the network");
    root.set(ident0).target = node;
    root.set(ident1).target = node;
}

void TokenParser::define(const String&name, unsigned ident0, unsigned ident1, unsigned ident2)
{
    Parser::Node*node = nodes.get(name,NULL);
    if (!node)
        FATAL__("addressed node '"+name+"' does not exist in the network");
    root.set(ident0).target = node;
    root.set(ident1).target = node;
    root.set(ident2).target = node;
}

void TokenParser::define(const String&name, unsigned ident0, unsigned ident1, unsigned ident2, unsigned ident3)
{
    Parser::Node*node = nodes.get(name,NULL);
    if (!node)
        FATAL__("addressed node '"+name+"' does not exist in the network");
    root.set(ident0).target = node;
    root.set(ident1).target = node;
    root.set(ident2).target = node;
    root.set(ident3).target = node;
}

/*
ParserConnection*  TokenParser::define(const String&name, ...)
{
    ParserNode*node = nodes.lookup(name);
    if (!node)
        FATAL__("addressed node '"+name+"' does not exist in the network");
    va_list ap;
    va_start(ap,name);
        while (unsigned ident = va_arg(ap,unsigned))
        {
            ParserConnection*connection = root.add(ident);
            connection->target = node;
        }
    va_end(ap);
        
}*/

Parser::Connection&  TokenParser::route(const String&name_from, const String&name_to)
{
    Parser::Node	*node0 = nodes.get(name_from,NULL),
					*node1 = nodes.get(name_to,NULL);
    if (!node0)
        FATAL__("addressed node '"+name_from+"' does not exist in the network");
    if (!node1)
        FATAL__("addressed node '"+name_to+"' does not exist in the network");

    node0->otherwise.target = node1;
    return node0->otherwise;
}


Parser::Connection&  TokenParser::conditionedRoute(const String&name_from, const String&name_to, Parser::condition_t condition)
{
    Parser::Node*node0 = nodes.get(name_from,NULL),
                *node1 = nodes.get(name_to,NULL);
    if (!node0)
        FATAL__("addressed node '"+name_from+"' does not exist in the network");
    if (!node1)
        FATAL__("addressed node '"+name_to+"' does not exist in the network");

    node0->conditioned.target = node1;
    node0->condition = condition;
    return node0->otherwise;
}


Parser::Connection&  TokenParser::route(const String&name_from, unsigned condition_ident, const String&name_to)
{
    Parser::Node*node0 = nodes.get(name_from,NULL),
                *node1 = nodes.get(name_to,NULL);
    if (!node0)
        FATAL__("addressed node '"+name_from+"' does not exist in the network");
    if (!node1)
        FATAL__("addressed node '"+name_to+"' does not exist in the network");

    Parser::Connection&connection = node0->connections.set(condition_ident);
    //if (!connection.target)
    //   connections.append(connection);
    connection.target = node1;
    return connection;
}

void TokenParser::route(const String&name_from, unsigned ident0, unsigned ident1, const String&name_to)
{
    Parser::Node*node0 = nodes.get(name_from,NULL),
                *node1 = nodes.get(name_to,NULL);
    if (!node0)
        FATAL__("addressed node '"+name_from+"' does not exist in the network");
    if (!node1)
        FATAL__("addressed node '"+name_to+"' does not exist in the network");

	{
		Parser::Connection&connection = node0->connections.set(ident0);
		//if (!connection->target)
		//    connections.append(connection);
		connection.target = node1;
	}
	{
		Parser::Connection&connection = node0->connections.set(ident1);
		//if (!connection->target)
		//	connections.append(connection);
		connection.target = node1;
	}
}

void TokenParser::route(const String&name_from, unsigned ident0, unsigned ident1, unsigned ident2, const String&name_to)
{
    Parser::Node*node0 = nodes.get(name_from,NULL),
                *node1 = nodes.get(name_to,NULL);
    if (!node0)
        FATAL__("addressed node '"+name_from+"' does not exist in the network");
    if (!node1)
        FATAL__("addressed node '"+name_to+"' does not exist in the network");

	const unsigned ident[] = {ident0, ident1, ident2};
	for (index_t i = 0; i < ARRAYSIZE(ident); i++)
	{
		Parser::Connection&connection = node0->connections.set(ident[i]);
		//if (!connection->target)
		//	connections.append(connection);
		connection.target = node1;
	}

}


void TokenParser::route(const String&name_from, unsigned ident0, unsigned ident1, unsigned ident2, unsigned ident3, const String&name_to)
{
    Parser::Node*node0 = nodes.get(name_from,NULL),
                *node1 = nodes.get(name_to,NULL);
    if (!node0)
        FATAL__("addressed node '"+name_from+"' does not exist in the network");
    if (!node1)
        FATAL__("addressed node '"+name_to+"' does not exist in the network");

	const unsigned ident[] = {ident0, ident1, ident2, ident3};

	for (index_t i = 0; i < ARRAYSIZE(ident); i++)
	{
		Parser::Connection&connection = node0->connections.set(ident[i]);
		//if (!connection->target)
		//	connections.append(connection);
		connection.target = node1;
	}
}




void               TokenParser::globalBlock(unsigned ident, unsigned block_ident, Parser::block_type_t type)
{
	if (node_export.count() != nodes)
		nodes.exportTo(node_export);

	for (index_t i = 0; i < node_export.count(); i++)
    {
		Parser::Node*node = node_export[i];
		
        Parser::Connection&connection = node->connections.set(ident);
        if (!connection.target)
        {
            connection.block_change = type;
            connection.block_ident = block_ident;
            connection.target = node;
            //connections.append(connection);
        }
    }
}

void                TokenParser::globalDetour(unsigned begin_symbol, unsigned end_symbol, const String&state_name)
{
    
	if (node_export.count() != nodes)
		nodes.exportTo(node_export);

	for (index_t i = 0; i < node_export.count(); i++)
    {
		Parser::Node*node = node_export[i];

        Parser::Node*sleep_node = detour_nodes.append();
        sleep_node->name = state_name;
        
        Parser::Connection&connection = node->connections.set(begin_symbol);
        if (!connection.target)
        {
            connection.target = sleep_node;
            //connections.append(connection);
			{
				Parser::Connection&connection2 = sleep_node->connections.set(end_symbol);
				//connections.append(connection2);
				connection2.target = node;
			}
        }
    }

}


String             TokenParser::stateToStr(const String&name)
{
	FATAL__("Temporarily unavailable");
	return "";
#if 0
    Parser::Node *node = nodes[name];
    if (!node)
        return "State '"+name+"' undefined.";
    String rs = "State '"+name+"':";
	foreach (node->connections,connection)
        rs+= "\n"+IntToStr(connection->ident)+" -> "+connection->target->name;
	return rs;
#endif /*0*/
}




void					TokenParser::parseTokenSequence(const TokenList&tokens, Parser::Result&result, index_t begin /*= 0*/, index_t end /*= InvalidIndex*/)	const
{
	result.reset();
	if (end > tokens.count())
		end = tokens.count();
	if (begin >= end)
		return;

    Parser::Stream		stream;

    index_t token_offset = begin;
    while (token_offset < end)
    {
        const TToken*token = tokens + token_offset;
        index_t minor = token_offset+1;
        const Parser::Connection*connection = root.queryPointer(token->ident);
		
        if (connection)
        {
            const Parser::Node*status = connection->target;
            bool record = true;
            int         level = 0;
            bool        blocked = false;
            unsigned    block_ident(0);

            do
            {
                if (record)
				{
					if (stream.isEmpty() || stream.last().state != connection->target->name)
					{
						stream.sequence_map.set(connection->target->name).append(stream.length());
						stream.append().setState(connection->target->name).setRange(token - tokens.pointer(),token - tokens.pointer() + 1);
					}
					else
						stream.last().setEnd(token - tokens.pointer() + 1 );
						//tokens.append(token);
				}
                record = true;
                bool abort = false;
                switch (connection->target->operation)
                {
					case Parser::PO_FINISH:
						#ifdef LOG_PARSING
							PARSE_STEP(7);
							if (write_log)
							{
								log << " -finishing"<<nl;
								String ident;
								foreach(storage,sequence)
								{
									log << "  path("<<ident<<")=";
									sequence->reset();
									while (TToken*token = sequence->each())
										if (!token->content.Pos('\n'))
											log << token->content;
									log << nl;
								}
								log << nl;
							}
							PARSE_STEP(8);
						#endif
                        result.append().adoptData(stream);
                        stream.reset();	//should be implied by adoptData()
                        token_offset = minor-1;
                        abort = true;
                    break;
					case Parser::PO_ABORT:
                        stream.reset();
						#ifdef LOG_PARSING
							if (write_log)
								log << " -aborting"<<nl;
						#endif
                        abort = true;
                    break;
					default:
					break;
                }
                if (abort)
                    break;
//                else

                if (connection->block_change != Parser::BT_NONE)
                {
                    if (!blocked)
                    {
                        if (connection->block_change != Parser::BT_UNBLOCK)
                        {
                            blocked = true;
                            block_ident = connection->block_ident;
                        }
                    }
                    else
                        if (block_ident == connection->block_ident && connection->block_change != Parser::BT_BLOCK)
                            blocked = false;
                }
                if (!blocked)
                    level += connection->level_change;

                status = connection->target;
                
                if (status->condition == Parser::PC_LEVEL_ZERO && !level)
                {
                    connection = &status->conditioned;
                    record = false;
                }
                else
                {
					token = minor < tokens.count() ? tokens + minor : NULL;
					minor++;
                    connection = token?status->connections.queryPointer(token->ident):NULL;
                    if (!connection)
                    {
						#ifdef LOG_PARSING
	                        if (write_log)
							{
								log << " -no route defined, continuing with otherwise"<<nl;
		                        log << "    route(s) defined for:"<<nl;
		                        status->connections.reset();
		                        while (ParserConnection*c = status->connections.each())
		                            log << "        "<<c->ident<<" -> "<<c->target->name<<nl;
							}
						#endif
                        
                        connection = &status->otherwise;
						
                    }
                }
				#ifdef LOG_PARSING
					PARSE_STEP(17);
				
					if (write_log)
						if (token && status)
							log << " -> '"<<token->content<<"'="<<IntToStr(token->ident)<<" ("<<status->name<<")"<<nl;
						else
							if (token)
								log << " -> '"<<token->content<<"'="<<IntToStr(token->ident)<<" (?)"<<nl;
							else
								log << " -> '?'=? ("<<status->name<<")"<<nl;
					
					PARSE_STEP(18);

				#endif
            }
            while (minor<=end);

            stream.reset();
        }
        token_offset++;
    }

	#ifdef LOG_PARSING
		if (write_log)
			log << " (done)"<<nl;
	#endif

	#ifdef LOG_PARSING
		if (write_log)
			log << " (done and done)"<<nl;
	#endif
}
