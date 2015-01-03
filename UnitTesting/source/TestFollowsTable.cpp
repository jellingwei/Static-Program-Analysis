#include <cppunit/config/SourcePrefix.h>
#include "FollowsTable.h"
#include "TestFollowsTable.h"

#include <iostream>
#include <string>
#include <fstream>
#include "TNode.h"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FollowsTableTest ); 

void 
FollowsTableTest::setUp() {
}

void 
FollowsTableTest::tearDown() {
}

void FollowsTableTest::testInit() {
	FollowsTable followsTable;

	pair<vector<int>, vector<int>> result = followsTable.getAllFollowsPairs(false);

	CPPUNIT_ASSERT_EQUAL((int)result.first.size(), 0);
	CPPUNIT_ASSERT_EQUAL((int)result.second.size(), 0);
	
	return;
}

void FollowsTableTest::testInsert() {
	FollowsTable followsTable;
	TNode* node1 = new TNode(Assign, 1, -1);
	TNode* node2 = new TNode(Assign, 2, -1);

	bool status = followsTable.setFollows(node1, node2);
	CPPUNIT_ASSERT(status);

	pair<vector<int>, vector<int>> result = followsTable.getAllFollowsPairs(false);
	CPPUNIT_ASSERT_EQUAL(1, (int)result.first.size());
	CPPUNIT_ASSERT_EQUAL(1, (int)result.second.size());
	CPPUNIT_ASSERT_EQUAL(1, (int)result.first.at(0));
	CPPUNIT_ASSERT_EQUAL(2, (int)result.second.at(0));

	CPPUNIT_ASSERT(followsTable.checkFollows(1, 2));


	status = followsTable.setFollows(&TNode(While, 5, -1), &TNode(Assign, 4, -1));
	CPPUNIT_ASSERT(status);

	result = followsTable.getAllFollowsPairs(false);
	CPPUNIT_ASSERT_EQUAL((int)result.first.size(), 2);
	CPPUNIT_ASSERT_EQUAL((int)result.second.size(), 2);
	CPPUNIT_ASSERT_EQUAL((int)result.first.at(1), 5);
	CPPUNIT_ASSERT_EQUAL((int)result.second.at(1), 4);

	CPPUNIT_ASSERT(followsTable.checkFollows(5, 4));

	// invalid case: stmt num = -1 and 1
	status = followsTable.setFollows(&TNode(While, -1, -1), &TNode(Assign, 1, -1));
	CPPUNIT_ASSERT(!status);
	result = followsTable.getAllFollowsPairs(false);
	CPPUNIT_ASSERT_EQUAL((int)result.first.size(), 2);
	CPPUNIT_ASSERT_EQUAL((int)result.second.size(), 2);

	CPPUNIT_ASSERT(!followsTable.checkFollows(-1, 1));

	// invalid case: stmt num = 5 and -1
	status = followsTable.setFollows(&TNode(Assign, 5, -1), &TNode(Assign, -1, -1));
	CPPUNIT_ASSERT(!status);
	result = followsTable.getAllFollowsPairs(false);
	CPPUNIT_ASSERT_EQUAL((int)result.first.size(), 2);
	CPPUNIT_ASSERT_EQUAL((int)result.second.size(), 2);

	CPPUNIT_ASSERT(!followsTable.checkFollows(5, -1));


	try {
		followsTable.setFollows(NULL, &TNode(Assign, 5, -1));
	//	CPPUNIT_ASSERT( false);
	}   
	catch (exception e) { // using this instead of CPPUNIT_ASSERT_THROW as CPPUNIT_ASSERT_THROW seems to expect an exception which is not std::exception
	} 
}
 
