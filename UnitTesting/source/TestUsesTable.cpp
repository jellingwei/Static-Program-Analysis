#include <cppunit/config/SourcePrefix.h>
#include "UsesTable.h"
#include "TestUsesTable.h"

#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( UsesTableTest ); 

void 
UsesTableTest::setUp() {
}

void 
UsesTableTest::tearDown() {
}

void UsesTableTest::testInit() {
	UsesTable usesTable;
	usesTable.init(20);

	pair<vector<int>, vector<int>> result = usesTable.getAllUsesPair();
	CPPUNIT_ASSERT_EQUAL((int)result.first.size(), 0);
	CPPUNIT_ASSERT_EQUAL((int)result.first.size(), 0);

	
	return;
}

void UsesTableTest::testInsert() {

	// case : insert valid values
	UsesTable usesTable;
	usesTable.init(20);

	CPPUNIT_ASSERT(!usesTable.isUses(1, 2));

	CPPUNIT_ASSERT(usesTable.setUses(1, 2));

	CPPUNIT_ASSERT(usesTable.isUses(1, 2));
	vector<int> result = usesTable.getUsesStmtNum(2);
	CPPUNIT_ASSERT_EQUAL(1, result[0]);
	result = usesTable.getUsesVarForStmt(1);
	CPPUNIT_ASSERT_EQUAL(2, result[0]);

	//pair<vector<int>, vector<int> > allPairs = usesTable.getAllUsesPair();
	//CPPUNIT_ASSERT_EQUAL(1, (int)allPairs.first.size());
	//CPPUNIT_ASSERT_EQUAL(1, (int)allPairs.second.size());
	//CPPUNIT_ASSERT_EQUAL(1, allPairs.first.at(0));
	//CPPUNIT_ASSERT_EQUAL(2, allPairs.second.at(0));

	// invalid insertion values
	try {
		usesTable.setUses(1, -1);
		CPPUNIT_ASSERT_MESSAGE("Exception not caught", false);
	} catch (exception) { // using this instead of CPPUNIT_ASSERT_THROW as CPPUNIT_ASSERT_THROW expects an exception which isn't std::exception
	}

	
	//
}


void UsesTableTest::testAllPairs() {
	UsesTable usesTable;
	usesTable.init(20);

	CPPUNIT_ASSERT_EQUAL(false, usesTable.isValid());

	usesTable.setUses(1, 1);
	usesTable.setUses(1, 2);
	usesTable.setUses(2, 3);

	CPPUNIT_ASSERT_EQUAL(true, usesTable.isValid());

}


void UsesTableTest::testIsUses() {
	UsesTable usesTable;
	usesTable.init(20);

	usesTable.setUses(2, 3);
	// test valid case, success
	CPPUNIT_ASSERT(usesTable.isUses(2, 3));

	// test valid case, failure
	CPPUNIT_ASSERT(!usesTable.isUses(2, 4));

	// test invalid case
	try {
		usesTable.isUses(1, -1);
		CPPUNIT_ASSERT_MESSAGE("Exception not caught", false);
	} catch (exception) { // using this instead of CPPUNIT_ASSERT_THROW as CPPUNIT_ASSERT_THROW expects an exception which isn't std::exception
	}

}

void UsesTableTest::testGetUsesStmtNum() {
	UsesTable usesTable;
	usesTable.init(20);

	usesTable.setUses(2, 3);

	vector<int> result = usesTable.getUsesStmtNum(2);
	CPPUNIT_ASSERT(result.empty());

	result = usesTable.getUsesStmtNum(3);
	CPPUNIT_ASSERT_EQUAL(1, (int)result.size());
	CPPUNIT_ASSERT_EQUAL(2, result.at(0));

	usesTable.setUses(3, 3);
	result = usesTable.getUsesStmtNum(3);
	CPPUNIT_ASSERT_EQUAL(2, (int)result.size());
	CPPUNIT_ASSERT(find(result.begin(), result.end(), 2) != result.end());
	CPPUNIT_ASSERT(find(result.begin(), result.end(), 3) != result.end());
}

void UsesTableTest::testGetUsesVarForStmt() {
	UsesTable usesTable;
	usesTable.init(20);

	usesTable.setUses(2, 2);

	vector<int> result = usesTable.getUsesVarForStmt(3);
	CPPUNIT_ASSERT(result.empty());

	result = usesTable.getUsesVarForStmt(2);
	CPPUNIT_ASSERT_EQUAL(1, (int)result.size());
	CPPUNIT_ASSERT_EQUAL(2, result.at(0));

	usesTable.setUses(2, 3);
	result = usesTable.getUsesVarForStmt(2);
	CPPUNIT_ASSERT_EQUAL(2, (int)result.size());
	CPPUNIT_ASSERT(find(result.begin(), result.end(), 2) != result.end());
	CPPUNIT_ASSERT(find(result.begin(), result.end(), 3) != result.end());

}
