#ifndef TestParser_h
#define TestParser_h

#include <cppunit/extensions/HelperMacros.h>

class FollowsTableTest : public CPPUNIT_NS::TestFixture 
{ 
	CPPUNIT_TEST_SUITE( FollowsTableTest ); 
	CPPUNIT_TEST( testInit );
	CPPUNIT_TEST(testInsert);

	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	void testInit();
	void testInsert();
};
#endif
    