#include <cppunit/config/SourcePrefix.h>
#include "VarTable.h"
#include "TestVarTable.h"

#include <iostream>
#include <string>
#include <fstream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( VarTableTest ); 

void 
VarTableTest::setUp() {
}

void 
VarTableTest::tearDown() {
}

void VarTableTest::testInit() {
	VarTable varTable;
	CPPUNIT_ASSERT_EQUAL(varTable.getVarTableSize(), 0);
	
	return;
}

void VarTableTest::testInsert() {
	VarTable varTable;
	CPPUNIT_ASSERT_EQUAL(varTable.getVarTableSize(), 0);

	CPPUNIT_ASSERT_EQUAL(varTable.insertVar("x", 1), 1); 
	CPPUNIT_ASSERT_EQUAL(varTable.getVarTableSize(), 1);

	CPPUNIT_ASSERT_EQUAL(varTable.insertVar("y", 10), 2); 
	CPPUNIT_ASSERT_EQUAL(varTable.getVarTableSize(), 2);

	// inserting an existing variable does not generate a new table entry
	CPPUNIT_ASSERT_EQUAL(varTable.insertVar("x", 12), 1); 
	CPPUNIT_ASSERT_EQUAL(varTable.getVarTableSize(), 2);

	// invalid parameters should cause insertVar to reject the entry and throw exception
	try {
		varTable.insertVar("z", -5); 
		CPPUNIT_ASSERT_MESSAGE("exception not thrown", false);
	} catch (exception e) {
	}
	try{
		varTable.insertVar("", 5); 
		CPPUNIT_ASSERT_MESSAGE("exception not thrown", false);
	} catch (exception e) {
	}
	CPPUNIT_ASSERT_EQUAL(varTable.getVarTableSize(), 2);

}

void VarTableTest::testGetVarIndex() {
	VarTable varTable;

	int var1 = varTable.insertVar("x", 1); 
	CPPUNIT_ASSERT_EQUAL(varTable.getVarIndex("x"), var1);

	int var2 = varTable.insertVar("y", 1); 
	CPPUNIT_ASSERT_EQUAL(varTable.getVarIndex("y"), var2);

	CPPUNIT_ASSERT_EQUAL(varTable.getVarIndex("x"), var1);

	// test with a variable that is not added yet
	CPPUNIT_ASSERT_EQUAL(varTable.getVarIndex("variableNotAdded"), -1);

	try {
		varTable.getVarIndex("");
		CPPUNIT_ASSERT_MESSAGE("exception not thrown", false);
	} catch (exception e) {
	}
}
 
void VarTableTest::testGetVarName() {
	VarTable varTable;

	int var1 = varTable.insertVar("x", 1); 
	CPPUNIT_ASSERT(strcmp(varTable.getVarName(var1).c_str(), "x") == 0);

	int var2 = varTable.insertVar("y", 1); 
	CPPUNIT_ASSERT(strcmp(varTable.getVarName(var2).c_str(), "y") == 0);

	CPPUNIT_ASSERT(strcmp(varTable.getVarName(var1).c_str(), "x") == 0);  

	// test with a variable index that is not in vartable
	CPPUNIT_ASSERT(strcmp(varTable.getVarName(50000).c_str(), "") == 0);  

	try {
		varTable.getVarName(-2).c_str();
		CPPUNIT_ASSERT_MESSAGE("exception not thrown", false);
	} catch (exception e) {
	}
	
}
 
void testGetStmtNum() {
	

}