#pragma once

#include <cppunit/extensions/HelperMacros.h>

/*	Test PKB
 *
 *	Test Source: test/i_src.txt
 *	procedure Test {
 *	1	a = 1;			
 *	2	c = d + f;					
 *	3	while a {					
 *	4		b = 2; }
 *	5	d = 1 + d;
 *	6	e = a + f;
 *	7	while b {
 *	8		while c {
 *	9			f = e + 1; }
 *	10		e = f + 2; }
 *	11	a = 3;
 *	}
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
