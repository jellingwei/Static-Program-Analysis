#include <cppunit/config/SourcePrefix.h>
#include "ModifiesTable.h"
#include "TestModifiesTable.h"

#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ModifiesTableTest ); 

void 
ModifiesTableTest::setUp() {
}

void 
ModifiesTableTest::tearDown() {
}

void ModifiesTableTest::testInit() {
	ModifiesTable modifiesTable;
	modifiesTable.init(20);

	pair<vector<int>, vector<int>> result = modifiesTable.getAllModPair();
	CPPUNIT_ASSERT_EQUAL((int)result.first.size(), 0);
	CPPUNIT_ASSERT_EQUAL((int)result.first.size(), 0);

	
	return;
}

void ModifiesTableTest::testInsert() {

	// case : insert valid values
	ModifiesTable modifiesTable;
	modifiesTable.init(20);

	CPPUNIT_ASSERT(!modifiesTable.isModifies(1, 2));

	CPPUNIT_ASSERT(modifiesTable.setModifies(1, 2));
	
	CPPUNIT_ASSERT(modifiesTable.isModifies(1, 2));
	vector<int> result = modifiesTable.getModStmtNum(2);
	CPPUNIT_ASSERT_EQUAL(1, result[0]);
	result = modifiesTable.getModVarForStmt(1);
	CPPUNIT_ASSERT_EQUAL(2, result[0]);
	
	//pair<vector<int>, vector<int> > allPairs = modifiesTable.getAllModPair();
	//CPPUNIT_ASSERT_EQUAL(1, (int)allPairs.first.size());
	//CPPUNIT_ASSERT_EQUAL(1, (int)allPairs.second.size());
	//CPPUNIT_ASSERT_EQUAL(1, allPairs.first.at(0));
	//CPPUNIT_ASSERT_EQUAL(2, allPairs.second.at(0));

	// invalid insertion values
	try {
		modifiesTable.setModifies(1, -1);
		CPPUNIT_ASSERT_MESSAGE("Exception not caught", false);
	} catch (exception) { // using this instead of CPPUNIT_ASSERT_THROW as CPPUNIT_ASSERT_THROW expects an exception which isn't std::exception
	}

	//allPairs = modifiesTable.getAllModPair();
	//CPPUNIT_ASSERT_EQUAL(1, (int)allPairs.first.size());
	//CPPUNIT_ASSERT_EQUAL(1, (int)allPairs.second.size());
	
	//
}


void ModifiesTableTest::testAllPairs() {
	ModifiesTable modifiesTable;
	modifiesTable.init(20);

	modifiesTable.setModifies(1, 1);
	modifiesTable.setModifies(1, 2);
	modifiesTable.setModifies(2, 3);

	
}


void ModifiesTableTest::testIsModifies() {
	ModifiesTable modifiesTable;
	modifiesTable.init(20);

	modifiesTable.setModifies(2, 3);
	// test valid case, success
	CPPUNIT_ASSERT(modifiesTable.isModifies(2, 3));

	// test valid case, failure
	CPPUNIT_ASSERT(!modifiesTable.isModifies(2, 4));

	// test invalid case
	try {
		modifiesTable.isModifies(1, -1);
		CPPUNIT_ASSERT_MESSAGE("Exception not caught", false);
	} catch (exception) { // using this instead of CPPUNIT_ASSERT_THROW as CPPUNIT_ASSERT_THROW expects an exception which isn't std::exception
	}

}

void ModifiesTableTest::testGetModifiesStmtNum() {
	ModifiesTable modifiesTable;
	modifiesTable.setModifies(2, 3);

	vector<int> result = modifiesTable.getModStmtNum(2);
	CPPUNIT_ASSERT(result.empty());

	result = modifiesTable.getModStmtNum(3);
	CPPUNIT_ASSERT_EQUAL(1, (int)result.size());
	CPPUNIT_ASSERT_EQUAL(2, result.at(0));

	modifiesTable.setModifies(3, 3);
	result = modifiesTable.getModStmtNum(3);
	CPPUNIT_ASSERT_EQUAL(2, (int)result.size());
	CPPUNIT_ASSERT(find(result.begin(), result.end(), 2) != result.end());
	CPPUNIT_ASSERT(find(result.begin(), result.end(), 3) != result.end());
}

void ModifiesTableTest::testGetModifiesVarForStmt() {
	ModifiesTable modifiesTable;
	modifiesTable.init(20);

	modifiesTable.setModifies(2, 2);

	vector<int> result = modifiesTable.getModVarForStmt(3);
	CPPUNIT_ASSERT(result.empty());

	result = modifiesTable.getModVarForStmt(2);
	CPPUNIT_ASSERT_EQUAL(1, (int)result.size());
	CPPUNIT_ASSERT_EQUAL(2, result.at(0));

	modifiesTable.setModifies(2, 3);
	result = modifiesTable.getModVarForStmt(2);
	CPPUNIT_ASSERT_EQUAL(2, (int)result.size());
	CPPUNIT_ASSERT(find(result.begin(), result.end(), 2) != result.end());
	CPPUNIT_ASSERT(find(result.begin(), result.end(), 3) != result.end());

}
