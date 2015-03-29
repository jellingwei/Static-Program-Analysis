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
		try {
			extractor.setModifiesForAssignmentStatements();
			extractor.setUsesForAssignmentStatements();
			extractor.setUsesForContainerStatements();
		} catch (exception e) {
			cout << "failure during design extraction of assignment and container statements" << endl;
			throw e;
		}

		try {
			
			extractor.setModifiesForCallStatements();
			extractor.setUsesForCallStatements();
		} catch (exception e) {
			cout << "failure during design extraction for call statements" << endl;
			throw e;
		}

		extractor.constructCfg();
		extractor.precomputeInformationForAffects();
		extractor.precomputeInformationForNext();
		//extractor.constructStatisticsTable();
	} catch (exception e) {
		cout << "Failed to construct PKB" << e.what() << endl;
	}
}