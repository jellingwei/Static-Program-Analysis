#include "PQLController.h"

void PQLController::evaluate(std::string query, std::list<std::string> &results)
{
	try {
		QueryParser::initParser(query);
		BOOLEAN_ res = QueryParser::parseQuery();

		QueryTree* qT = QueryParser::getQueryTree();
		vector<Synonym> queryResults;

		queryResults = QueryEvaluator::processQueryTree(qT);
		ResultProjector::projectResultToList(queryResults, results);
	} catch (exception e) {
		results.push_front(e.what());
	}
}