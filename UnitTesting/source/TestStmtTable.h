#pragma once

#include <cppunit/extensions/HelperMacros.h>

class StmtTableTest : public CPPUNIT_NS::TestFixture 
{ 
	CPPUNIT_TEST_SUITE( StmtTableTest ); 
	CPPUNIT_TEST( testInit );
	CPPUNIT_TEST( testInsert );
	CPPUNIT_TEST( testGetStmtNumForType );

	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	void testInit();
	void testInsert();
	void testGetStmtNumForType();

};
