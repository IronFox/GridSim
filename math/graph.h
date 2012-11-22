#ifndef graphH
#define graphH


/******************************************************************

Collection of graph-algorithms.

This file is part of Delta-Works
Copyright (C) 2006-2008 Stefan Elsen, University of Trier, Germany.
http://www.delta-works.org/forge/
http://informatik.uni-trier.de/

******************************************************************/


#include <list/queue.h>


template <class>    class Graph;
template <class>    class CGNode;
template <class>    class CGEdge;

struct TGPlain
{};


template <class Entry=TGPlain>  class CGNode:public Entry
{
private:
        bool            selected;
        int             my_indeg;

        friend class Graph<Entry>;
        friend class CGEdge<Entry>;
public:
        ReferenceVector<CGNode<Entry> >    in,out;
        ReferenceVector<CGEdge<Entry> >    edge_in,edge_out;
        unsigned        dfs_in,dfs_out,index;

                        CGNode(unsigned index);
        unsigned        scan(unsigned);
};


template <class Entry=TGPlain> class CGEdge
{
private:
        friend class Graph<Entry>;

public:
        CGNode<Entry>   *from,*to;
        
                        CGEdge(CGNode<Entry>*from, CGNode<Entry>*to);
virtual                ~CGEdge();
};



template <class Entry=TGPlain>  class Graph
{
public:
        Vector<CGNode<Entry> >   nodes;
        Vector<CGEdge<Entry> >   edges;
        
        void            addNodes(unsigned count);
        CGEdge<Entry>*  addEdge(unsigned index0, unsigned index1);
        CGEdge<Entry>*  addEdge(CGNode<Entry>*node0, CGNode<Entry>*node1);
        bool            createTopologicalOrder(ReferenceVector<CGNode<Entry> >&out);
        void            performDepthFirstSearch();
};




#include "graph.tpl.h"


#endif
