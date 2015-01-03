#pragma once

#include <cppunit/extensions/HelperMacros.h>

class QueryEvaluatorTest : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( QueryEvaluatorTest );
	CPPUNIT_TEST( testIntersectWithCurrentValues );
	CPPUNIT_TEST( testIsValueExist );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();
	void testIntersectWithCurrentValues();
	void testIsValueExist();
};
