#include "QNode.h"

QNode::QNode()
{
}

QNode::QNode(QNODE_TYPE QNODE_TYPE,Synonym arg0,Synonym arg1, Synonym arg2)
{
	_QNODE_TYPE = QNODE_TYPE;
	_arg0 = arg0;
	_arg1 = arg1;
	_arg2 = arg2;
	_current_child_index = -1;

}

Synonym QNode::getArg0() 
{
	return _arg0;
}

Synonym QNode::getArg1() 
{
	return _arg1;
}

Synonym QNode::getArg2() 
{
	return _arg2;
}

QNODE_TYPE QNode::getNodeType()
{
	return _QNODE_TYPE;
}

void QNode::setParent(QNode* parent_node)
{
	_parent = parent_node;
}

void QNode::setChild(QNode* child_node)
{
	_child_list.push_back(child_node);
}

int QNode::getNumberOfChildren()
{
	return _child_list.size();
}

QNode* QNode::getParent() 
{
	return _parent;
}

QNode* QNode::getChild()
{
	if(getNumberOfChildren() == 0) {
		return NULL;
	}
	_current_child_index = 0;
	return _child_list.front();
}

QNode* QNode::getChild(int index)
{
	if(index >= getNumberOfChildren() || index < 0){
		return NULL;
	}
	_current_child_index = index;
	return _child_list.at(index);
}

QNode* QNode::getNextChild()
{
	if(_current_child_index == -1 || 
		_current_child_index + 1 >= getNumberOfChildren()){
		return NULL;
	}
	return _child_list.at(++_current_child_index);
}

QNode* QNode::getPreviousChild()
{
	if(_current_child_index == -1 || 
		_current_child_index - 1 < 0){
		return NULL;
	}
	return _child_list.at(--_current_child_index);
}