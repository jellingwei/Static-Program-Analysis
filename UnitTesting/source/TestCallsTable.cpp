#include <cppunit/config/SourcePrefix.h>
#include "CallsTable.h"
#include "TestCallsTable.h"

#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( CallsTableTest ); 

void 
CallsTableTest::setUp() {
}

void 
CallsTableTest::tearDown() {
}

void CallsTableTest::testInit() {
	CallsTable callsTable;

	pair<vector<int>, vector<int>> result = callsTable.getAllCallsPairs(true);
	CPPUNIT_ASSERT_EQUAL((int)result.first.size(), 0);

	pair<vector<int>, vector<int>> result1 = callsTable.getAllCallsPairs(false);
	CPPUNIT_ASSERT_EQUAL((int)result1.first.size(), 0);

	
	return;
}

void CallsTableTest::testInsert() {

	// case : insert valid values
	CallsTable callsTable;

	CPPUNIT_ASSERT(!callsTable.isCalls(1, 2, false));
	CPPUNIT_ASSERT(!callsTable.isCalls(1, 2, true));

	CPPUNIT_ASSERT(callsTable.setCalls(1, 2));
	CPPUNIT_ASSERT(callsTable.setCalls(0, 3)); // boundary case

	CPPUNIT_ASSERT(callsTable.isCalls(1, 2, false));
	CPPUNIT_ASSERT(callsTable.isCalls(1, 2, true));

	CPPUNIT_ASSERT(callsTable.isCalls(0, 3, false));
	CPPUNIT_ASSERT(callsTable.isCalls(0, 3, true));

	vector<int> result = callsTable.getProcsCalling(2, false);
	CPPUNIT_ASSERT(find(result.begin(), result.end(), 1) != result.end());
	result = callsTable.getProcsCalling(2, true);
	CPPUNIT_ASSERT(find(result.begin(), result.end(), 1) != result.end());

	vector<int> result1 = callsTable.getProcsCalledBy(1, false);
	CPPUNIT_ASSERT(find(result1.begin(), result1.end(), 2) != result1.end());
	result1 = callsTable.getProcsCalledBy(1, true);
	CPPUNIT_ASSERT(find(result1.begin(), result1.end(), 2) != result1.end());

	result = callsTable.getProcsCalling(3, false);
	CPPUNIT_ASSERT(find(result.begin(), result.end(), 0) != result.end());
	result = callsTable.getProcsCalling(3, true);
	CPPUNIT_ASSERT(find(result.begin(), result.end(), 0) != result.end());

	result = callsTable.getProcsCalledBy(0, false);
	CPPUNIT_ASSERT(find(result.begin(), result.end(), 3) != result.end());
	result = callsTable.getProcsCalledBy(0, true);
	CPPUNIT_ASSERT(find(result.begin(), result.end(), 3) != result.end());

	/*pair<vector<int>, vector<int> > allPairs = callsTable.getAllCallsPairs(false);
	CPPUNIT_ASSERT_EQUAL(1, (int)allPairs.first.size());
	CPPUNIT_ASSERT_EQUAL(1, (int)allPairs.second.size());
	CPPUNIT_ASSERT_EQUAL(1, allPairs.first.at(0));
	CPPUNIT_ASSERT_EQUAL(2, allPairs.second.at(0));

	pair<vector<int>, vector<int> > allPairs = callsTable.getAllCallsPairs(true);
	CPPUNIT_ASSERT_EQUAL(1, (int)allPairs.first.size());
	CPPUNIT_ASSERT_EQUAL(1, (int)allPairs.second.size());
	CPPUNIT_ASSERT_EQUAL(1, allPairs.first.at(0));
	CPPUNIT_ASSERT_EQUAL(2, allPairs.second.at(0)); */

	// invalid insertion values
	try {
		callsTable.setCalls(1, -1);
		CPPUNIT_ASSERT_MESSAGE("Exception not caught", false);
	} catch (exception) { // using this instead of CPPUNIT_ASSERT_THROW as CPPUNIT_ASSERT_THROW expects an exception which isn't std::exception
	}

	try {
		callsTable.setCalls(-1, 1);
		CPPUNIT_ASSERT_MESSAGE("Exception not caught", false);
	} catch (exception) { // using this instead of CPPUNIT_ASSERT_THROW as CPPUNIT_ASSERT_THROW expects an exception which isn't std::exception
	}

	/*allPairs = callsTable.getAllCallsPairs(false);
	CPPUNIT_ASSERT_EQUAL(1, (int)allPairs.first.size());
	CPPUNIT_ASSERT_EQUAL(1, (int)allPairs.second.size());*/

	//
}


/*void CallsTableTest::testAllPairs() {
	CallsTable callsTable;

	callsTable.setUses(1, 1);
	callsTable.setUses(1, 2);
	callsTable.setUses(2, 3);

	pair<vector<int>, vector<int> > allPairs = callsTable.getAllUsesPair();

	CPPUNIT_ASSERT_EQUAL(3, (int)allPairs.first.size());
	CPPUNIT_ASSERT_EQUAL(3, (int)allPairs.second.size());

	
	CPPUNIT_ASSERT(find(allPairs.first.begin(), allPairs.first.end(), 1) != allPairs.first.end());
	CPPUNIT_ASSERT(find(allPairs.first.begin(), allPairs.first.end(), 2) != allPairs.first.end());
	CPPUNIT_ASSERT(find(allPairs.first.begin(), allPairs.first.end(), 3) == allPairs.first.end());

	CPPUNIT_ASSERT(find(allPairs.second.begin(), allPairs.second.end(), 1) != allPairs.second.end());
	CPPUNIT_ASSERT(find(allPairs.second.begin(), allPairs.second.end(), 2) != allPairs.second.end());
	CPPUNIT_ASSERT(find(allPairs.second.begin(), allPairs.second.end(), 3) != allPairs.second.end());

}*/


void CallsTableTest::testIsCalls() {
	CallsTable callsTable;

	callsTable.setCalls(2, 3);
	callsTable.setCalls(0, 1);

	// test valid case, success
	CPPUNIT_ASSERT(callsTable.isCalls(2, 3, false));
	CPPUNIT_ASSERT(callsTable.isCalls(2, 3, true));

	CPPUNIT_ASSERT(callsTable.isCalls(0, 1, false));
	CPPUNIT_ASSERT(callsTable.isCalls(0, 1, true));

	// test valid case, failure
	CPPUNIT_ASSERT(!callsTable.isCalls(2, 4, false));
	CPPUNIT_ASSERT(!callsTable.isCalls(2, 4, true));

	CPPUNIT_ASSERT(!callsTable.isCalls(0, 0, false));
	CPPUNIT_ASSERT(!callsTable.isCalls(0, 0, true));

	// test invalid case
	try {
		callsTable.isCalls(1, -1, false);
		CPPUNIT_ASSERT_MESSAGE("Exception not caught", false);
	} catch (exception) { // using this instead of CPPUNIT_ASSERT_THROW as CPPUNIT_ASSERT_THROW expects an exception which isn't std::exception
	}

	try {
		callsTable.isCalls(-1, 1, false);
		CPPUNIT_ASSERT_MESSAGE("Exception not caught", false);
	} catch (exception) { // using this instead of CPPUNIT_ASSERT_THROW as CPPUNIT_ASSERT_THROW expects an exception which isn't std::exception
	}
}

void CallsTableTest::testGetProcsCalling() {
	CallsTable callsTable;
	callsTable.setCalls(2, 3);
	callsTable.setCalls(0, 1);

	vector<int> result = callsTable.getProcsCalling(2, false);
	CPPUNIT_ASSERT(result.empty());
	result = callsTable.getProcsCalling(2, true);
	CPPUNIT_ASSERT(result.empty());

	result = callsTable.getProcsCalling(0, false);
	CPPUNIT_ASSERT(result.empty());
	result = callsTable.getProcsCalling(0, true);
	CPPUNIT_ASSERT(result.empty());

	result = callsTable.getProcsCalling(3, false);
	CPPUNIT_ASSERT_EQUAL(1, (int)result.size());
	CPPUNIT_ASSERT(find(result.begin(), result.end(), 2) != result.end());

	result = callsTable.getProcsCalling(3, true);
	CPPUNIT_ASSERT_EQUAL(1, (int)result.size());
	CPPUNIT_ASSERT(find(result.begin(), result.end(), 2) != result.end());

	result = callsTable.getProcsCalling(1, false);
	CPPUNIT_ASSERT_EQUAL(1, (int)result.size());
	CPPUNIT_ASSERT(find(result.begin(), result.end(), 0) != result.end());

	result = callsTable.getProcsCalling(1, true);
	CPPUNIT_ASSERT_EQUAL(1, (int)result.size());
	CPPUNIT_ASSERT(find(result.begin(), result.end(), 0) != result.end());

	callsTable.setCalls(3, 3);
	result = callsTable.getProcsCalling(3, false);
	CPPUNIT_ASSERT_EQUAL(2, (int)result.size());
	CPPUNIT_ASSERT(find(result.begin(), result.end(), 2) != result.end());
	CPPUNIT_ASSERT(find(result.begin(), result.end(), 3) != result.end());

	result = callsTable.getProcsCalling(3, true);
	CPPUNIT_ASSERT_EQUAL(2, (int)result.size());
	CPPUNIT_ASSERT(find(result.begin(), result.end(), 2) != result.end());
	CPPUNIT_ASSERT(find(result.begin(), result.end(), 3) != result.end());
}

void CallsTableTest::testGetProcsCalledBy() {
	CallsTable callsTable;
	callsTable.setCalls(2, 2);

	vector<int> result = callsTable.getProcsCalledBy(3, false);
	CPPUNIT_ASSERT(result.empty());

	result = callsTable.getProcsCalledBy(2, false);
	CPPUNIT_ASSERT_EQUAL(1, (int)result.size());
	CPPUNIT_ASSERT(find(result.begin(), result.end(), 2) != result.end());

	callsTable.setCalls(2, 3);
	result = callsTable.getProcsCalledBy(2, false);
	CPPUNIT_ASSERT_EQUAL(2, (int)result.size());
	CPPUNIT_ASSERT(find(result.begin(), result.end(), 2) != result.end());
	CPPUNIT_ASSERT(find(result.begin(), result.end(), 3) != result.end());

}
