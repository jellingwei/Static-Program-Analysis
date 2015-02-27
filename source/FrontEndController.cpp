#include "FrontEndController.h"
#include "SourceParser.h"
#include "DesignExtractor.h"

void FrontEndController::constructPkb(std::string filename)
{
	try {
		Parser::initParser(filename);
		bool status = Parser::parseProgram();
		if (!status) {
			throw exception("parsing failed");
		}

		cout<<"Design Extractor"<<std::endl;
		DesignExtractor extractor;
		extractor.setModifiesForAssignmentStatements();
		extractor.setUsesForAssignmentStatements();
		extractor.setUsesForContainerStatements();

		vector<TNode*> callNodes = extractor.obtainCallStatementsInTopologicalOrder();
		extractor.setModifiesForCallStatements(callNodes);
		extractor.setUsesForCallStatements(callNodes);

		extractor.constructCfg();
		//extractor.constructStatisticsTable();
	} catch (exception e) {
		cout << "Failed to construct PKB" << e.what() << endl;
	}
}