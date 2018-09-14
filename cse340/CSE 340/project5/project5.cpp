extern "C" {
	#include "compiler.h"
}
#include <string>
#include "string.h"
#include "project5.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h> 
#include <vector>
#include <algorithm>

using namespace std;

// Declare globals
vector< ValueNode*> dict;

struct StatementNode* parse_generate_intermediate_representation(){
	// var_section
	parse_var_section();

	// body
	struct StatementNode* head = parse_body();
	return head;
}

void parse_var_section(){
	parse_id_list();
	ttype = getToken();
	if (ttype == SEMICOLON){
		// end of var_section
	}
	else{
		// syntax error
	}
}

void parse_id_list(){
	ttype = getToken();
	if (ttype == ID){
		struct ValueNode* temp_value_node = ALLOC(struct ValueNode);
		temp_value_node->name = strdup(token);
		temp_value_node->value = NULL;
		dict.push_back(temp_value_node);
		ttype = getToken();
		if (ttype == COMMA){
			parse_id_list();
		}
		else if (ttype == SEMICOLON){
			ungetToken();
		}
		else{
			// syntax error
		}
	}
	else{
		// syntax error
	}
}

struct StatementNode* parse_body(){
	struct StatementNode* stmt_list;
	ttype = getToken();
	if (ttype == LBRACE){
		stmt_list = parse_stmt_list();
		ttype = getToken();
		if (ttype == RBRACE){
			return stmt_list;
		}
		else{
			// syntax error
		}
	}
	else{
		// syntax error
	}
	return stmt_list;
}

struct StatementNode* parse_stmt_list(){
	struct StatementNode* stmt;
	struct StatementNode* stmt_list = NULL;

	stmt = parse_stmt();
	ttype = getToken();
	if (ttype == ID || ttype == IF || ttype == PRINT || ttype == SWITCH || ttype == WHILE){
		ungetToken();
		stmt_list = parse_stmt_list();

		if (stmt->type == IF_STMT){
			stmt->next->next = stmt_list;
			//cout << stmt->next->type << " " << stmt->next->next->type << endl;
		}
		else{
			stmt->next = stmt_list;
		}
		return stmt;

	}
	else{
		ungetToken();
		return stmt;
	}
}

struct StatementNode* parse_stmt(){
	struct StatementNode* stmt = ALLOC(struct StatementNode);
	stmt->next = NULL;
	ttype = getToken();

	if (ttype == ID){
		// Assignment statement
		ungetToken();
		stmt->type = ASSIGN_STMT;
		struct AssignmentStatement* assign_stm = parse_assign_stmt();
		stmt->assign_stmt = assign_stm;
	}
	else if(ttype == PRINT){
		ungetToken();
		stmt->type = PRINT_STMT;
		struct PrintStatement* print_stm = parse_print_stmt();
		stmt->print_stmt = print_stm;
	}
	else if (ttype == IF){
		stmt->type = IF_STMT;
		struct IfStatement* if_stm = ALLOC(struct IfStatement);
		parse_condition(if_stm);
		if_stm->true_branch = parse_body();
		stmt->if_stmt = if_stm;
		struct StatementNode* end_of_body = get_last_statement_node(if_stm->true_branch);
		struct StatementNode* noop_node = ALLOC(struct StatementNode);
		noop_node->type = NOOP_STMT;
		noop_node->next = NULL;
		end_of_body->next = noop_node;

		if_stm->false_branch = noop_node;
		stmt->next = noop_node;
	}
	else if (ttype == WHILE){
		stmt->type = IF_STMT;
		struct IfStatement* while_stmt = ALLOC(struct IfStatement);
		parse_condition(while_stmt);
		while_stmt->true_branch = parse_body();
		stmt->if_stmt = while_stmt;

		struct StatementNode* gt = ALLOC(struct StatementNode);
		gt->type = GOTO_STMT;
		struct GotoStatement* gt_node = ALLOC(struct GotoStatement);
		gt->goto_stmt = gt_node;
		gt_node->target = stmt;

		struct StatementNode* end_of_body = get_last_statement_node(while_stmt->true_branch);
		end_of_body->next = gt;
		struct StatementNode* noop_node = ALLOC(struct StatementNode);
		noop_node->type = NOOP_STMT;
		noop_node->next = NULL;

		while_stmt->false_branch = noop_node;
		stmt->next = noop_node;

	}
	else if (ttype == SWITCH){
		// get var
		ttype = getToken();
		if (ttype == ID){
			ungetToken();
			stmt = parse_switch();
			//cout << "stmt->next->type: " << stmt->next->type << endl;
		}
		else{
			cout << "ERROR" << endl;
		}

	}
	else{
		// TODO: do more else-if and else
	}

	return stmt;
}

struct StatementNode* parse_switch(){
	struct StatementNode* case_list;

	// val stores the SWITCH value. Compared with each case until match found.
	struct ValueNode* val = parse_id();

	// If false_branch taken, next value of last StatementNode in false_branch set to this noop_node.
	struct StatementNode* noop_node = ALLOC(struct StatementNode);
	noop_node->type = NOOP_STMT;
	noop_node->next = NULL;

	ttype = getToken();
	if (ttype == LBRACE){
		case_list = parse_case_list(val, noop_node);
		ttype = getToken();
		if (ttype == RBRACE){
			case_list->next = noop_node;
			// good. end of switch statement.
		}
		else{
			// syntax error;
			// Assuming we handle default cases before this point
		}
	}
	else{
		// syntax error
	}
	return case_list;
}

struct StatementNode* parse_case_list(struct ValueNode* val, struct StatementNode* noop_node){
	struct StatementNode* cas;
	struct StatementNode* case_list;

	cas = parse_case(val);

	ttype = getToken();
	if (ttype == CASE || ttype == DEFAULT){
		ungetToken();
		// get next case recursively
		case_list = parse_case_list(val, noop_node);
		// If SWITCH val NOT EQUAL case num, go to next case.
		cas->if_stmt->true_branch = case_list;
		// Set next field of last StatementNode in case body to the noop node
		
	}
	else{
		ungetToken();
		cas->if_stmt->true_branch = noop_node;
	}
	struct StatementNode* end_of_body = get_last_statement_node(cas->if_stmt->false_branch);
	end_of_body->next = noop_node;
	cas->next = noop_node;
	return cas;
}

struct StatementNode* parse_case(struct ValueNode* val){
	struct StatementNode* cas = ALLOC(struct StatementNode);
	struct IfStatement* if_stmt = ALLOC(IfStatement);
	cas->type = IF_STMT;
	cas->if_stmt = if_stmt;
	cas->next = NULL; // TODO ?

	ttype = getToken();
	if (ttype == CASE){
		ttype = getToken();
		if (ttype == NUM){
			if_stmt->condition_operand1 = num2ValNode();
			if_stmt->condition_operand2 = val;
			if_stmt->condition_op = NOTEQUAL;
			ttype = getToken();
			if (ttype == COLON){
				if_stmt->false_branch = parse_body();
			}
			else{
				// syntax error
			}
		}
		else{
			// syntax error
		}

	}
	else if (ttype == DEFAULT){

		if_stmt->condition_operand1 = val;
		if_stmt->condition_operand2 = val;
		if_stmt->condition_op = NOTEQUAL;

		ttype = getToken();
		if (ttype == COLON){
			if_stmt->false_branch = parse_body();
			if_stmt->true_branch = if_stmt->false_branch;
		}
		else{
			// syntax error
		}
	}
	else{
		// syntax error
	}

	return cas;
}

struct ValueNode* num2ValNode(){
	struct ValueNode* vn = ALLOC(struct ValueNode);
	vn->name = NULL;
	vn->value = atoi(token);
	return vn;
}

void parse_condition(struct IfStatement* condition){
	condition->condition_operand1 = parse_primary();
	ttype = getToken();
	if (ttype == GREATER || ttype == LESS || ttype == NOTEQUAL){
		condition->condition_op = ttype;
		condition->condition_operand2 = parse_primary();
	}
	else{
		// syntax error
	}
}

struct AssignmentStatement* parse_assign_stmt(){
	struct AssignmentStatement* assign_stmt = ALLOC(struct AssignmentStatement);
	assign_stmt->left_hand_side = parse_id();
	ttype = getToken();
	if (ttype == EQUAL){
		ttype = getToken();
		if (ttype == ID || ttype == NUM){
			ungetToken();
			assign_stmt->operand1 = parse_primary();
			ttype = getToken();
			if (ttype == PLUS || ttype == MINUS || ttype == MULT || ttype == DIV){
				assign_stmt->op = ttype;
				assign_stmt->operand2 = parse_primary();

			}
			else{
				ungetToken();
				assign_stmt->op = NULL;
				assign_stmt->operand2 = NULL;
			}
			ttype = getToken();
			if (ttype == SEMICOLON){
				// good. end of assignment statement
			}
			else{
				// syntax error
			}
		}
		else {
			// syntax error
		}
	}
	else{
		// syntax error
	}
	return assign_stmt;
}
class MatchesName
{
    std::string _name;

	public:
	    MatchesName(const std::string &name) : _name(name) {}

	    bool operator()(ValueNode* &item) const
	    {
	        return item->name == _name;
	    }
};
struct ValueNode* parse_id(){

	ttype = getToken();
	if (ttype == ID){
		vector<ValueNode*>::iterator it;

		it = find_if(dict.begin(), dict.end(), MatchesName(string(token)));
	    if (it != (dict.end())){
	    	return *it;
	    }
	}
	else{
		// syntax error
	}
	return NULL;
}

struct ValueNode* parse_primary(){
	struct ValueNode* primary;
	ttype = getToken();
	if (ttype == ID){
		ungetToken();
		primary = parse_id();
	}
	else if (ttype == NUM){
		primary = ALLOC(struct ValueNode);
		primary->name = NULL;
		primary->value = atoi(token);
	}
	else{
		// syntax error
	}
	return primary;
}

struct PrintStatement* parse_print_stmt(){
	struct PrintStatement* print_stm = ALLOC(struct PrintStatement);
	ttype = getToken();
	if (ttype == PRINT){
		ttype = getToken();
		if (ttype == ID){
			ungetToken();
			print_stm->id = parse_id();
			ttype = getToken();
			if(ttype == SEMICOLON){
				// good. print statement complete
			}
			else{
				// syntax error
			}
		}
		else{
			// syntax error
		}
	}
	else{
		// syntax error
	}
	return print_stm;
}

struct StatementNode* get_last_statement_node(struct StatementNode* stmt){
	while (stmt->next != NULL){
		stmt = stmt->next;
	}
	return stmt;
}