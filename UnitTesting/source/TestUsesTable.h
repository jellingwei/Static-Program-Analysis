#pragma once

#include <cppunit/extensions/HelperMacros.h>

class UsesTableTest : public CPPUNIT_NS::TestFixture 
{ 
	CPPUNIT_TEST_SUITE( UsesTableTest ); 
	CPPUNIT_TEST( testInit );
	CPPUNIT_TEST(testInsert);
	CPPUNIT_TEST(testAllPairs);
	CPPUNIT_TEST(testIsUses);
	CPPUNIT_TEST(testGetUsesVarForStmt);
	CPPUNIT_TEST(testGetUsesStmtNum);

	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	void testInit();
	void testInsert();
	void testAllPairs();
	void testIsUses();
	void testGetUsesVarForStmt();
	void testGetUsesStmtNum();

};
