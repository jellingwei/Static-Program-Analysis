#pragma once

#include <cppunit/extensions/HelperMacros.h>

class PatternMatchTest: public CPPUNIT_NS::TestFixture 
{
	CPPUNIT_TEST_SUITE( PatternMatchTest ); 
	CPPUNIT_TEST( testAssign );
	CPPUNIT_TEST( testWhile );
	CPPUNIT_TEST_SUITE_END();

	public:
		void setUp();
		void testAssign();
		void testWhile();
};