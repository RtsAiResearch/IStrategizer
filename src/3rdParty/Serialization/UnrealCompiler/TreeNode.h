#ifndef TREENODE_H
#define TREENODE_H

#include <string>
#include <vector>
#include <algorithm>
#include "Toolbox.h"
using namespace std;

template<class TKey, class TValue>
class TreeNode
{
protected:
    TKey    m_key;
    TValue* m_value;
    
    vector< TreeNode<TKey, TValue>* > m_children;
public:
    TreeNode(TKey p_key) : m_key(p_key), m_value(NULL) {}
    TreeNode(TKey p_key, TValue* p_value) : m_key(p_key), m_value(p_value) {}

    const vector< TreeNode<TKey, TValue>* >* Children() const { return &m_children; }
    vector< TreeNode<TKey, TValue>* >* Children() { return &m_children; }

    TKey Key() const { return m_key; }
    void Key(TKey val) { m_key = val; }

    TValue* Value() const { return m_value; }
    void Value(TValue* val) { m_value = val; }

    TreeNode<TKey, TValue>* AddChild(TreeNode<TKey, TValue>* p_node)
    {
        m_children.push_back(p_node);
        return p_node;
    }

    void ReverseChildren()
    {
        reverse(m_children.begin(), m_children.end());
    }

    void RemoveChild(TreeNode<TKey, TValue>* p_node)
    {
        vector< TreeNode<TKey, TValue>* >::iterator where;
        where = find(m_children.begin(), m_children.end(), p_node);

        if(where != m_children.end())
        {
            m_children.erase(where);
        }
    }

    ~TreeNode()
    {
        Toolbox::MemoryClean(m_children);
        Toolbox::MemoryClean(m_value);
    }
};

#endif // TREENODE_H
