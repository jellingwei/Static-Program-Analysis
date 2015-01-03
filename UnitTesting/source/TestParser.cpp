#include <cppunit/config/SourcePrefix.h>
#include "SourceParser.h"
#include "TestParser.h"

#include <iostream>
#include <string>
#include <fstream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ParserTest ); 

void 
ParserTest::setUp() {
}

void 
ParserTest::tearDown() {
}

void ParserTest::testInit() {
	CPPUNIT_ASSERT_EQUAL(Parser::initParser("nonExistentFile.txt"), false);
	
	return;
}
 

