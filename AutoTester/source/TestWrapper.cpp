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
	//@todo maybe a front end controller
	cout<<"TestWrapper parsing source"<<std::endl;
	Parser::initParser(filename);
	Parser::parseProgram();

	cout<<"Design Extractor"<<std::endl;
	DesignExtractor extractor;
	vector<TNode*> callNodes = extractor.obtainCallStatementsInTopologicalOrder();
	extractor.setModifiesForCallStatements(callNodes);
	extractor.setUsesForCallStatements(callNodes);

	//extractor.constructCfg();
	//extractor.constructStatisticsTable();
}

// method to evaluating a query
//Note: PQL query comes in the form of a single line
void TestWrapper::evaluate(std::string query, std::list<std::string>& results){

	std::cout<<"TestWrapper evaluating query"<<std::endl;
	PQLController::evaluate(query, results);
}
