#pragma once

#include <cppunit/extensions/HelperMacros.h>

class VarTableTest : public CPPUNIT_NS::TestFixture 
{ 
	CPPUNIT_TEST_SUITE( VarTableTest ); 
	CPPUNIT_TEST( testInit );
	CPPUNIT_TEST( testInsert );
	CPPUNIT_TEST( testGetVarName );
	CPPUNIT_TEST( testGetVarIndex );

	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	void testInit();
	void testInsert();
	void testGetVarName();
	void testGetVarIndex();

};
