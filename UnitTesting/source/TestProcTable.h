#pragma once

#include <cppunit/extensions/HelperMacros.h>


class ProcTableTest : public CPPUNIT_NS::TestFixture 
{ 
	CPPUNIT_TEST_SUITE( ProcTableTest ); 
	CPPUNIT_TEST( testInit );
	CPPUNIT_TEST( testInsert );
	CPPUNIT_TEST( testGetProcName );
	CPPUNIT_TEST( testGetProcIndex );

	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	void testInit();
	void testInsert();
	void testGetProcName();
	void testGetProcIndex();

};
