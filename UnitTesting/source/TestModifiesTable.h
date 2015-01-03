#pragma once

#include <cppunit/extensions/HelperMacros.h>

class ModifiesTableTest : public CPPUNIT_NS::TestFixture 
{ 
	CPPUNIT_TEST_SUITE( ModifiesTableTest ); 
	CPPUNIT_TEST( testInit );
	CPPUNIT_TEST(testInsert);
	CPPUNIT_TEST(testAllPairs);
	CPPUNIT_TEST(testIsModifies);
	CPPUNIT_TEST(testGetModifiesVarForStmt);
	CPPUNIT_TEST(testGetModifiesStmtNum);

	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	void testInit();
	void testInsert();
	void testAllPairs();
	void testIsModifies();
	void testGetModifiesVarForStmt();
	void testGetModifiesStmtNum();

};
