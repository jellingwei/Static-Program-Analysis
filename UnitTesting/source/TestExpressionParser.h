#pragma once

#include <cppunit/extensions/HelperMacros.h>

class ExpressionParserTest : public CPPUNIT_NS::TestFixture 
{ 
	CPPUNIT_TEST_SUITE( ExpressionParserTest ); 
	CPPUNIT_TEST( testSingleNode );
	CPPUNIT_TEST( testSimpleCases );
	CPPUNIT_TEST( testComplexCases );
	CPPUNIT_TEST( testSideEffects );
	CPPUNIT_TEST( testQuery );
	

	CPPUNIT_TEST_SUITE_END();

public:
	ExpressionParser* exprParser;

	void setUp();
	void tearDown();

	void testSingleNode();
	void testSimpleCases();
	void testComplexCases();
	void testSideEffects();
	void testQuery();
	
};
