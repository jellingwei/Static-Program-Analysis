#include "QueryTree.h"

QueryTree::QueryTree()
{
	Synonym empty;
	_root = QNode(ROOT,empty, empty, empty);
	_such_that = QNode(SUCHTHAT,empty, empty, empty);
	_result = QNode(RESULT,empty, empty, empty);
	_pattern = QNode(PATTERN, empty, empty, empty);
	linkNode(&_root, &_result);
	linkNode(&_root, &_such_that);
	unordered_map<string, string> _synonymsMap; 
	_synonymsMap.clear();
}

QNode* QueryTree::createQNode(QNODE_TYPE QNODE_TYPE,Synonym arg0, Synonym arg1, Synonym arg2)
{
	QNode* node = new QNode(QNODE_TYPE, arg0, arg1, arg2);
	return node;
}

bool QueryTree::linkNode(QNode* parent_node, QNode* child_node)
{
	child_node->setParent(parent_node);
	parent_node->setChild(child_node);
	return true;
}

QNode* QueryTree::getRoot()
{
	return &_root;
}

QNode* QueryTree::getResultNode()
{
	return &_result;
}

QNode* QueryTree::getPatternNode()
{
	return &_pattern;
}

QNode* QueryTree::getSuchThatNode()
{
	return &_such_that;
}

unordered_map<string, string> QueryTree::getSynonymsMap()
{
	return _synonymsMap;
}

void QueryTree::setSynonymsMap(unordered_map<string, string> synonymsMap)
{
	_synonymsMap = synonymsMap;
}

void QueryTree::printTree()
{
	cout << endl << "#################### Print Tree ####################" << endl;
	cout << "Result: " << endl;
	for(int i=0; i<_result.getNumberOfChildren(); i++) {
		printNode(_result.getChild(i));
	}
	cout << "Such That: " << endl;
	for(int i=0; i<_such_that.getNumberOfChildren(); i++) {
		printNode(_such_that.getChild(i));
	}
	cout << "Pattern: " << endl;
	for(int i=0; i<_pattern.getNumberOfChildren(); i++) {
		printNode(_pattern.getChild(i));
	}
	cout << "#################### Print Tree ####################" << endl << endl;
}

void QueryTree::printNode(QNode* node)
{
	string type, arg0_type, arg0_name, arg1_type, arg1_name, arg2_type, arg2_name;


	switch(node->getNodeType()) {
	case Modifies:          
		type = "Modifies";
		break;
	case Follows:
		type = "Follows";
		break;
	case FollowsS:
		type = "Follows*";
		break;
	case Parent:
		type = "Parent";
		break;
	case ParentS:
		type = "Parent*";
		break;
	case Uses:
		type = "Uses";
		break;
	case UsesS:
		type = "Uses*";
		break;
	case Calls:
		type = "Calls";
		break;
	case CallsS:
		type = "Calls*";
		break;
	case Next:
		type = "Next";
		break;
	case NextS:
		type = "Next*";
		break;
	case Affects:
		type = "Affects";
		break;
	case AffectsS:
		type = "Affects*";
		break;
	case Selection:
		type = "Selection";
		break;
	case Pattern:
		type = "Pattern";
		break;
	default:
		type = "unknown";
	}
	arg0_type = node->getArg0().getType();
	arg0_name = node->getArg0().getName();
	arg1_type = node->getArg1().getType();
	arg1_name = node->getArg1().getName();
	arg2_type = node->getArg2().getType();
	arg2_name = node->getArg2().getName();

	cout << "(TYPE:" << type << ", " << 
			"Arg0:" << arg0_type << "," << arg0_name << "," <<
			"Arg1:" << arg1_type << "," << arg1_name << " ," <<
			"Arg2:" << arg2_type << "," << arg2_name << ")"
			<< endl;
}
