#pragma once

#include <cppunit/extensions/HelperMacros.h>

class CallsTableTest : public CPPUNIT_NS::TestFixture 
{ 
	CPPUNIT_TEST_SUITE( CallsTableTest ); 
	CPPUNIT_TEST( testInit );
	CPPUNIT_TEST(testInsert);
	
	CPPUNIT_TEST(testIsCalls);
	CPPUNIT_TEST(testGetProcsCalling);
	CPPUNIT_TEST(testGetProcsCalledBy);

	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	void testInit();
	void testInsert();

	void testIsCalls();
	void testGetProcsCalling();
	void testGetProcsCalledBy();

};
