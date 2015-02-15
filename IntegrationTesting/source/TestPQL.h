#pragma once

#include <cppunit/extensions/HelperMacros.h>

/*	Test PQL
 *
 *	Test Source: test/i_src.txt
 */

class PQLTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( PQLTest );
CPPUNIT_TEST( testPQL );
CPPUNIT_TEST_SUITE_END();

public:
void setUp();
void tearDown();
void testPQL();
};