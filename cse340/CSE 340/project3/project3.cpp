#include <algorithm>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <set>
#include <vector>
extern "C" {
	#include "lexer.h"
}

using namespace std;

struct symbol{
	string name;
	bool isTerminal;
};

struct nonterminal{
	string name;
	int ruleCount;
};

// Forward declarations
bool isTerm(vector<nonterminal>*, string, vector<string>*, bool);
bool getFirstSets(unordered_map<string, set<string> >* firsts, vector<vector<symbol> >*);
bool getFirst(unordered_map<string, set<string> >::iterator it, unordered_map<string, set<string> >* firsts, vector<vector<symbol> >*);
bool getFollowSets(unordered_map<string, set<string> >* follows, unordered_map<string, set<string> >* firsts, vector<vector<symbol> >* grammar);
bool getFollow(unordered_map<string, set<string> >::iterator it, unordered_map<string, set<string> >* follows,unordered_map<string, set<string> >* firsts, vector<vector<symbol> >* grammar);
void followSymbolFound(unordered_map<string, set<string> >::iterator it, unordered_map<string, set<string> >* follows, unordered_map<string, set<string> >* firsts, vector<vector<symbol> >* grammar, int i, int j);

int main (int argc, char* argv[])
{
    int task;
    vector<nonterminal> ntv;
    vector<vector<symbol> > grammar;

    vector<string> tv; // vector of terminals

    // dictionary for first sets
    unordered_map<string, set<string> > firsts;

    // dictionary for follow sets
    unordered_map<string, set<string> > follows;

    if (argc < 2)
    {
        printf("Error: missing argument\n");
        return 1;
    }

    /*
       Note that by convention argv[0] is the name of your executable,
       and the first argument to your program is stored in argv[1]
     */

    task = atoi(argv[1]);


    // TODO: Read the input grammar at this point

    // Read non-terminals
    while(1){
    	getToken();
    	// TODO: Maybe include DOUBLEHASH for test cases
    	if (t_type == HASH){
    		break;
    	}
    	struct nonterminal temp;
    	temp.name = current_token;
    	temp.ruleCount = 0;
    	ntv.push_back(temp);
    	set<string> tempset ;
    	firsts.insert({{current_token, tempset}});
    	follows.insert({{current_token, tempset}});
    }
    
    vector<symbol> rule;
    bool isFirst = true;
    while(1){
    	getToken();
    	if (t_type == DOUBLEHASH){
    		break;
    	}
    	else if (t_type == HASH){
    		if (rule.size() == 1){
    			struct symbol temp;
    			temp.name = "#";
    			temp.isTerminal = isTerm(&ntv, "#", &tv, isFirst);
    			rule.push_back(temp);
    		}
    		grammar.push_back(rule);
    		rule.clear();
    		isFirst = true;
    	}
    	else if (t_type == ID){
    		struct symbol temp;
    		temp.name = current_token;
    		temp.isTerminal = isTerm(&ntv, current_token, &tv, isFirst);
    		rule.push_back(temp);
    	}
    	else if (t_type == ARROW){
    		isFirst = false;
    	}
    	else{
    		continue;
    	}
    }



    /*
       Hint: You can modify and use the lexer from previous project
       to read the input. Note that there are only 4 token types needed
       for reading the input in this project.

       WARNING: You will need to modify lexer.c and lexer.h to support
                the project 3 input language.
     */

    switch (task) {
        case 0:
            // TODO: Output information about the input grammar
            // Print terminals in order they appeared
		    for(unsigned int i = 0; i < tv.size(); i++){
		    	cout << tv[i];
		    }
		    cout << endl;
		    // Print rules
		    for(unsigned int i = 0; i < ntv.size(); i++){
		    	cout << ntv[i].name << ": " << ntv[i].ruleCount << endl;
		    }
		    // for(unsigned int i = 0; i < grammar.size(); i++){
		    // 	for(unsigned int j = 0; j < grammar[i].size(); j++){
		    // 		cout << grammar[i][j].name;
		    // 		if (j==0)
		    // 			cout << " -> ";
		    // 	}
		    // 	cout << endl;
		    // }
            break;

        case 1:
        	{            // TODO: Calculate FIRST sets for the input grammar
            bool changed = true;
            // print out all items in each set
            // for (auto it = firsts.begin(); it != firsts.end(); ++it){
            // 	for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++){
            // 		cout << (*it2);
            // 	}
            // }
            while (changed){
            	// Loop until getFirstSets returns false, indicating that first sets were calculated for every NT and no new changes are occurring when running FIRST on the pre-loaded sets.
            	changed = getFirstSets(&firsts, &grammar);
            }
            for (auto it = ntv.begin(); it != ntv.end(); it++){
            	set<string> temp_set = firsts.at((*it).name);
            	cout << "FIRST(" << (*it).name << ") = { ";
            	if (temp_set.size() > 0){
            	    set<string>::iterator it2 = temp_set.begin();
            		cout << *it2;
            		it2++;
            		for (;it2 != temp_set.end(); it2++){
            			cout << ", " << (*it2);
            		}
          		}
 
            	cout << " }" << endl;
            }

            break;}


case 2:
        {            // TODO: Calculate FIRST sets for the input grammar
            bool changed = true;

            // print out all items in each set
            // for (auto it = firsts.begin(); it != firsts.end(); ++it){
            // 	for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++){
            // 		cout << (*it2);
            // 	}u
            // }
            while (changed){
            	// Loop until getFirstSets returns false, indicating that first sets were calculated for every NT and no new changes are occurring when running FIRST on the pre-loaded sets.
            	changed = getFirstSets(&firsts, &grammar);
            }

            // Get follow sets
            bool follow_changed = true;
            auto it = ntv.begin();
            follows[(*it).name].insert("$");
            while (follow_changed){
            	follow_changed = getFollowSets(&follows, &firsts, &grammar);
            }


            // Print FOLLOW sets
            for (auto it = ntv.begin(); it != ntv.end(); it++){
            	string temp_name =  (*it).name;
            	set<string> temp_set = follows.at((*it).name);
            	cout << "FOLLOW(" << (*it).name << ") = { ";
            	if (temp_set.size() > 0){
            	    set<string>::iterator it2 = temp_set.begin();
            		cout << *it2;
            		it2++;
            		for (;it2 != temp_set.end(); it2++){
            			cout << ", " << (*it2);
            		}
          		}
 
            	cout << " }" << endl;
            }
            // TODO: Output the FIRST sets in the exact order and format required
       		break;
   		}

        default:
            printf("Error: unrecognized task number %d\n", task);
            break;
    }
    return 0;
}

bool getFirstSets(unordered_map<string, set<string> >* firsts, vector<vector<symbol> >* grammar){
	bool changed = false;
	unordered_map<string, set<string> >::iterator it;
	// For every pre-loaded FIRST set
	for (it = (*firsts).begin(); it != (*firsts).end(); it++){
		// run getFirst on set and determine if a change was made.
		bool changed_temp = getFirst(it, firsts, grammar);

		// If ANY of the sets are changed, set changed to true. This guarantees that the while loop that called getFirstSets will loop at least one more time.
		if (!changed && changed_temp)
			changed = true;
	}
	return changed;
}

bool getFirst(unordered_map<string, set<string> >::iterator it, unordered_map<string, set<string> >* firsts, vector<vector<symbol> >* grammar){
	set<string> original = (*it).second;
	string first_set = (*it).first;
	for(unsigned int i = 0; i < (*grammar).size(); i++){
		// find first symbols for each rule
		struct symbol lhs = (*grammar)[i][0];
		if (first_set.compare(lhs.name) == 0){
			// For each rule for "i"
			int j = 1;
			int ruleLength = (*grammar)[i].size();
			bool noEpsilonFound = false;
			while(!noEpsilonFound){
				struct symbol current = (*grammar)[i][j];

				// If a terminal or epsilon, insert into set (and mark noEpsilonFound as true to prevent next symbol in rule from being looked at)
				if (current.isTerminal || current.name.compare("#") == 0){
					(*it).second.insert(current.name);
					noEpsilonFound = true;
					break;
				}

				// Otherwise, not a terminal
				else if (!current.isTerminal){
					// Make a copy of the set, erase the epsilon, and add the set to FIRST set of non-terminal
					set<string> temp_set = (*firsts).at(current.name);
					bool epsilonFound = false;

					// for each terminal or epsilon in pre-loaded FIRST set of current RHS non-terminal, check to see if epsilon.
					for (set<string>::iterator it2=temp_set.begin(); it2!=temp_set.end(); ++it2){
						if ((*it2).compare("#") == 0){
							// remove epsilon
							temp_set.erase("#");
							epsilonFound = true;
							break;
						}
					}
					if (!epsilonFound){
						// epsilon not found, so set noEpsilonFound to true to prevent next symbol for being looked at
						noEpsilonFound = true;
					}
					for (set<string>::iterator it2=temp_set.begin(); it2!=temp_set.end(); ++it2){
						// adding temp_set to FIRST set
						(*it).second.insert(*it2);
					}
				}
				j++;
				if (j == ruleLength){
					break;
				}
			}
			if (noEpsilonFound == false){
				// every symbol in RHS can be epsilon, so insert epsilon into the FIRST set.
				(*it).second.insert("#");
			}
		}

    }
    // if original set matches current set, no changes were made. Return false to indicate the FIRST sets are completed, else return TRUE to keep looping
	return (original == (*it).second) ? false : true;
}

bool getFollowSets(unordered_map<string, set<string> >* follows, unordered_map<string, set<string> >* firsts, vector<vector<symbol> >* grammar){
	bool changed = false;
	unordered_map<string, set<string> >::iterator it;
	// For every pre-loaded FOLLOW set
	for (it = (*follows).begin(); it != (*follows).end(); it++){
		// run getFirst on set and determine if a change was made.
		bool changed_temp = getFollow(it, follows, firsts, grammar);

		// If ANY of the sets are changed, set changed to true. This guarantees that the while loop that called getFirstSets will loop at least one more time.
		if (!changed && changed_temp)
			changed = true;
	}
	return changed;
}

bool getFollow(unordered_map<string, set<string> >::iterator it, unordered_map<string, set<string> >* follows, unordered_map<string, set<string> >* firsts, vector<vector<symbol> >* grammar){
	string current_follow_set_key = (*it).first;
	set<string> original = (*it).second;

	for (unsigned int i = 0; i < (*grammar).size(); i++){
		string lhs = (*grammar)[i][0].name;
		int symbolCount = (*grammar)[i].size();

		for (int j = 1; j < symbolCount; j++){
			if (current_follow_set_key == (*grammar)[i][j].name)
				followSymbolFound(it, follows, firsts, grammar, i, j);
		}
	}
	
	return (original == (*it).second) ? false : true;
}

void followSymbolFound(unordered_map<string, set<string> >::iterator it, unordered_map<string, set<string> >* follows, unordered_map<string, set<string> >* firsts, vector<vector<symbol> >* grammar, int i, int j){
	string current_follow_set_key = (*it).first;
	string lhs = (*grammar)[i][0].name;
	int symbolCount = (*grammar)[i].size();

	// RULE 2: If symbol match is last symbol in rule, add follow set of LHS to current follow set.
	if (j == symbolCount - 1){
		set<string> lhs_follow_set = (*follows).at(lhs);
		for (auto it2 = lhs_follow_set.begin(); it2 != lhs_follow_set.end(); it2++){
			(*it).second.insert(*it2);
		}
	}
	else{
		int k = j + 1;
		for (; k < symbolCount; k++){
			bool isLastSymbol = false;
			bool hasEpsilon = false;

			// RULE 4 (Add FIRST(following symbol) - epsilon to current follow set)
			// If isTerminal, then FIRST(following symbol) = following symbol. Add to follow set.
			if ((*grammar)[i][k].isTerminal){
				(*it).second.insert((*grammar)[i][k].name);
				break;
			}

			string cursorSymbol = (*grammar)[i][k].name;
			// RULE 4 (following symbol in NT)
			if (k == symbolCount - 1)
				isLastSymbol = true;

			set<string> cursorSymbolFirstSet = (*firsts).at(cursorSymbol);
			for (set<string>::iterator it2=cursorSymbolFirstSet.begin(); it2!=cursorSymbolFirstSet.end(); it2++){
				if ((*it2).compare("#") == 0){
					hasEpsilon = true;
				}
				else{
					(*it).second.insert(*it2);
				}
			}

			if (!hasEpsilon)
				break;

			// RULE 5 - if code hasn't broken by this point, the for loop will iterate to the next symbol, fulfilling this rule.

			// RULE 3 - If the cursorSymbol has epsilon and is the last symbol in the rule, then all the symbols following the "current symbol" in the rule contain epsilon.
			//          So add follow set of LHS to follow set of "current symbol"
			if (isLastSymbol && hasEpsilon){
				set<string> lhs_follow_set = (*follows).at(lhs);
				for (auto it2 = lhs_follow_set.begin(); it2 != lhs_follow_set.end(); it2++){
					(*it).second.insert(*it2);
				}
			}

		}
	}
}

class MatchesName
{
    std::string _name;

	public:
	    MatchesName(const std::string &name) : _name(name) {}

	    bool operator()(const nonterminal &item) const
	    {
	        return item.name == _name;
	    }
};
bool isTerm(vector<nonterminal>* nonterminals, string symbol, vector<string>* terminals, bool isFirst){
	bool isATerminal;
	bool isEpsilon = (symbol.compare("#") == 0) ? true : false;
    auto result = find_if((*nonterminals).begin(), (*nonterminals).end(), MatchesName(symbol));
    if (result != ((*nonterminals).end())){
    	isATerminal = false;
    	if (isFirst)
    		result -> ruleCount++;
    }
    else{
    	if (!isEpsilon)
    		isATerminal = true;
    	vector<string>::iterator it;
    	it = find((*terminals).begin(), (*terminals).end(), symbol);
    	bool isInTerminals = (it != (*terminals).end()) ? true : false;
    	if (!isInTerminals && !isEpsilon){
    		(*terminals).push_back(symbol);
    	}
    }
    return isATerminal;
}