#pragma once

#include <cppunit/extensions/HelperMacros.h>

class ConstantTableTest : public CPPUNIT_NS::TestFixture 
{ 
	CPPUNIT_TEST_SUITE( ConstantTableTest ); 
	CPPUNIT_TEST( testInit );
	CPPUNIT_TEST( testInsert );

	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	void testInit();
	void testInsert();
	void testGetConstantIndex();

};

    