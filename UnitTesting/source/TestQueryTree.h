#pragma once

#include <cppunit/extensions/HelperMacros.h>

class QueryTreeTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( QueryTreeTest );
CPPUNIT_TEST( testSynonym );
CPPUNIT_TEST( testQNode );
CPPUNIT_TEST( testQueryTree );
CPPUNIT_TEST_SUITE_END();

public:
void setUp();
void tearDown();
void testSynonym();
void testQNode();
void testQueryTree();

};