#pragma once

#include <cppunit/extensions/HelperMacros.h>

class ExpressionParserTest : public CPPUNIT_NS::TestFixture 
{ 
	CPPUNIT_TEST_SUITE( ExpressionParserTest ); 
	CPPUNIT_TEST( testInit );
	

	CPPUNIT_TEST_SUITE_END();

public:
	ExpressionParser* exprParser;
	int varX;
	int varY;
	void setUp();
	void tearDown();

	void testInit();
	
};
