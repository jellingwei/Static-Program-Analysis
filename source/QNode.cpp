#include "QNode.h"

QNode::QNode()
{
}

/**
 * Initialises and creates a QNode for Query Tree with the given QNODETYPE and SYNONYM arguments. 
 * NODETYPE differentiates the type of QNode.
 */
QNode::QNode(QNODE_TYPE QNODE_TYPE,Synonym arg0,Synonym arg1, Synonym arg2)
{
	_QNODE_TYPE = QNODE_TYPE;
	_arg0 = arg0;
	_arg1 = arg1;
	_arg2 = arg2;
	_current_child_index = -1;

}

/**
 * Return SYNONYM argument 0.
 */
Synonym QNode::getArg0() 
{
	return _arg0;
}

/**
 * Return SYNONYM argument 1.
 */
Synonym QNode::getArg1() 
{
	return _arg1;
}

/**
 * Return SYNONYM argument 2.
 */
Synonym QNode::getArg2() 
{
	return _arg2;
}

/**
 * Return the node type of the QNode.
 */
QNODE_TYPE QNode::getNodeType()
{
	return _QNODE_TYPE;
}

/**
 * Set the QNode, parent_node, as a parent node.
 */
void QNode::setParent(QNode* parent_node)
{
	_parent = parent_node;
}

/**
 * Set the QNode, child_node, as a child node.
 */
void QNode::setChild(QNode* child_node)
{
	_child_list.push_back(child_node);
}

/**
 * Return the number of children the QNode has.
 */
int QNode::getNumberOfChildren()
{
	return _child_list.size();
}

/**
 * Return the parent QNode of the current QNode.
 */
QNode* QNode::getParent() 
{
	return _parent;
}

/**
 * Return the child QNode of the current QNode.
 * If the current QNode has no child node, return NULL.
 */
QNode* QNode::getChild()
{
	if(getNumberOfChildren() == 0) {
		return NULL;
	}
	_current_child_index = 0;
	return _child_list.front();
}

/**
 * Return the child QNode of the current QNode with the given index.
 * If the current QNode has no child node or index is negative, return NULL.
 */ 
QNode* QNode::getChild(int index)
{
	if(index >= getNumberOfChildren() || index < 0){
		return NULL;
	}
	_current_child_index = index;
	return _child_list.at(index);
}

/**
 * Return the next child's QNode.
 */ 
QNode* QNode::getNextChild()
{
	if(_current_child_index == -1 || 
		_current_child_index + 1 >= getNumberOfChildren()){
		return NULL;
	}
	return _child_list.at(++_current_child_index);
}

/**
 * Return the previous child’s QNode.
 */ 
QNode* QNode::getPreviousChild()
{
	if(_current_child_index == -1 || 
		_current_child_index - 1 < 0){
		return NULL;
	}
	return _child_list.at(--_current_child_index);
}