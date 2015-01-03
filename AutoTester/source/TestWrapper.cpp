#include "TestWrapper.h"
#include <iostream> 

// implementation code of WrapperFactory - do NOT modify the next 5 lines
AbstractWrapper* WrapperFactory::wrapper = 0;
AbstractWrapper* WrapperFactory::createWrapper() {
	if (wrapper == 0) wrapper = new TestWrapper;
	return wrapper;
}
// Do not modify the following line
volatile bool TestWrapper::GlobalStop = false;

// a default constructor
TestWrapper::TestWrapper() {
	// create any objects here as instance variables of this class
	// as well as any initialization required for your spa program
}

// method for parsing the SIMPLE source
void TestWrapper::parse(std::string filename) {

	std::cout<<"TestWrapper parsing source"<<std::endl;
	Parser::initParser(filename);

	Parser::parseProgram();

	// ...rest of your code...
}

// method to evaluating a query
//Note: PQL query comes in the form of a single line
void TestWrapper::evaluate(std::string query, std::list<std::string>& results){

	std::cout<<"TestWrapper evaluating query"<<std::endl;
	QueryParser::initParser(query);

	bool res = QueryParser::parseQuery();

	// ...code to evaluate query...

	// store the answers to the query in the results list (it is initially empty)
	// each result must be a string.
	QueryTree* qT = QueryParser::getQueryTree();
	vector<Synonym> queryResults;
	try {
		queryResults = QueryEvaluator::processQueryTree(qT);
	} catch (exception e) {
	}
	ResultProjector::projectResultToList(queryResults, results);
}
