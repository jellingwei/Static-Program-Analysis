#include <cppunit/config/SourcePrefix.h>
#include "ConstantTable.h"
#include "TestConstantTable.h"

#include <iostream>
#include <string>
#include <fstream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ConstantTableTest ); 

void 
ConstantTableTest::setUp() {
}

void 
ConstantTableTest::tearDown() {
}

void ConstantTableTest::testInit() {
	ConstantTable constantTable;
	CPPUNIT_ASSERT_EQUAL(constantTable.getSize(), 0);
	
	return;
}

void ConstantTableTest::testInsert() {
	ConstantTable constantTable;
	CPPUNIT_ASSERT_EQUAL(0, constantTable.getSize());

	CPPUNIT_ASSERT(constantTable.insertConstant(1, 1)); 
	CPPUNIT_ASSERT_EQUAL(1, constantTable.getSize());

	CPPUNIT_ASSERT(constantTable.insertConstant(1000, 10)); 
	CPPUNIT_ASSERT_EQUAL(2, constantTable.getSize());

	// inserting an existing variable does not generate a new table entry
	CPPUNIT_ASSERT(constantTable.insertConstant(1, 12)); 
	CPPUNIT_ASSERT_EQUAL(2, constantTable.getSize());

	// test that inserting 0 as a constant works
	CPPUNIT_ASSERT(constantTable.insertConstant(0, 99999)); 
	CPPUNIT_ASSERT_EQUAL(3, constantTable.getSize());

	// invalid parameters should cause insertConstant to reject the entry and throw exception
	try {
		constantTable.insertConstant(6000, -5); 
		CPPUNIT_ASSERT_MESSAGE("exception not thrown", false);
	} catch (std::invalid_argument e) {
	}
	try{
		constantTable.insertConstant(-1, 5); 
		CPPUNIT_ASSERT_MESSAGE("exception not thrown", false);
	} catch (std::invalid_argument e) {
	}
	CPPUNIT_ASSERT_EQUAL(3, constantTable.getSize());

}
