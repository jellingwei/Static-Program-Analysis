#include "QNode.h"

QNode::QNode()
{
}

/**
 * Initialises and creates a QNode for Query Tree with the given QNODETYPE and SYNONYM arguments. 
 * @param QNODE_TYPE the type of Qnode which can be the Default QueryTree Placeholder QNODE_TYPEs (ROOT/RESULT/CLAUSES) or 
 *					 the Query QNODE_TYPEs and the Select and Pattern QNODE_TYPEs.
 * @param arg0 
 * @param arg1 
 * @param arg2
 * @param arg3
 * @return the QNode for Query Tree with the its corresponding QNODETYPE and SYNONYM arguments.
 */
QNode::QNode(QNODE_TYPE QNODE_TYPE,Synonym arg0,Synonym arg1, Synonym arg2, Synonym arg3)
{
	_QNODE_TYPE = QNODE_TYPE;
	_arg0 = arg0;
	_arg1 = arg1;
	_arg2 = arg2;
	_arg3 = arg3;
	_current_child_index = -1;

}

/**
 * @return SYNONYM argument 0.
 */
Synonym QNode::getArg0() 
{
	return _arg0;
}

/**
 * #return SYNONYM argument 1.
 */
Synonym QNode::getArg1() 
{
	return _arg1;
}

/**
 * @return SYNONYM argument 2.
 */
Synonym QNode::getArg2() 
{
	return _arg2;
}

/**
 * @return SYNONYM argument 3.
 */
Synonym QNode::getArg3() 
{
	return _arg3;
}

/**
 * Set arg0
 * @param arg A Synonym object
 */
void QNode::setArg0(Synonym arg) 
{
	_arg0 = arg;
}

/**
 * Set arg1
 * @param arg A Synonym object
 */
void QNode::setArg1(Synonym arg) 
{
	_arg1 = arg;
}

/**
 * Set arg2
 * @param arg A Synonym object
 */
void QNode::setArg2(Synonym arg) 
{
	_arg2 = arg;
}

/**
 * Set arg3
 * @param arg A Synonym object
 */
void QNode::setArg3(Synonym arg) 
{
	_arg3 = arg;
}

/**
 * @return the node type of the QNode.
 */
QNODE_TYPE QNode::getNodeType()
{
	return _QNODE_TYPE;
}

/**
 * Set the QNode, parent_node, as a parent node.
 * @param parent_node a QNode which is a parent node of another QNode
 */
void QNode::setParent(QNode* parent_node)
{
	_parent = parent_node;
}

/**
 * Set the QNode, child_node, as a child node.
 * @param child_node a QNode which is a child node of another QNode
 */
void QNode::setChild(QNode* child_node)
{
	_child_list.push_back(child_node);
}

void QNode::setDirection(DIRECTION direction)
{
	_direction = direction;
}

DIRECTION QNode::getDirection()
{
	return _direction;
}

/**
 * @return the number of children the QNode has.
 */
int QNode::getNumberOfChildren()
{
	return _child_list.size();
}

/**
 * @return the parent QNode of the current QNode.
 */
QNode* QNode::getParent() 
{
	return _parent;
}

/**
 * @return the child QNode of the current QNode.
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
 * @param index the index of the current QNode 
 * @return the child QNode of the current QNode with the given index.
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
 * @return the QNode of the next child.
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
 * @return the QNode of the previous child.
 */ 
QNode* QNode::getPreviousChild()
{
	if(_current_child_index == -1 || 
		_current_child_index - 1 < 0){
		return NULL;
	}
	return _child_list.at(--_current_child_index);
}