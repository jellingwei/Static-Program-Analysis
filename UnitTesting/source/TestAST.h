#pragma once

#include <cppunit/extensions/HelperMacros.h>

class ASTTest : public CPPUNIT_NS::TestFixture 
{
	CPPUNIT_TEST_SUITE( ASTTest ); 
	CPPUNIT_TEST( testInit );
	CPPUNIT_TEST( testTNode );
	CPPUNIT_TEST( testASTTree );
	CPPUNIT_TEST( testPatternMatch );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void testInit();
	void testTNode();
	void testASTTree();
	void testPatternMatch();
};