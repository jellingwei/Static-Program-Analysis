#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <deque>

#include "PKB.h"
#include "NextTable.h"

using namespace std;


NextTable::NextTable() {
	
	
}

vector<int> NextTable::getNextAfter(int progLine1, bool transitiveClosure) {
	PKB pkb = PKB::getInstance();
	if (pkb.cfgNodeTable.count(progLine1) == 0) { // progline does not have a cfg node
		return vector<int>();
	}
	CNode* curNode = pkb.cfgNodeTable.at(progLine1);
	vector<int> result;

	deque<CNode*> frontier; 
	set<int> visited;

	frontier.push_back(curNode);

	while (!frontier.empty()) {
		curNode = frontier.back(); frontier.pop_back();

		vector<CNode*>* nextNodes = curNode->getAfter();
		for (auto iter = nextNodes->begin(); iter != nextNodes->end(); ++iter) {
			if ((*iter)->getNodeType() == Proc_C || (*iter)->getNodeType() == EndProc_C) {
				continue;
			}

			//@todo clean up tmr
			// if the previous node is a dummy end_if node,
			// skip it and go to its "after" nodes directly
			if ((*iter)->getNodeType() == EndIf_C) {
				assert((*iter)->getAfter()->size() == 1 || (*iter)->getAfter()->size() == 0);
				if ((*iter)->getAfter()->size() == 0) { // if is the last stmt in the stmtlist
					continue;
				}
				CNode* nextNodeAfterEndIf = (*iter)->getAfter()->at(0);

				if (visited.count(nextNodeAfterEndIf->getProcLineNumber()) == 0) {
					frontier.push_back(nextNodeAfterEndIf);
					if (nextNodeAfterEndIf->getNodeType() != EndIf_C){
						result.push_back(nextNodeAfterEndIf->getProcLineNumber());
					}
					visited.insert(nextNodeAfterEndIf->getProcLineNumber());
				}

			} else if (visited.count((*iter)->getProcLineNumber()) == 0) {
				frontier.push_back(*iter);
				if ((*iter)->getNodeType() != EndIf_C){
					result.push_back((*iter)->getProcLineNumber());
				}
				visited.insert((*iter)->getProcLineNumber());
			}
		}

		// break after the first iteration if we are not finding transitive closure
		if (!transitiveClosure) {
			break;
		}
	}

	return result;
}

vector<int> NextTable::getNextBefore(int progLine2, bool transitiveClosure) {
	PKB pkb = PKB::getInstance();
	if (pkb.cfgNodeTable.count(progLine2) == 0) { // progline does not have a cfg node
		return vector<int>();
	}
	CNode* curNode = pkb.cfgNodeTable.at(progLine2);
	vector<int> result;

	deque<CNode*> frontier; 
	set<int> visited;
	frontier.push_back(curNode);

	while (!frontier.empty()) {
		curNode = frontier.back(); frontier.pop_back();

		vector<CNode*>* nextNodes = curNode->getBefore();
		for (auto iter = nextNodes->begin(); iter != nextNodes->end(); ++iter) {
			if ((*iter)->getNodeType() == Proc_C || (*iter)->getNodeType() == EndProc_C) {
				continue;
			}

			// if the previous node is a dummy end_if node,
			// skip it and go to its "before" nodes directly
			if ((*iter)->getNodeType() == EndIf_C) {
				assert((*iter)->getBefore()->size() == 2);
				CNode* nextNodeAfterEndIf = (*iter)->getBefore()->at(0);
				CNode* nextNodeAfterEndIf1 = (*iter)->getBefore()->at(1);

				if (visited.count(nextNodeAfterEndIf->getProcLineNumber()) == 0) {
					frontier.push_back(nextNodeAfterEndIf);
				
					if (nextNodeAfterEndIf->getNodeType() != EndIf_C){
						result.push_back(nextNodeAfterEndIf->getProcLineNumber());
					}
				
					visited.insert(nextNodeAfterEndIf->getProcLineNumber());
				} 
				if (visited.count(nextNodeAfterEndIf1->getProcLineNumber()) == 0) {
					frontier.push_back(nextNodeAfterEndIf1);
				
					if (nextNodeAfterEndIf1->getNodeType() != EndIf_C){
						result.push_back(nextNodeAfterEndIf1->getProcLineNumber());
					}
				
					visited.insert(nextNodeAfterEndIf1->getProcLineNumber());
				} 


			} else if (visited.count((*iter)->getProcLineNumber()) == 0) {
				frontier.push_back(*iter);
				if ((*iter)->getNodeType() != EndIf_C){
					result.push_back((*iter)->getProcLineNumber());
				}
				visited.insert((*iter)->getProcLineNumber());
			}
		}

		// break after the first iteration if we are not finding transitive closure
		if (!transitiveClosure) {
			break;
		}
	}

	return result;
}

bool NextTable::isNext(int progLine1, int progLine2, bool transitiveClosure) {
	PKB pkb = PKB::getInstance();
	
	if (pkb.cfgNodeTable.count(progLine1) == 0) { // progline does not have a cfg node
		return false;
	}
	CNode* curNode = pkb.cfgNodeTable.at(progLine1);
	
	vector<int> result;

	deque<CNode*> frontier; 
	set<int> visited;
	frontier.push_back(curNode);
	
	while (!frontier.empty()) {
		curNode = frontier.back(); frontier.pop_back();

		vector<CNode*>* nextNodes = curNode->getAfter();
		
		for (auto iter = nextNodes->begin(); iter != nextNodes->end(); ++iter) {
			CNode* node = *iter;
			if ((*iter)->getNodeType() == EndIf_C) {
				assert((*iter)->getAfter()->size() == 1 || (*iter)->getAfter()->size() == 0);
				if ((*iter)->getAfter()->size() == 0) { // if is the last stmt in the stmtlist
					continue;
				}
				CNode* nextNodeAfterEndIf = (*iter)->getAfter()->at(0);

				if (visited.count(nextNodeAfterEndIf->getProcLineNumber()) == 0) {
					frontier.push_back(nextNodeAfterEndIf);
					if (nextNodeAfterEndIf->getNodeType() != EndIf_C){
						result.push_back(nextNodeAfterEndIf->getProcLineNumber());
					}
					visited.insert(nextNodeAfterEndIf->getProcLineNumber());
				}

			} else if (visited.count(node->getProcLineNumber()) == 0 ) {
				frontier.push_back(node);
				if ((*iter)->getNodeType() != EndIf_C){
					result.push_back((*iter)->getProcLineNumber());
				}
				visited.insert(node->getProcLineNumber());
		
			}

			if (node->getProcLineNumber() == progLine2) {
				return true;
			}
		}

		// break after the first iteration if we are not finding transitive closure
		if (!transitiveClosure) {
			break;
		}
	}
	

	return false;
}