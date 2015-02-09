#include "FrontEndController.h"
#include "SourceParser.h"
#include "DesignExtractor.h"

void FrontEndController::constructPkb(std::string filename)
{
	try {
		Parser::initParser(filename);
		Parser::parseProgram();

		cout<<"Design Extractor"<<std::endl;
		DesignExtractor extractor;
		vector<TNode*> callNodes = extractor.obtainCallStatementsInTopologicalOrder();
		extractor.setModifiesForCallStatements(callNodes);
		extractor.setUsesForCallStatements(callNodes);

		//extractor.constructCfg();
		//extractor.constructStatisticsTable();
	} catch (exception e) {
		cout << "Failed to construct PKB" << e.what() << endl;
	}
}