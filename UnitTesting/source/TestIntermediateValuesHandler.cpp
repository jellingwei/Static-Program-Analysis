#include <cppunit/config/SourcePrefix.h>
#include <string>
#include <vector>
#include <unordered_map>

#include "TestIntermediateValuesHandler.h"
#include "IntermediateValuesHandler.h"
#include "Synonym.h"

using std::string;

#define DEBUG(x) do { std::cerr << x << endl; } while (0)

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( IntermediateValuesHandlerTest );

void fillVector(vector<int> &vectorToFill, int start, int end);
void fillSet(set<int> &setToFill, int start, int end);

void IntermediateValuesHandlerTest::setUp() 
{
}

void IntermediateValuesHandlerTest::tearDown() 
{
}

void fillVector(vector<int> &vectorToFill, int start, int end) 
{
	for (int i = start; i <= end; i++) 
	{
		vectorToFill.push_back(i);
	}
}

void fillSet(set<int> &setToFill, int start, int end) 
{
	for (int i = start; i <= end; i++) 
	{
		setToFill.insert(i);
	}
}

void IntermediateValuesHandlerTest::testJoinWithExistingValues() 
{
	vector<int> values;

	fillVector(values, 1, 10);  //Fill from 1 to 10
	Synonym a("assign", "a", values);
	Synonym c("constant", "c", values);
	values.clear();
	fillVector(values, 11, 20);  //Fill from 11 to 20
	Synonym s("stmt", "s", values);
	Synonym w("while", "w", values);

	IntermediateValuesHandler::clear();

	//Add one synonym to the intermediate values table
	IntermediateValuesHandler::joinWithExistingValues(a);
	CPPUNIT_ASSERT_EQUAL(10, (int)IntermediateValuesHandler::getIntermediateValues(0).size());
	CPPUNIT_ASSERT_EQUAL(10, (int)IntermediateValuesHandler::getIntermediateValuesSet(0).size());

	//Add another synonym to the intermediate values table
	//A cartesian product should be performed with the existing values
	IntermediateValuesHandler::joinWithExistingValues(c);
	CPPUNIT_ASSERT_EQUAL(100, (int)IntermediateValuesHandler::getIntermediateValues(0).size());
	CPPUNIT_ASSERT_EQUAL(10, (int)IntermediateValuesHandler::getIntermediateValuesSet(0).size());
	CPPUNIT_ASSERT_EQUAL(100, (int)IntermediateValuesHandler::getIntermediateValues(1).size());
	CPPUNIT_ASSERT_EQUAL(10, (int)IntermediateValuesHandler::getIntermediateValuesSet(1).size());

	//Add a pair of synonyms to the intermediate values table
	//A cartesian product of the pair should be performed with the existing values
	IntermediateValuesHandler::joinWithExistingValues(s, w);
	CPPUNIT_ASSERT_EQUAL(1000, (int)IntermediateValuesHandler::getIntermediateValues(0).size());
	CPPUNIT_ASSERT_EQUAL(10, (int)IntermediateValuesHandler::getIntermediateValuesSet(0).size());
	CPPUNIT_ASSERT_EQUAL(1000, (int)IntermediateValuesHandler::getIntermediateValues(1).size());
	CPPUNIT_ASSERT_EQUAL(10, (int)IntermediateValuesHandler::getIntermediateValuesSet(1).size());
	CPPUNIT_ASSERT_EQUAL(1000, (int)IntermediateValuesHandler::getIntermediateValues(2).size());
	CPPUNIT_ASSERT_EQUAL(10, (int)IntermediateValuesHandler::getIntermediateValuesSet(2).size());
	CPPUNIT_ASSERT_EQUAL(1000, (int)IntermediateValuesHandler::getIntermediateValues(3).size());
	CPPUNIT_ASSERT_EQUAL(10, (int)IntermediateValuesHandler::getIntermediateValuesSet(3).size());

	//Clear and add a pair of synonyms to the intermediate values table
	IntermediateValuesHandler::clear();
	IntermediateValuesHandler::joinWithExistingValues(s, w);
	CPPUNIT_ASSERT_EQUAL(10, (int)IntermediateValuesHandler::getIntermediateValues(0).size());
	CPPUNIT_ASSERT_EQUAL(10, (int)IntermediateValuesHandler::getIntermediateValuesSet(0).size());
	CPPUNIT_ASSERT_EQUAL(10, (int)IntermediateValuesHandler::getIntermediateValues(1).size());
	CPPUNIT_ASSERT_EQUAL(10, (int)IntermediateValuesHandler::getIntermediateValuesSet(1).size());
}

void IntermediateValuesHandlerTest::testIntersectWithExistingValues()
{
	vector<int> values1, values2;
	fillVector(values1, 1, 10);  //Fill from 1 to 10
	fillVector(values2, 11, 20);
	Synonym s("stmt", "s", values1);

	//Intersection of one synonym with one synonym

	IntermediateValuesHandler::clear();
	IntermediateValuesHandler::joinWithExistingValues(s);  //Insert into the intermediate values table
	CPPUNIT_ASSERT_EQUAL(10, (int)IntermediateValuesHandler::getIntermediateValues(0).size());

	IntermediateValuesHandler::intersectWithExistingValues(0, values1);
	CPPUNIT_ASSERT_EQUAL(10, (int)IntermediateValuesHandler::getIntermediateValues(0).size());
	CPPUNIT_ASSERT_EQUAL(10, (int)IntermediateValuesHandler::getIntermediateValuesSet(0).size());

	values1.clear();
	fillVector(values1, 5, 10);  //Fill from 5 to 10
	IntermediateValuesHandler::intersectWithExistingValues(0, values1);
	CPPUNIT_ASSERT_EQUAL(6, (int)IntermediateValuesHandler::getIntermediateValues(0).size());
	CPPUNIT_ASSERT_EQUAL(6, (int)IntermediateValuesHandler::getIntermediateValuesSet(0).size());

	values1.clear();
	fillVector(values1, 11, 15);
	IntermediateValuesHandler::intersectWithExistingValues(0, values1);
	CPPUNIT_ASSERT_EQUAL(0, (int)IntermediateValuesHandler::getIntermediateValues(0).size());
	CPPUNIT_ASSERT_EQUAL(0, (int)IntermediateValuesHandler::getIntermediateValuesSet(0).size());

	//Intersection of two synonyms with one synonym

	IntermediateValuesHandler::clear();
	Synonym w("while", "w", values2);
	IntermediateValuesHandler::joinWithExistingValues(s, w);

	IntermediateValuesHandler::intersectWithExistingValues(1, values2);
	CPPUNIT_ASSERT_EQUAL(10, (int)IntermediateValuesHandler::getIntermediateValues(0).size());
	CPPUNIT_ASSERT_EQUAL(10, (int)IntermediateValuesHandler::getIntermediateValuesSet(0).size());
	CPPUNIT_ASSERT_EQUAL(10, (int)IntermediateValuesHandler::getIntermediateValues(1).size());
	CPPUNIT_ASSERT_EQUAL(10, (int)IntermediateValuesHandler::getIntermediateValuesSet(1).size());

	values2.clear();
	fillVector(values2, 11, 15);
	IntermediateValuesHandler::intersectWithExistingValues(1, values2);
	CPPUNIT_ASSERT_EQUAL(5, (int)IntermediateValuesHandler::getIntermediateValues(0).size());
	CPPUNIT_ASSERT_EQUAL(5, (int)IntermediateValuesHandler::getIntermediateValuesSet(0).size());
	CPPUNIT_ASSERT_EQUAL(5, (int)IntermediateValuesHandler::getIntermediateValues(1).size());
	CPPUNIT_ASSERT_EQUAL(5, (int)IntermediateValuesHandler::getIntermediateValuesSet(1).size());

	values1.clear();
	fillVector(values1, 1, 3);
	IntermediateValuesHandler::intersectWithExistingValues(0, values1);
	CPPUNIT_ASSERT_EQUAL(3, (int)IntermediateValuesHandler::getIntermediateValues(0).size());
	CPPUNIT_ASSERT_EQUAL(3, (int)IntermediateValuesHandler::getIntermediateValuesSet(0).size());
	CPPUNIT_ASSERT_EQUAL(3, (int)IntermediateValuesHandler::getIntermediateValues(1).size());
	CPPUNIT_ASSERT_EQUAL(3, (int)IntermediateValuesHandler::getIntermediateValuesSet(1).size());

	IntermediateValuesHandler::intersectWithExistingValues(1, values1);
	CPPUNIT_ASSERT_EQUAL(0, (int)IntermediateValuesHandler::getIntermediateValues(0).size());
	CPPUNIT_ASSERT_EQUAL(0, (int)IntermediateValuesHandler::getIntermediateValuesSet(0).size());
	CPPUNIT_ASSERT_EQUAL(0, (int)IntermediateValuesHandler::getIntermediateValues(1).size());
	CPPUNIT_ASSERT_EQUAL(0, (int)IntermediateValuesHandler::getIntermediateValuesSet(1).size());

	//Intersection of two synonyms with two synonyms
	values1.clear();
	fillVector(values1, 1, 10);
	values2.clear();
	fillVector(values2, 11, 20);
	IntermediateValuesHandler::clear();
	IntermediateValuesHandler::joinWithExistingValues(s, w);
	
	IntermediateValuesHandler::intersectWithExistingValues(0, values1, 1, values2);
	CPPUNIT_ASSERT_EQUAL(10, (int)IntermediateValuesHandler::getIntermediateValues(0).size());
	CPPUNIT_ASSERT_EQUAL(10, (int)IntermediateValuesHandler::getIntermediateValuesSet(0).size());
	CPPUNIT_ASSERT_EQUAL(10, (int)IntermediateValuesHandler::getIntermediateValues(1).size());
	CPPUNIT_ASSERT_EQUAL(10, (int)IntermediateValuesHandler::getIntermediateValuesSet(1).size());

	values1.clear();
	values2.clear();
	values1.push_back(1);
	values2.push_back(11);
	values1.push_back(5);
	values2.push_back(15);
	values1.push_back(9);
	values2.push_back(0);
	values1.push_back(0);
	values2.push_back(20);
	IntermediateValuesHandler::intersectWithExistingValues(0, values1, 1, values2);
	CPPUNIT_ASSERT_EQUAL(2, (int)IntermediateValuesHandler::getIntermediateValues(0).size());
	CPPUNIT_ASSERT_EQUAL(2, (int)IntermediateValuesHandler::getIntermediateValuesSet(0).size());
	CPPUNIT_ASSERT_EQUAL(2, (int)IntermediateValuesHandler::getIntermediateValues(1).size());
	CPPUNIT_ASSERT_EQUAL(2, (int)IntermediateValuesHandler::getIntermediateValuesSet(1).size());
}

void IntermediateValuesHandlerTest::testIntersectAndJoinWithExistingValues()
{
	vector<int> values;
	fillVector(values, 1, 10);  //Fill from 1 to 10
	Synonym s("stmt", "s", values);
	values.clear();
	fillVector(values, 11, 20);
	Synonym w("while", "w", values);

	IntermediateValuesHandler::clear();
	IntermediateValuesHandler::joinWithExistingValues(s);  //Insert into the intermediate values table
	CPPUNIT_ASSERT_EQUAL(10, (int)IntermediateValuesHandler::getIntermediateValues(0).size());

	IntermediateValuesHandler::intersectAndJoinWithExistingValues(0, s, w);
	CPPUNIT_ASSERT_EQUAL(10, (int)IntermediateValuesHandler::getIntermediateValues(0).size());
	CPPUNIT_ASSERT_EQUAL(10, (int)IntermediateValuesHandler::getIntermediateValues(1).size());

	IntermediateValuesHandler::clear();
	IntermediateValuesHandler::joinWithExistingValues(s);  //Insert into the intermediate values table
	values.clear();
	fillVector(values, 1, 5);
	s.setValues(values);
	values.clear();
	fillVector(values, 6, 10);
	w.setValues(values);

	IntermediateValuesHandler::intersectAndJoinWithExistingValues(0, s, w);
	CPPUNIT_ASSERT_EQUAL(5, (int)IntermediateValuesHandler::getIntermediateValues(0).size());
	CPPUNIT_ASSERT_EQUAL(5, (int)IntermediateValuesHandler::getIntermediateValues(1).size());

	IntermediateValuesHandler::intersectAndJoinWithExistingValues(1, s, w);
	CPPUNIT_ASSERT_EQUAL(0, (int)IntermediateValuesHandler::getIntermediateValues(0).size());
	CPPUNIT_ASSERT_EQUAL(0, (int)IntermediateValuesHandler::getIntermediateValues(1).size());
}

void IntermediateValuesHandlerTest::testFindIntermediateSynonymIndex()
{
	IntermediateValuesHandler::clear();
	CPPUNIT_ASSERT_EQUAL(-1, IntermediateValuesHandler::findIntermediateSynonymIndex("s"));

	vector<int> values;
	fillVector(values, 1, 10);  //Fill from 1 to 10
	Synonym s("stmt", "s", values);

	IntermediateValuesHandler::joinWithExistingValues(s);  //Insert into the intermediate values table
	CPPUNIT_ASSERT_EQUAL(0, IntermediateValuesHandler::findIntermediateSynonymIndex("s"));
	CPPUNIT_ASSERT_EQUAL(-1, IntermediateValuesHandler::findIntermediateSynonymIndex("w"));

	Synonym w("while", "w", values);
	IntermediateValuesHandler::joinWithExistingValues(w);
	CPPUNIT_ASSERT_EQUAL(0, IntermediateValuesHandler::findIntermediateSynonymIndex("s"));
	CPPUNIT_ASSERT_EQUAL(1, IntermediateValuesHandler::findIntermediateSynonymIndex("w"));
}

void IntermediateValuesHandlerTest::testIsValueExist() 
{
	set<int> setValues;

	//Empty set should return false
	CPPUNIT_ASSERT_EQUAL(IntermediateValuesHandler::isValueExist(setValues, 0), false);

	//Search for non existent values should return false
	fillSet(setValues, 1, 10);  //Fill from 1 to 10
	CPPUNIT_ASSERT_EQUAL(IntermediateValuesHandler::isValueExist(setValues, 0), false);
	CPPUNIT_ASSERT_EQUAL(IntermediateValuesHandler::isValueExist(setValues, 11), false);
	
	//Search for existing values should return true
	CPPUNIT_ASSERT_EQUAL(IntermediateValuesHandler::isValueExist(setValues, 1), true);
	CPPUNIT_ASSERT_EQUAL(IntermediateValuesHandler::isValueExist(setValues, 5), true);
	CPPUNIT_ASSERT_EQUAL(IntermediateValuesHandler::isValueExist(setValues, 10), true);
}
