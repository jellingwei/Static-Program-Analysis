#include <cppunit/config/SourcePrefix.h>
#include "ProcTable.h"
#include "TestProcTable.h"

#include <iostream>
#include <string>
#include <fstream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ProcTableTest ); 

void 
ProcTableTest::setUp() {
}

void 
ProcTableTest::tearDown() {
}

void ProcTableTest::testInit() {
	ProcTable procTable;
	CPPUNIT_ASSERT_EQUAL(procTable.getProcTableSize(), 0);
	
	return;
}

void ProcTableTest::testInsert() {
	ProcTable procTable;
	CPPUNIT_ASSERT_EQUAL(0, procTable.getProcTableSize());

	CPPUNIT_ASSERT_EQUAL(0, procTable.insertProc("Procedure1")); 
	CPPUNIT_ASSERT_EQUAL(1, procTable.getProcTableSize());

	CPPUNIT_ASSERT_EQUAL(1, procTable.insertProc("Procedure2")); 
	CPPUNIT_ASSERT_EQUAL(2, procTable.getProcTableSize());

	// inserting an existing procedure does not generate a new table entry
	CPPUNIT_ASSERT_EQUAL(1, procTable.insertProc("Procedure2")); 
	CPPUNIT_ASSERT_EQUAL(2, procTable.getProcTableSize());

	// invalid parameters should cause insertProc to reject the entry and throw exception
	try{
		procTable.insertProc(""); 
		CPPUNIT_ASSERT_MESSAGE("exception not thrown", false);
	} catch (exception e) {
	}
	CPPUNIT_ASSERT_EQUAL(2, procTable.getProcTableSize());
}

void ProcTableTest::testGetProcIndex() {
	ProcTable procTable;

	int proc1 = procTable.insertProc("x"); 
	CPPUNIT_ASSERT_EQUAL(procTable.getProcIndex("x"), proc1);

	int proc2 = procTable.insertProc("y"); 
	CPPUNIT_ASSERT_EQUAL(procTable.getProcIndex("y"), proc2);

	CPPUNIT_ASSERT_EQUAL(procTable.getProcIndex("x"), proc1);

	// test with a procedure that is not added yet
	CPPUNIT_ASSERT_EQUAL(procTable.getProcIndex("procedureNotAdded"), -1);

	try {
		procTable.getProcIndex("");
		CPPUNIT_ASSERT_MESSAGE("exception not thrown", false);
	} catch (exception e) {
	}
}
 
void ProcTableTest::testGetProcName() {
	ProcTable procTable;

	int proc1 = procTable.insertProc("x");  
	CPPUNIT_ASSERT(strcmp(procTable.getProcName(proc1).c_str(), "x") == 0);

	int proc2 = procTable.insertProc("y"); 
	CPPUNIT_ASSERT(strcmp(procTable.getProcName(proc2).c_str(), "y") == 0);

	CPPUNIT_ASSERT(strcmp(procTable.getProcName(proc1).c_str(), "x") == 0);

	// test with a procedure index that is not in vartable
	CPPUNIT_ASSERT(strcmp(procTable.getProcName(5000).c_str(), "") == 0);  

	try {
		procTable.getProcName(-2).c_str();
		CPPUNIT_ASSERT_MESSAGE("exception not thrown", false);
	} catch (exception e) {
	}
	
}
