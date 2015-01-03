#include <cppunit/config/SourcePrefix.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "TestQueryEvaluator.h"
#include "QueryEvaluator.h"
#include "Synonym.h"
using std::string;

#define DEBUG(x) do { std::cerr << x << endl; } while (0)

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( QueryEvaluatorTest );

void fillVector(vector<int> &vectorToFill, int multiple);
void fillSet(set<int> &setToFill, int multiple);

/**
* Most of the query evaluator functions cannot be unit tested because there are many classes that it depends on
* Those methods will be tested during integration testing
* Therefore, only functions that can perform tasks without external dependencies will be tested here
*/

void QueryEvaluatorTest::setUp() {
}

void QueryEvaluatorTest::tearDown() {
}

void fillVector(vector<int> &vectorToFill, int multiple) {
	for (int i = 1; i <= 10; i++) {
		vectorToFill.push_back(i*multiple);
	}
}

void fillSet(set<int> &setToFill, int multiple) {
	for (int i = 1; i <= 10; i++) {
		setToFill.insert(i*multiple);
	}
}

void QueryEvaluatorTest::testIntersectWithCurrentValues() {
	vector<int> vectorValues;
	set<int> setValues;

	//Empty set and empty vector intersect should have size of 0
	CPPUNIT_ASSERT_EQUAL(0, (int)QueryEvaluator::intersectWithCurrentValues(vectorValues, setValues).size());

	//Vector with values but empty set intersect should have size of 0
	fillVector(vectorValues, 1);
	CPPUNIT_ASSERT_EQUAL(0, (int)QueryEvaluator::intersectWithCurrentValues(vectorValues, setValues).size());

	//Set with values but empty vector intersect should have size of 0
	vectorValues.clear();
	fillSet(setValues, 1);  //{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}
	CPPUNIT_ASSERT_EQUAL(0, (int)QueryEvaluator::intersectWithCurrentValues(vectorValues, setValues).size());

	//Both having all common values
	fillVector(vectorValues, 1);  //{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}
	CPPUNIT_ASSERT_EQUAL(10, (int)QueryEvaluator::intersectWithCurrentValues(vectorValues, setValues).size());

	//Both having no common values
	vectorValues.clear();
	setValues.clear();
	fillVector(vectorValues, 11);  //{11, 22, 33, 44, 55, 66, 77, 88, 99, 110}
	fillSet(setValues, 1);  //{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}
	CPPUNIT_ASSERT_EQUAL(0, (int)QueryEvaluator::intersectWithCurrentValues(vectorValues, setValues).size());

	//Both having some common values
	vectorValues.clear();
	setValues.clear();
	fillVector(vectorValues, 1);  //{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}
	fillSet(setValues, 2);  //{2, 4, 6, 8, 10, 12, 14, 16, 18, 20}
	set<int> joinedValues = QueryEvaluator::intersectWithCurrentValues(vectorValues, setValues);
	CPPUNIT_ASSERT_EQUAL(5, (int)joinedValues.size());

	//Assert that the intersected values are the proper ones
	set<int>::iterator itr = joinedValues.begin();
	CPPUNIT_ASSERT_EQUAL(2, *itr);
	++itr;
	CPPUNIT_ASSERT_EQUAL(4, *itr);
	++itr;
	CPPUNIT_ASSERT_EQUAL(6, *itr);
	++itr;
	CPPUNIT_ASSERT_EQUAL(8, *itr);
	++itr;
	CPPUNIT_ASSERT_EQUAL(10, *itr);
}

void QueryEvaluatorTest::testIsValueExist() {
	set<int> setValues;

	//Empty set should return false
	CPPUNIT_ASSERT(!QueryEvaluator::isValueExist(setValues, 0));

	//Search for non existent values should return false
	fillSet(setValues, 1);
	CPPUNIT_ASSERT(!QueryEvaluator::isValueExist(setValues, 0));
	CPPUNIT_ASSERT(!QueryEvaluator::isValueExist(setValues, 11));
	
	//Search for existing values should return true
	CPPUNIT_ASSERT(QueryEvaluator::isValueExist(setValues, 1));
	CPPUNIT_ASSERT(QueryEvaluator::isValueExist(setValues, 10));
}
