#ifndef graphH
#define graphH


/******************************************************************

Collection of graph-algorithms.

******************************************************************/

#include <memory>

#include "../container/queue.h"
#include "../container/buffer.h"

namespace DeltaWorks
{

	namespace Math
	{
		class Graph;

		namespace GraphDef
		{
			struct BaseAttachment
			{
				virtual				~BaseAttachment()	{}
			};
			typedef std::shared_ptr<BaseAttachment>	PBaseAttachment;

			class Node;
			class Edge;

			typedef std::weak_ptr<Node>		WNode;
			typedef std::weak_ptr<Edge>		WEdge;
			typedef std::shared_ptr<Node>	PNode;
			typedef std::shared_ptr<Edge>	PEdge;


			class Node
			{
			private:
				bool            isSelected;
				int             inDegree;
				Container::Buffer<WNode,0>	in,
								out;
				Container::Buffer<WEdge,0>	inEdges,
								outEdges;
				count_t			dfsIn,
								dfsOut;

				friend class	Edge;
				friend class	Graph;
			public:
				const index_t	index;
				PBaseAttachment	attachment;
				/**/			Node(index_t index);
				index_t			Scan(index_t dfsIn);
			};

			class Edge : public std::enable_shared_from_this<Edge>
			{
			private:
				PNode			from,
								to;
			public:
				void			LinkTo(const PNode&from, const PNode&to);
				virtual			~Edge();
			};
		}

		class Graph
		{
		public:
			typedef GraphDef::PNode	PNode;
			typedef GraphDef::PEdge	PEdge;
		private:
			Container::Buffer<PNode,0>		nodes;
			Container::Buffer<PEdge,0>		edges;
		public:
			count_t				CountNodes()	const	{return nodes.count();}
			count_t				CountEdge()		const	{return edges.count();}
			void				Clear()	{nodes.clear(); edges.clear();}
			void				AddNodes(count_t count);
			const PNode&		GetNode(index_t index)	{return nodes[index];}
			PEdge				AddEdge(index_t index0, index_t index1);
			PEdge				AddEdge(const PNode&node0, const PNode&node1);
			bool				CreateTopologicalOrder(Container::BasicBuffer<PNode>&out);
			void				PerformDepthFirstSearch();
		};

	}


}


#endif
