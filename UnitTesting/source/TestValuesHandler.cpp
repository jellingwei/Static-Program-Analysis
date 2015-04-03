#include <cppunit/config/SourcePrefix.h>
#include <string>
#include <vector>

#include "TestValuesHandler.h"
#include "ValuesHandler.h"
#include "Synonym.h"

using std::string;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ValuesHandlerTest );

void fillVector(vector<int> &vectorToFill, int start, int end);
void fillSet(set<int> &setToFill, int start, int end);
void populateMainTable();
void populateSingletonTable();

void ValuesHandlerTest::setUp() 
{
	cout << "Clear all\n";
	ValuesHandler::clearAll();
}

void ValuesHandlerTest::tearDown() 
{
}

void fillVector(vector<int> &vectorToFill, int start, int end) 
{
	vectorToFill.clear();
	for (int i = start; i <= end; i++) {
		vectorToFill.push_back(i);
	}
}

void fillSet(set<int> &setToFill, int start, int end) 
{
	setToFill.clear();
	for (int i = start; i <= end; i++) {
		setToFill.insert(i);
	}
}

void populateMainTable()
{
	vector<int> values;
	fillVector(values, 1, 10);  //Fill the vector from 1 to 10
	Synonym s(STMT, "s", values);
	fillVector(values, 1, 10);  //Fill the vector from 11 to 20
	Synonym v(VARIABLE, "v", values);
	ValuesHandler::joinWithMainTable(s, v);
}

void populateSingletonTable()
{
	vector<int> values;
	fillVector(values, 1, 10);  //Fill the vector from 1 to 10
	Synonym ifStmt(IF, "ifStmt", values);
	ValuesHandler::addToSingletonTableForTesting(ifStmt);

	fillVector(values, 1, 5);  //Fill the vector from 1 to 5
	Synonym callStmt(CALL, "callStmt", values);
	ValuesHandler::addToSingletonTableForTesting(callStmt);
}

void ValuesHandlerTest::testFindIndexInMainTable()
{
	cout << "Find index in main table\n";
	populateMainTable();
	//Search for valid synonym names in the main table
	CPPUNIT_ASSERT_EQUAL(0, ValuesHandler::findIndexInMainTable("s"));
	CPPUNIT_ASSERT_EQUAL(1, ValuesHandler::findIndexInMainTable("v"));

	//Search for invalid synonym names in the main table
	CPPUNIT_ASSERT_EQUAL(-1, ValuesHandler::findIndexInMainTable("ifStmt"));
}

void ValuesHandlerTest::testIsExistInMainTable()
{
	cout << "Is exist in main table\n";
	populateMainTable();
	//Search for valid synonym names in the main table
	CPPUNIT_ASSERT_EQUAL(true, ValuesHandler::isExistInMainTable("s"));
	CPPUNIT_ASSERT_EQUAL(true, ValuesHandler::isExistInMainTable("v"));

	//Search for invalid synonym names in the main table
	CPPUNIT_ASSERT_EQUAL(false, ValuesHandler::isExistInMainTable("ifStmt"));
}

void ValuesHandlerTest::testIsExistInSingletonTable()
{
	cout << "Is exist in singleton table\n";
	populateSingletonTable();
	//Search for valid synonym names in the singleton table
	CPPUNIT_ASSERT_EQUAL(true, ValuesHandler::isExistInSingletonTable("ifStmt"));
	CPPUNIT_ASSERT_EQUAL(true, ValuesHandler::isExistInSingletonTable("callStmt"));

	//Search for invalid synonym names in the main table
	CPPUNIT_ASSERT_EQUAL(false, ValuesHandler::isExistInSingletonTable("s"));
}

void ValuesHandlerTest::testRemoveFromSingletonTable()
{
	cout << "Remove from singleton table\n";
	populateSingletonTable();
	CPPUNIT_ASSERT_EQUAL(true, ValuesHandler::isExistInSingletonTable("ifStmt"));
	ValuesHandler::removeFromSingletonTable("ifStmt");  //Remove something valid
	CPPUNIT_ASSERT_EQUAL(false, ValuesHandler::isExistInSingletonTable("ifStmt"));

	ValuesHandler::removeFromSingletonTable("s");  //Remove something invalid
	CPPUNIT_ASSERT_EQUAL(false, ValuesHandler::isExistInSingletonTable("s"));
}

void ValuesHandlerTest::testGetIntermediateValuesSetInMain()
{
	cout << "Get intermediate values set\n";
	populateMainTable();
	set<int> sValues = ValuesHandler::getIntermediateValuesSetInMain(0);
	CPPUNIT_ASSERT_EQUAL(10, (int)sValues.size());
	sValues = ValuesHandler::getIntermediateValuesSetInMain(1);
	CPPUNIT_ASSERT_EQUAL(10, (int)sValues.size());
}

void ValuesHandlerTest::testGetIntermediateValuesInMain()
{
	cout << "Get intermediate values\n";
	populateMainTable();
	vector<int> sValues = ValuesHandler::getIntermediateValuesInMain(0);
	CPPUNIT_ASSERT_EQUAL(10, (int)sValues.size());
	sValues = ValuesHandler::getIntermediateValuesInMain(1);
	CPPUNIT_ASSERT_EQUAL(10, (int)sValues.size());
}

void ValuesHandlerTest::testHashIntersectWithMainTable()
{
	cout << "Hash intersect with main\n";
	populateMainTable();
	vector<int> values;
	for (int i = 1; i <= 19; i+=2) {
		values.push_back(i);
	}
	Synonym s(STMT, "s", values);
	Synonym v(VARIABLE, "v", values);
	
	cout << "Hash intersect with one synonym\n";
	//Test intersection with one synonym
	CPPUNIT_ASSERT_EQUAL(true, ValuesHandler::hashIntersectWithMainTable(s));
	CPPUNIT_ASSERT_EQUAL(5, (int)ValuesHandler::getIntermediateValuesInMain(0).size());
	CPPUNIT_ASSERT_EQUAL(5, (int)ValuesHandler::getIntermediateValuesInMain(1).size());
	
	cout << "Hash intersect with two synonyms\n";
	//Test intersection with two synonyms
	ValuesHandler::clearAll();
	populateMainTable();
	CPPUNIT_ASSERT_EQUAL(true, ValuesHandler::hashIntersectWithMainTable(v, s));
	CPPUNIT_ASSERT_EQUAL(5, (int)ValuesHandler::getIntermediateValuesInMain(0).size());
	CPPUNIT_ASSERT_EQUAL(5, (int)ValuesHandler::getIntermediateValuesInMain(1).size());
	
	//Test intersection with one synonym that has no common values
	ValuesHandler::clearAll();
	populateMainTable();
	values.clear();
	fillVector(values, 11, 20);
	s.setValues(values);
	CPPUNIT_ASSERT_EQUAL(false, ValuesHandler::hashIntersectWithMainTable(s));
	CPPUNIT_ASSERT_EQUAL(0, (int)ValuesHandler::getIntermediateValuesInMain(0).size());
	CPPUNIT_ASSERT_EQUAL(0, (int)ValuesHandler::getIntermediateValuesInMain(1).size());
	
	//Test intersection with two synonyms that has no common values
	ValuesHandler::clearAll();
	populateMainTable();
	values.clear();
	fillVector(values, 1, 10);
	v.setValues(values);
	CPPUNIT_ASSERT_EQUAL(false, ValuesHandler::hashIntersectWithMainTable(v, s));
	CPPUNIT_ASSERT_EQUAL(0, (int)ValuesHandler::getIntermediateValuesInMain(0).size());
	CPPUNIT_ASSERT_EQUAL(0, (int)ValuesHandler::getIntermediateValuesInMain(1).size());
}

void ValuesHandlerTest::testHashJoinWithMainTable()
{
	cout << "Hash join with main\n";
	populateMainTable();
	vector<int> values;
	fillVector(values, 6, 15);
	Synonym s(STMT, "s", values);
	Synonym a(ASSIGN, "a", values);
	
	//Hash join with common values
	CPPUNIT_ASSERT_EQUAL(true, ValuesHandler::hashJoinWithMainTable(s, a));
	CPPUNIT_ASSERT_EQUAL(true, ValuesHandler::isExistInMainTable("a"));
	CPPUNIT_ASSERT_EQUAL(2, ValuesHandler::findIndexInMainTable("a"));
	CPPUNIT_ASSERT_EQUAL(5, (int)ValuesHandler::getIntermediateValuesInMain(0).size());
	CPPUNIT_ASSERT_EQUAL(5, (int)ValuesHandler::getIntermediateValuesInMain(1).size());
	CPPUNIT_ASSERT_EQUAL(5, (int)ValuesHandler::getIntermediateValuesInMain(2).size());
	
	//Hash join with no common values
	fillVector(values, 11, 15);
	s.setValues(values);
	CPPUNIT_ASSERT_EQUAL(false, ValuesHandler::hashJoinWithMainTable(s, a));
	CPPUNIT_ASSERT_EQUAL(true, ValuesHandler::isExistInMainTable("a"));
	CPPUNIT_ASSERT_EQUAL(2, ValuesHandler::findIndexInMainTable("a"));
	CPPUNIT_ASSERT_EQUAL(0, (int)ValuesHandler::getIntermediateValuesInMain(0).size());
	CPPUNIT_ASSERT_EQUAL(0, (int)ValuesHandler::getIntermediateValuesInMain(1).size());
	CPPUNIT_ASSERT_EQUAL(0, (int)ValuesHandler::getIntermediateValuesInMain(2).size());
}

void ValuesHandlerTest::testJoinWithMainTable()
{
	cout << "Join with main\n";
	populateMainTable();
	vector<int> values;
	fillVector(values, 1, 5);
	Synonym s1(STMT, "s1", values);
	fillVector(values, 6, 10);
	Synonym s2(STMT, "s2", values);
	
	CPPUNIT_ASSERT_EQUAL(true, ValuesHandler::joinWithMainTable(s1, s2));
	CPPUNIT_ASSERT_EQUAL(true, ValuesHandler::isExistInMainTable("s1"));
	CPPUNIT_ASSERT_EQUAL(2, ValuesHandler::findIndexInMainTable("s1"));
	CPPUNIT_ASSERT_EQUAL(true, ValuesHandler::isExistInMainTable("s2"));
	CPPUNIT_ASSERT_EQUAL(3, ValuesHandler::findIndexInMainTable("s2"));
	CPPUNIT_ASSERT_EQUAL(50, (int)ValuesHandler::getIntermediateValuesInMain(0).size());
	CPPUNIT_ASSERT_EQUAL(50, (int)ValuesHandler::getIntermediateValuesInMain(1).size());
	CPPUNIT_ASSERT_EQUAL(50, (int)ValuesHandler::getIntermediateValuesInMain(2).size());
	CPPUNIT_ASSERT_EQUAL(50, (int)ValuesHandler::getIntermediateValuesInMain(3).size());
}

void ValuesHandlerTest::testGetPairBySingletonIntersect()
{
	cout << "Get pair by singleton intersection\n";
	populateSingletonTable();
	vector<int> values;
	fillVector(values, 6, 15);
	Synonym ifStmt(IF, "ifStmt", values);
	Synonym s(STMT, "s", values);
	
	pair<vector<int>, vector<int>> pair = ValuesHandler::getPairBySingletonIntersect(ifStmt, s);
	CPPUNIT_ASSERT_EQUAL(5, (int)pair.first.size());
	CPPUNIT_ASSERT_EQUAL(5, (int)pair.second.size());
	CPPUNIT_ASSERT_EQUAL(false, ValuesHandler::isExistInSingletonTable("s"));
}

void ValuesHandlerTest::testHashIntersectWithSingletonTable()
{
	cout << "Hash intersect with singleton table\n";
	populateSingletonTable();
	vector<int> values;
	fillVector(values, 6, 15);
	Synonym ifStmt(IF, "ifStmt", values);
	
	//Intersect with common values
	CPPUNIT_ASSERT_EQUAL(true, ValuesHandler::hashIntersectWithSingletonTable(ifStmt));
	Synonym newIfStmt = ValuesHandler::getSynonym("ifStmt");
	CPPUNIT_ASSERT_EQUAL(5, (int)newIfStmt.getValues().size());
	
	//Intersect with no common values
	fillVector(values, 11, 15);
	ifStmt.setValues(values);
	CPPUNIT_ASSERT_EQUAL(false, ValuesHandler::hashIntersectWithSingletonTable(ifStmt));
	newIfStmt = ValuesHandler::getSynonym("ifStmt");
	CPPUNIT_ASSERT_EQUAL(0, (int)newIfStmt.getValues().size());
}

void ValuesHandlerTest::testProcessWithSingletonTable()
{
	cout << "Process with singleton table\n";
	populateSingletonTable();
	vector<int> values;
	fillVector(values, 1, 15);
	Synonym ifStmt(IF, "ifStmt", values);
	Synonym callStmt(CALL, "callStmt", values);
	CPPUNIT_ASSERT_EQUAL(true, ValuesHandler::processPairWithSingletonTable(ifStmt, callStmt));	
	CPPUNIT_ASSERT_EQUAL(true, ValuesHandler::isExistInMainTable("ifStmt"));
	CPPUNIT_ASSERT_EQUAL(true, ValuesHandler::isExistInMainTable("callStmt"));
	CPPUNIT_ASSERT_EQUAL(0, ValuesHandler::findIndexInMainTable("ifStmt"));
	CPPUNIT_ASSERT_EQUAL(1, ValuesHandler::findIndexInMainTable("callStmt"));
	CPPUNIT_ASSERT_EQUAL(false, ValuesHandler::isExistInSingletonTable("ifStmt"));
	CPPUNIT_ASSERT_EQUAL(false, ValuesHandler::isExistInSingletonTable("callStmt"));
	CPPUNIT_ASSERT_EQUAL(5, (int)ValuesHandler::getIntermediateValuesInMain(0).size());
	CPPUNIT_ASSERT_EQUAL(5, (int)ValuesHandler::getIntermediateValuesInMain(1).size());
	
	//Test case no longer valid since the structure of the ValuesHandler is changed
	/*ValuesHandler::clearAll();
	populateSingletonTable();
	Synonym s(STMT, "s", values);  //A non existent synonym in both main table and singleton table
	CPPUNIT_ASSERT_EQUAL(true, ValuesHandler::processPairWithSingletonTable(ifStmt, s));
	CPPUNIT_ASSERT_EQUAL(true, ValuesHandler::isExistInMainTable("ifStmt"));
	CPPUNIT_ASSERT_EQUAL(true, ValuesHandler::isExistInMainTable("s"));
	CPPUNIT_ASSERT_EQUAL(0, ValuesHandler::findIndexInMainTable("ifStmt"));
	CPPUNIT_ASSERT_EQUAL(1, ValuesHandler::findIndexInMainTable("s"));
	CPPUNIT_ASSERT_EQUAL(false, ValuesHandler::isExistInSingletonTable("ifStmt"));
	CPPUNIT_ASSERT_EQUAL(false, ValuesHandler::isExistInSingletonTable("s"));
	CPPUNIT_ASSERT_EQUAL(10, (int)ValuesHandler::getIntermediateValuesInMain(0).size());
	CPPUNIT_ASSERT_EQUAL(10, (int)ValuesHandler::getIntermediateValuesInMain(1).size());*/
}

void ValuesHandlerTest::testMergeSingletonToMain()
{
	cout << "Merge singleton to main\n";
	populateMainTable();
	populateSingletonTable();
	vector<int> values;
	fillVector(values, 1, 5);
	Synonym ifStmt(IF, "ifStmt", values);
	Synonym s(STMT, "s", values);
	
	CPPUNIT_ASSERT_EQUAL(true, ValuesHandler::mergeSingletonToMain(s, ifStmt));
	CPPUNIT_ASSERT_EQUAL(true, ValuesHandler::isExistInMainTable("ifStmt"));
	CPPUNIT_ASSERT_EQUAL(2, ValuesHandler::findIndexInMainTable("ifStmt"));
	CPPUNIT_ASSERT_EQUAL(false, ValuesHandler::isExistInSingletonTable("ifStmt"));
	CPPUNIT_ASSERT_EQUAL(5, (int)ValuesHandler::getIntermediateValuesInMain(0).size());
	CPPUNIT_ASSERT_EQUAL(5, (int)ValuesHandler::getIntermediateValuesInMain(1).size());
	CPPUNIT_ASSERT_EQUAL(5, (int)ValuesHandler::getIntermediateValuesInMain(2).size());
}

void ValuesHandlerTest::testIsValueExistInSet() 
{
	cout << "Is value exist in set\n";
	set<int> setValues;

	//Empty set should return false
	CPPUNIT_ASSERT_EQUAL(ValuesHandler::isValueExistInSet(setValues, 0), false);

	//Search for non existent values should return false
	fillSet(setValues, 1, 10);  //Fill from 1 to 10
	CPPUNIT_ASSERT_EQUAL(ValuesHandler::isValueExistInSet(setValues, 0), false);
	CPPUNIT_ASSERT_EQUAL(ValuesHandler::isValueExistInSet(setValues, 11), false);
	
	//Search for existing values should return true
	CPPUNIT_ASSERT_EQUAL(ValuesHandler::isValueExistInSet(setValues, 1), true);
	CPPUNIT_ASSERT_EQUAL(ValuesHandler::isValueExistInSet(setValues, 5), true);
	CPPUNIT_ASSERT_EQUAL(ValuesHandler::isValueExistInSet(setValues, 10), true);
}
