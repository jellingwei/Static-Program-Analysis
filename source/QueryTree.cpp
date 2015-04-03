#include "QueryTree.h"

QueryTree::QueryTree()
{
	Synonym empty;
	_root = QNode(ROOT,empty, empty, empty, empty);
	_result = QNode(RESULT,empty, empty, empty, empty);
	_clauses = QNode(CLAUSES, empty, empty, empty, empty);

	linkNode(&_root, &_result);
	linkNode(&_root, &_clauses);
	
	unordered_map<string, SYNONYM_TYPE> _synonymsMap; 
	_synonymsMap.clear();
}

/**
 * Creates a QNode with the given NODE_TYPE and SYNONYM arguments.
 * @param QNODE_TYPE
 * @param arg0
 * @param arg1
 * @param arg2
 * @param arg3
 * @return a QNode with its corresponding NODE_TYPE and SYNONYM arguments.
 */
QNode* QueryTree::createQNode(QNODE_TYPE QNODE_TYPE,Synonym arg0, Synonym arg1, Synonym arg2, Synonym arg3)
{
	QNode* node = new QNode(QNODE_TYPE, arg0, arg1, arg2, arg3);
	return node;
}

/**
 * Creates a link between the parent QNode and the child QNode.
 * @param parent_node
 * @param child_node
 * @return TRUE to indicate the link between the parent QNode and the child QNode is created successfully.
 */
STATUS QueryTree::linkNode(QNode* parent_node, QNode* child_node)
{
	child_node->setParent(parent_node);
	parent_node->setChild(child_node);
	return true;
}

void QueryTree::setResultNode(QNode* resultNode)
{
	_result = *resultNode;
}

void QueryTree::setClausesNode(QNode* clausesNode)
{
	_clauses = *clausesNode;
}

/**
 * @return the root of the query tree.
 */
QNode* QueryTree::getRoot()
{
	return &_root;
}

/**
 * @return the head of the result nodes.
 */
QNode* QueryTree::getResultNode()
{
	return &_result;
}

/**
 * @return the head of the clauses nodes.
 */
QNode* QueryTree::getClausesNode()
{
	return &_clauses;
}

/**
 * It stores all the design entities and synonyms in string. The key to the
 * synonym map is the synonym.
 * @return a synonym map if the query is successfully parsed and there is no syntax error.
 * Otherwise, it returns an empty synonym map .
 */
unordered_map<string, SYNONYM_TYPE> QueryTree::getSynonymsMap()
{
	return _synonymsMap;
}

/**
 * Set the synonym map in the query tree. This overwrites the current
 * synonym map in the query tree.
 * @param synonymsMap
 */
void QueryTree::setSynonymsMap(unordered_map<string, SYNONYM_TYPE> synonymsMap)
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
	cout << "Clauses: " << endl;
	for(int i=0; i<_clauses.getNumberOfChildren(); i++) {
		printNode(_clauses.getChild(i));
	}
	cout << "#################### Print Tree ####################" << endl << endl;
}

void QueryTree::printNode(QNode* node)
{
	string type, arg0_type, arg1_type, arg2_type, arg3_type;
	string arg0_name, arg1_name, arg2_name, arg3_name;

	switch(node->getNodeType()) {
	case ModifiesP:
	case ModifiesS:          
		type = "Modifies";
		break;
	case Follows:
		type = "Follows";
		break;
	case FollowsT:
		type = "Follows*";
		break;
	case Parent:
		type = "Parent";
		break;
	case ParentT:
		type = "Parent*";
		break;
	case UsesP:
	case UsesS:
		type = "Uses";
		break;
	case Calls:
		type = "Calls";
		break;
	case CallsT:
		type = "Calls*";
		break;
	case Next:
		type = "Next";
		break;
	case NextT:
		type = "Next*";
		break;
	case Affects:
		type = "Affects";
		break;
	case AffectsT:
		type = "Affects*";
		break;
	case Contains:
		type = "Contains";
		break;
	case ContainsT:
		type = "Contains*";
		break;
	case Sibling:
		type = "Sibling";
		break;
	case NextBip:
		type = "NextBip";
		break;
	case NextBipT:
		type = "NextBip*";
		break;
	case AffectsBip:
		type = "AffectsBip";
		break;
	case AffectsBipT:
		type = "AffectsBip*";
		break;
	case Selection:
		type = "Selection";
		break;
	case Pattern:
		type = "Pattern";
		break;
	case With:
		type = "With";
		break;
	default:
		type = "unknown";
	}
	arg0_type = Synonym::convertToString(node->getArg0().getType());
	arg0_name = node->getArg0().getName();
	arg1_type = Synonym::convertToString(node->getArg1().getType());
	arg1_name = node->getArg1().getName();
	arg2_type = Synonym::convertToString(node->getArg2().getType());
	arg2_name = node->getArg2().getName();
	arg3_type = Synonym::convertToString(node->getArg3().getType());
	arg3_name = node->getArg3().getName();

	cout << "(TYPE:" << type << ", " << 
			"Arg0:" << arg0_type << "," << arg0_name << ", " <<
			"Arg1:" << arg1_type << "," << arg1_name << ", " <<
			"Arg2:" << arg2_type << "," << arg2_name << ", " <<
			"Arg3:" << arg3_type << "," << arg3_name << ")"
			<< endl;
}
