#include "../global_root.h"
#include "graph.h"

namespace DeltaWorks
{

	namespace Math
	{
		namespace GraphDef
		{
			Node::Node(index_t index):isSelected(false),inDegree(0),dfsIn(0),dfsOut(0),index(index)
			{}

			index_t	Node::Scan(index_t in)
			{
				isSelected = true;
				dfsIn = in;
				foreach (out,node)
				{
					PNode n = node->lock();
					if (!n->isSelected)
						in = n->Scan(in+1);
				}
				dfsOut = in+1;
				return dfsOut;
			}


			void Edge::LinkTo(const PNode&from_, const PNode&to_)
			{
				ASSERT__(!from);
				ASSERT__(!to);
				to = to_;
				from = from_;
				to->in.append(from);
				from->out.append(to);
				to->inEdges.append(shared_from_this());
				from->outEdges.append(shared_from_this());
			}

			template<typename T>
			static	void _EraseIn(Container::BasicBuffer<std::weak_ptr<T> >&container, const std::shared_ptr<T>&element)
			{
				for (index_t i = 0; i < container.count(); i++)
					if (container[i].lock() == element)
					{
						container.Erase(i);
						return;
					}
			}

			/*virtual*/ Edge::~Edge()
			{
				//_EraseIn(from->out,to);
				//_EraseIn(to->in,from);
				//_EraseIn(to->inEdges,shared_from_this());
				//_EraseIn(from->outEdges,shared_from_this());
			}
		}

		void				Graph::AddNodes(count_t count)
		{
			index_t offset = nodes.count();
			PNode*stride = nodes.appendRow(count);
			for (index_t i = 0; i < count; i++)
				stride[i].reset(new GraphDef::Node(offset+i));
		}
		GraphDef::PEdge		Graph::AddEdge(index_t index0, index_t index1)
		{
			return AddEdge(nodes[index0],nodes[index1]);
		}
		GraphDef::PEdge		Graph::AddEdge(const PNode&node0, const PNode&node1)
		{
			ASSERT__(node0 != node1);
			ASSERT__(!!node0);
			ASSERT__(!!node1);
			for (index_t i = 0; i < node0->out.count(); i++)
			{
				PNode n = node0->out[i].lock();
				if (n == node1)
					return node0->outEdges[i].lock();
			}
			for (index_t i = 0; i < node0->in.count(); i++)
			{
				PNode n = node0->in[i].lock();
				if (n == node1)
					return node0->inEdges[i].lock();
			}

			PEdge edge(new GraphDef::Edge());
			edge->LinkTo(node0,node1);
			edges.append(edge);
			return edge;
		}
	
		bool				Graph::CreateTopologicalOrder(Container::BasicBuffer<PNode>&out)
		{
			Container::Queue<PNode>	queue;
			foreach (nodes,n)
			{
				(*n)->inDegree = (int)(*n)->in.count();
				if (!(*n)->inDegree)
					queue.Push(*n);
			}
    
			out.clear();
			while (queue.IsNotEmpty())
			{
				PNode node = queue.Pop();
				if (node->inDegree > 0)
					continue;
				//node->inDegree--;
				out.append(node);
				foreach (node->out,out)
				{
					PNode n = out->lock();
					if (!--n->inDegree)
						queue.Push(n);
				}
			}
			return out.count() == nodes.count();
		}
		void				Graph::PerformDepthFirstSearch()
		{
			count_t cnt(0);
			foreach (nodes,n)
				(*n)->isSelected = false;
			foreach (nodes,n)
				if ((*n)->in.IsEmpty())
					cnt = (*n)->Scan(cnt+1);
		};

	}

}