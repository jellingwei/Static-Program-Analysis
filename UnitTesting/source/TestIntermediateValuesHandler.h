#pragma once

#include <cppunit/extensions/HelperMacros.h>

class IntermediateValuesHandlerTest : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( IntermediateValuesHandlerTest );
	CPPUNIT_TEST( testJoinWithExistingValues );
	CPPUNIT_TEST( testIntersectWithExistingValues );
	CPPUNIT_TEST( testIntersectAndJoinWithExistingValues );
	CPPUNIT_TEST( testFindIntermediateSynonymIndex );
	CPPUNIT_TEST( testIsValueExist );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();
	void testJoinWithExistingValues();
	void testIntersectWithExistingValues();
	void testIntersectAndJoinWithExistingValues();
	void testFindIntermediateSynonymIndex();
	void testIsValueExist();
};
