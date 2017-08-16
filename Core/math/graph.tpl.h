#ifndef graphTplH
#define graphTplH

/******************************************************************

Collection of graph-algorithms.


******************************************************************/


template <class Entry> CGNode<Entry>::CGNode(unsigned index_):selected(false),my_indeg(0),dfs_in(0),dfs_out(0),index(index_)
{}

template <class Entry> unsigned CGNode<Entry>::scan(unsigned in)
{
    selected = true;
    dfs_in = in;
    out.reset();
    while (CGNode<Entry>*node = out.each())
        if (!node->selected)
            in = node->scan(in+1);
    dfs_out = in+1;
    return dfs_out;
}


template <class Entry> CGEdge<Entry>::CGEdge(CGNode<Entry>*from_, CGNode<Entry>*to_):from(from_),to(to_)
{
    to->in.append(from);
    from->out.append(to);
    to->edge_in.append(this);
    from->edge_out.append(this);
}

template <class Entry> CGEdge<Entry>::~CGEdge()
{
    from->out.drop(to);
    to->in.drop(from);
    to->edge_in.drop(this);
    from->edge_out.drop(this);
}



template <class Entry> bool Graph<Entry>::createTopologicalOrder(ReferenceVector<CGNode<Entry> >&out)
{
    DynamicQueue<CGNode<Entry>*> queue;
    nodes.reset();
    while (CGNode<Entry>*node = nodes.each())
    {
        node->my_indeg = node->in.Count();
        if (!node->my_indeg)
            queue.Insert(node);
    }
    
    out.flush();

    while (!queue.empty())
    {
        CGNode<Entry>*node = queue.get();
        if (node->my_indeg)
            continue;
        node->my_indeg--;
        out.append(node);
        node->out.reset();
        while (CGNode<Entry>*tnode = node->out.each())
            if (!--tnode->my_indeg)
                queue.Insert(tnode);
    }
    return out.Count() == nodes;
}

template <class Entry> void Graph<Entry>::performDepthFirstSearch()
{
    unsigned cnt(0);
    nodes.reset();
    while (CGNode<Entry>*node = nodes.each())
        node->selected = false;
    nodes.reset();
    while (CGNode<Entry>*node = nodes.each())
        if (!node->in)
            cnt = node->scan(cnt+1);
}

template <class Entry> void            Graph<Entry>::addNodes(unsigned count)
{
    for (unsigned i = 0; i < count; i++)
        nodes.append(new CGNode<Entry>(nodes));
}

template <class Entry> CGEdge<Entry>*  Graph<Entry>::addEdge(unsigned index0, unsigned index1)
{
    if (index0 >= nodes || index1 >= nodes)
        return NULL;
    return edges.append(new CGEdge<Entry>(nodes[index0],nodes[index1]));
}

template <class Entry> CGEdge<Entry>*  Graph<Entry>::addEdge(CGNode<Entry>*node0, CGNode<Entry>*node1)
{
    return edges.append(new CGEdge<Entry>(node0,node1));
}



#endif
