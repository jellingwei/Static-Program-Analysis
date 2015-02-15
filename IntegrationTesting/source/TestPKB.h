#pragma once

#include <cppunit/extensions/HelperMacros.h>

/*	Test PKB
 *
 *	Test Source: test/i_src.txt
*/

class PKBTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( PKBTest );
CPPUNIT_TEST( testPKB );
CPPUNIT_TEST_SUITE_END();

public:
void setUp();
void tearDown();
void testPKB();

};
