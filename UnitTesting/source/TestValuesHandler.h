#pragma once

#include <cppunit/extensions/HelperMacros.h>

class ValuesHandlerTest : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( ValuesHandlerTest );
	CPPUNIT_TEST( testIsValueExistInSet );
	CPPUNIT_TEST( testFindIndexInMainTable );
	CPPUNIT_TEST( testIsExistInMainTable );
	CPPUNIT_TEST( testIsExistInSingletonTable );
	CPPUNIT_TEST( testRemoveFromSingletonTable );
	CPPUNIT_TEST( testGetIntermediateValuesSetInMain );
	CPPUNIT_TEST( testGetIntermediateValuesInMain );
	CPPUNIT_TEST( testHashIntersectWithMainTable );
	CPPUNIT_TEST( testHashJoinWithMainTable );
	CPPUNIT_TEST( testJoinWithMainTable );
	CPPUNIT_TEST( testGetPairBySingletonIntersect );
	CPPUNIT_TEST( testHashIntersectWithSingletonTable );
	CPPUNIT_TEST( testProcessWithSingletonTable );
	CPPUNIT_TEST( testMergeSingletonToMain );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();
	void testIsValueExistInSet();
	void testFindIndexInMainTable();
	void testIsExistInMainTable();
	void testIsExistInSingletonTable();
	void testRemoveFromSingletonTable();
	void testGetIntermediateValuesSetInMain();
	void testGetIntermediateValuesInMain();
	void testHashIntersectWithMainTable();
	void testHashJoinWithMainTable();
	void testJoinWithMainTable();
	void testGetPairBySingletonIntersect();
	void testHashIntersectWithSingletonTable();
	void testProcessWithSingletonTable();
	void testMergeSingletonToMain();
};
