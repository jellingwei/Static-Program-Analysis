#include <cppunit/config/SourcePrefix.h>
#include "TestQueryTree.h"
#include "QueryTree.h"
#include "QNode.h"
#include "Synonym.h"
#include <vector>
#include <unordered_map>

#define DEBUG(x) do { std::cerr << x << endl; } while (0)

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( QueryTreeTest ); 

void 
QueryTreeTest::setUp()
{
}

void 
QueryTreeTest::tearDown()
{
}

void QueryTreeTest::testSynonym()
{
	// Test Synonym Constructors and Getters
	Synonym syn1(SYNONYM_TYPE(ASSIGN), "name");
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test Syn1 getType", syn1.getType(), SYNONYM_TYPE(ASSIGN));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test Syn1 getName", syn1.getName(), (string)"name");

	vector<int> int_vec; int_vec.push_back(1);
	Synonym syn2(SYNONYM_TYPE(CALL), "name", int_vec);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test Syn2 getType", syn2.getType(), SYNONYM_TYPE(CALL));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test Syn2 getName", syn2.getName(), (string)"name");
	//CPPUNIT_ASSERT_EQUAL_MESSAGE("Test Syn2 getValue", syn2.getValues().at(0), 1);

	/*vector<string> s_vec; s_vec.push_back("1");
	Synonym syn3(Synonym::convertToEnum("type"), "name", s_vec);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test Syn3 getType", syn3.getType(), SYNONYM_TYPE(UNDEFINED));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test Syn3 getName", syn3.getName(), (string)"name");
	//CPPUNIT_ASSERT_EQUAL_MESSAGE("Test Syn3 getValue",syn3.getValues().at(0), 1);*/
}

void QueryTreeTest::testQNode()
{
	// Test QNode Constructor and Setters and Getters
	Synonym syn1(Synonym::convertToEnum("type1"), "name1");
	Synonym syn2(Synonym::convertToEnum("type2"), "name2");
	Synonym syn3(Synonym::convertToEnum("type3"), "name3");
	QNode clausesNode(CLAUSES, Synonym(), syn1, syn2, syn3);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test QNode getNodeType", clausesNode.getNodeType(), CLAUSES);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test QNode getArg1", clausesNode.getArg1().getName(), (string) "name1");
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test QNode getArg2", clausesNode.getArg2().getType(), SYNONYM_TYPE(UNDEFINED));
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test QNode getArg3", clausesNode.getArg3().getName(), (string) "name3");
	
	// Test QNode Parent and Child Relations
	QNode followsQueryNode(Follows, Synonym(), syn1, syn1, Synonym());
	QNode usesQueryNode(UsesP, Synonym(), syn2, syn2, Synonym());
	clausesNode.setChild(&followsQueryNode);
	clausesNode.setChild(&usesQueryNode);
	followsQueryNode.setParent(&clausesNode);
	usesQueryNode.setParent(&clausesNode);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test followsNode getParent", followsQueryNode.getParent()->getNodeType(), CLAUSES);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test usesNode getParent", usesQueryNode.getParent()->getNodeType(), CLAUSES);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test suchThatNode getNumOfChildren", clausesNode.getNumberOfChildren(), 2);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test suchThatNode getChild (First Child)", clausesNode.getChild()->getNodeType(), Follows);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test suchThatNode getNextChild", clausesNode.getNextChild()->getNodeType(), UsesP);
	// Test suchThatNode getNextChild with no next child
	CPPUNIT_ASSERT(clausesNode.getNextChild() == NULL);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test suchThatNode getPreviousChild", clausesNode.getPreviousChild()->getNodeType(), Follows);
	// Test suchThatNode getPrevious with no previous child
	CPPUNIT_ASSERT(clausesNode.getPreviousChild() == NULL);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test suchThatNode getChild(Index)", clausesNode.getChild(1)->getNodeType(), UsesP);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test suchThatNode Indexing after getChild(Index)", clausesNode.getPreviousChild()->getNodeType(), Follows);
}

void QueryTreeTest::testQueryTree()
{  
	// Test Query Tree Constructor and default QNodes
	QueryTree* qT = new QueryTree();
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test QT Root Node", qT->getRoot()->getNodeType(), ROOT);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test QT Result Node", qT->getResultNode()->getNodeType(), RESULT);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test QT Clauses Node", qT->getClausesNode()->getNodeType(), CLAUSES);
	
	// Test Query Tree Link Node
	Synonym syn(Synonym::convertToEnum("type"), "name");
	QNode select(Selection, Synonym(), syn, syn, Synonym());
	qT->linkNode(qT->getResultNode(), &select);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test QT Link Node (child)", qT->getResultNode()->getChild()->getNodeType(), Selection);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test QT Link Node (parent)", select.getParent()->getNodeType(), RESULT);

	// Test Synonyms Map
	unordered_map<string,SYNONYM_TYPE> testMap;
	std::pair<string,SYNONYM_TYPE> p("key",PROCEDURE);
	testMap.emplace(p);
	qT->setSynonymsMap(testMap);
	CPPUNIT_ASSERT_EQUAL_MESSAGE("Test QT Synonyms Map", qT->getSynonymsMap().at("key"), PROCEDURE);
}