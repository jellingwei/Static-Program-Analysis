#include "PQLController.h"

void PQLController::evaluate(std::string query, std::list<std::string> &results)
{
	try {
		QueryParser::initParser(query);
		BOOLEAN_ res = QueryParser::parseQuery();

		QueryTree* qT = QueryParser::getQueryTree();
		qT = QueryOptimiser::optimiseQueryTree(qT);
		vector<Synonym> queryResults;

		queryResults = QueryEvaluator::processQueryTree(qT);
		ResultProjector::projectResultToList(queryResults, results);
	} catch (exception e) {
		#ifdef DEBUG
			results.push_front(e.what());
		#endif
	}
}