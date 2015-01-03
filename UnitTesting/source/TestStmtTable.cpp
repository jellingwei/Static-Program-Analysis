#include <cppunit/config/SourcePrefix.h>
#include "StmtTable.h"
#include "TestStmtTable.h"

#include <iostream>
#include <string>
#include <fstream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( StmtTableTest ); 

void 
StmtTableTest::setUp() {
}

void 
StmtTableTest::tearDown() {
}

void StmtTableTest::testInit() {
	StmtTable stmtTable;

	CPPUNIT_ASSERT_EQUAL(0, stmtTable.getSize());
	
	return;
}

void StmtTableTest::testInsert() {
	StmtTable stmtTable;
	
	stmtTable.insertStmt(1, "assign");
	stmtTable.insertStmt(2, "while");
	CPPUNIT_ASSERT_EQUAL(2, stmtTable.getSize());

	CPPUNIT_ASSERT(stmtTable.isAssign(1));
	CPPUNIT_ASSERT(strcmp("assign", stmtTable.getType(1).c_str()) == 0);
	CPPUNIT_ASSERT(strcmp("while", stmtTable.getType(2).c_str()) == 0);

	
	return;
}

void StmtTableTest::testGetStmtNumForType() {
	StmtTable stmtTable;

	stmtTable.insertStmt(1, "assign");
	stmtTable.insertStmt(2, "assign");

	vector<int> result = stmtTable.getStmtNumForType("assign");
	CPPUNIT_ASSERT_EQUAL(2, (int)result.size());


	stmtTable.insertStmt(3, "while");
	stmtTable.insertStmt(4, "while");
	stmtTable.insertStmt(5, "while");

	result = stmtTable.getStmtNumForType("while");
	CPPUNIT_ASSERT_EQUAL(3, (int)result.size());
	result = stmtTable.getStmtNumForType("assign");
	CPPUNIT_ASSERT_EQUAL(2, (int)result.size());

	stmtTable.insertStmt(6, "while");
	stmtTable.insertStmt(7, "assign");

	result = stmtTable.getStmtNumForType("while");
	CPPUNIT_ASSERT_EQUAL(4, (int)result.size());
	result = stmtTable.getStmtNumForType("assign");
	CPPUNIT_ASSERT_EQUAL(3, (int)result.size());

	// inserting a previously inserted stmt# should not cause any changes
	stmtTable.insertStmt(7, "while");
	stmtTable.insertStmt(7, "assign");

	result = stmtTable.getStmtNumForType("while");
	CPPUNIT_ASSERT_EQUAL(4, (int)result.size());
	result = stmtTable.getStmtNumForType("assign");
	CPPUNIT_ASSERT_EQUAL(3, (int)result.size());


	// get all stmt and all prog_lines return the same results
	result = stmtTable.getStmtNumForType("stmt");
	vector<int> result1 = stmtTable.getStmtNumForType("prog_line");
	CPPUNIT_ASSERT(result.size() == result1.size());
	CPPUNIT_ASSERT(result.front() == result1.front());
	CPPUNIT_ASSERT(result.back() == result1.back());
}