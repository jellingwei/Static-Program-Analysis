#ifndef TestParser_h
#define TestParser_h

#include <cppunit/extensions/HelperMacros.h>

class ParserTest : public CPPUNIT_NS::TestFixture 
{ 
	CPPUNIT_TEST_SUITE( ParserTest ); 
	CPPUNIT_TEST( testInit );
	

	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	// method to test the initialisation of the parser
	void testInit();

};
#endif
    