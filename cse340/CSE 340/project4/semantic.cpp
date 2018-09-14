/*----------------------------------------------------------------------------
 Note: the code in this file is not to be shared with anyone or posted online.
 (c) Rida Bazzi, 2015, Adam Doupe, 2015
 ----------------------------------------------------------------------------*/

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "syntax.h"
#include <string>
#include <unordered_map>
#include <set>
#include <vector>

/* ------------------------------------------------------- */
/* -------------------- LEXER SECTION -------------------- */
/* ------------------------------------------------------- */

#define KEYWORDS  11
using namespace std;
//void populateTypeDictionary(unordered_map<string, type_nameNode* > dict, programNode* parseTree);
void parse_Body(unordered_map<string, type_nameNode* >* varDict, unordered_map<string, type_nameNode* >* typeDict, unordered_map<string, bool>* idTypeDict, bodyNode* body);
void parse_While(unordered_map<string, type_nameNode* >* varDict, unordered_map<string, type_nameNode* >* typeDict, unordered_map<string, bool>* idTypeDict, while_stmtNode* while_stmt);
void parse_Do(unordered_map<string, type_nameNode* >* varDict, unordered_map<string, type_nameNode* >* typeDict, unordered_map<string, bool>* idTypeDict, while_stmtNode* do_stmt);
void parse_Condition(unordered_map<string, type_nameNode* >* varDict, unordered_map<string, type_nameNode* >* typeDict, unordered_map<string, bool>* idTypeDict, conditionNode* condition);
void parse_Assign(unordered_map<string, type_nameNode* >* varDict, unordered_map<string, type_nameNode* >* typeDict, unordered_map<string, bool>* idTypeDict, assign_stmtNode* assign_stmt);
void type_mismatch_error(string constraint);
type_nameNode* parse_Primary(unordered_map<string, type_nameNode* >* varDict, unordered_map<string, type_nameNode* >* typeDict, unordered_map<string, bool>* idTypeDict, primaryNode* primary);
type_nameNode* compareTypeNames(unordered_map<string, type_nameNode* >* varDict, unordered_map<string, type_nameNode* >* typeDict, unordered_map<string, bool>* idTypeDict, type_nameNode* t1, type_nameNode* t2, string err);
void parse_Switch(unordered_map<string, type_nameNode* >* varDict, unordered_map<string, type_nameNode* >* typeDict, unordered_map<string, bool>* idTypeDict, switch_stmtNode* switch_stmt);
void parse_CaseList(unordered_map<string, type_nameNode* >* varDict, unordered_map<string, type_nameNode* >* typeDict, unordered_map<string, bool>* idTypeDict, case_listNode* case_list);
void printEverything(unordered_map<string, type_nameNode* >* varDict, unordered_map<string, type_nameNode* >* typeDict, unordered_map<string, bool>* idTypeDict);
typedef enum
{
    END_OF_FILE = -1, VAR = 1, WHILE, INT, REAL, STRING, BOOLEAN,
    TYPE, LONG, DO, CASE, SWITCH,
    PLUS, MINUS, DIV, MULT, EQUAL,
    COLON, COMMA, SEMICOLON,
    LBRAC, RBRAC, LPAREN, RPAREN, LBRACE, RBRACE,
    NOTEQUAL, GREATER, LESS, LTEQ, GTEQ, DOT,
    ID, NUM, REALNUM,
    ERROR
} token_type;

const char *reserved[] = {"",
    "VAR", "WHILE", "INT", "REAL", "STRING", "BOOLEAN",
    "TYPE", "LONG", "DO", "CASE", "SWITCH",
    "+", "-", "/", "*", "=",
    ":", ",", ";",
    "[", "]", "(", ")", "{", "}",
    "<>", ">", "<", "<=", ">=", ".",
    "ID", "NUM", "REALNUM",
    "ERROR"
};

// Global Variables associated with the next input token
#define MAX_TOKEN_LENGTH 100
char token[MAX_TOKEN_LENGTH]; // token string
token_type t_type; // token type
bool activeToken = false;
int tokenLength;
int line_no = 1;
int line_num = 0;
int imp_type_num = 100;
int current_Switch_id = -1;

vector<string> vals;

void skipSpace()
{
    char c;
    
    c = getchar();
    line_no += (c == '\n');
    while (!feof(stdin) && isspace(c))
    {
        c = getchar();
        line_no += (c == '\n');
    }
    ungetc(c, stdin);
}

int isKeyword(char *s)
{
    int i;
    
    for (i = 1; i <= KEYWORDS; i++)
    {
        if (strcmp(reserved[i], s) == 0)
        {
            return i;
        }
    }
    return false;
}

/*
 * ungetToken() simply sets a flag so that when getToken() is called
 * the old t_type is returned and the old token is not overwritten.
 * NOTE: BETWEEN ANY TWO SEPARATE CALLS TO ungetToken() THERE MUST BE
 * AT LEAST ONE CALL TO getToken(). CALLING TWO ungetToken() WILL NOT
 * UNGET TWO TOKENS
 */
void ungetToken()
{
    activeToken = true;
}

token_type scan_number()
{
    char c;
    
    c = getchar();
    if (isdigit(c))
    {
        // First collect leading digits before dot
        // 0 is a NUM by itself
        if (c == '0')
        {
            token[tokenLength] = c;
            tokenLength++;
            token[tokenLength] = '\0';
        }
        else
        {
            while (isdigit(c))
            {
                token[tokenLength] = c;
                tokenLength++;
                c = getchar();
            }
            ungetc(c, stdin);
            token[tokenLength] = '\0';
        }
        // Check if leading digits are integer part of a REALNUM
        c = getchar();
        if (c == '.')
        {
            c = getchar();
            if (isdigit(c))
            {
                token[tokenLength] = '.';
                tokenLength++;
                while (isdigit(c))
                {
                    token[tokenLength] = c;
                    tokenLength++;
                    c = getchar();
                }
                token[tokenLength] = '\0';
                if (!feof(stdin))
                {
                    ungetc(c, stdin);
                }
                return REALNUM;
            }
            else
            {
                ungetc(c, stdin);
                c = '.';
                ungetc(c, stdin);
                return NUM;
            }
        }
        else
        {
            ungetc(c, stdin);
            return NUM;
        }
    }
    else
    {
        return ERROR;
    }
}

token_type scan_id_or_keyword()
{
    token_type the_type;
    int k;
    char c;
    
    c = getchar();
    if (isalpha(c))
    {
        while (isalnum(c))
        {
            token[tokenLength] = c;
            tokenLength++;
            c = getchar();
        }
        if (!feof(stdin))
        {
            ungetc(c, stdin);
        }
        token[tokenLength] = '\0';
        k = isKeyword(token);
        if (k == 0)
        {
            the_type = ID;
        }
        else
        {
            the_type = (token_type) k;
        }
        return the_type;
    }
    else
    {
        return ERROR;
    }
}

token_type getToken()
{
    char c;
    
    if (activeToken)
    {
        activeToken = false;
        return t_type;
    }
    skipSpace();
    tokenLength = 0;
    c = getchar();
    switch (c)
    {
        case '.': return DOT;
        case '+': return PLUS;
        case '-': return MINUS;
        case '/': return DIV;
        case '*': return MULT;
        case '=': return EQUAL;
        case ':': return COLON;
        case ',': return COMMA;
        case ';': return SEMICOLON;
        case '[': return LBRAC;
        case ']': return RBRAC;
        case '(': return LPAREN;
        case ')': return RPAREN;
        case '{': return LBRACE;
        case '}': return RBRACE;
        case '<':
            c = getchar();
            if (c == '=')
            {
                return LTEQ;
            }
            else if (c == '>')
            {
                return NOTEQUAL;
            }
            else
            {
                ungetc(c, stdin);
                return LESS;
            }
        case '>':
            c = getchar();
            if (c == '=')
            {
                return GTEQ;
            }
            else
            {
                ungetc(c, stdin);
                return GREATER;
            }
        default:
            if (isdigit(c))
            {
                ungetc(c, stdin);
                return scan_number();
            }
            else if (isalpha(c))
            {
                ungetc(c, stdin);
                return scan_id_or_keyword();
            }
            else if (c == EOF)
            {
                return END_OF_FILE;
            }
            else
            {
                return ERROR;
            }
    }
}

/* ----------------------------------------------------------------- */
/* -------------------- SYNTAX ANALYSIS SECTION -------------------- */
/* ----------------------------------------------------------------- */

void syntax_error(const char* msg)
{
    printf("Syntax error while parsing %s line %d\n", msg, line_no);
    exit(1);
}

/* -------------------- PRINTING PARSE TREE -------------------- */
void print_parse_tree(struct programNode* program)
{
    print_decl(program->decl);
    print_body(program->body);
}

void print_decl(struct declNode* dec)
{
    if (dec->type_decl_section != NULL)
    {
        print_type_decl_section(dec->type_decl_section);
    }
    if (dec->var_decl_section != NULL)
    {
        print_var_decl_section(dec->var_decl_section);
    }
}

void print_body(struct bodyNode* body)
{
    printf("{\n");
    print_stmt_list(body->stmt_list);
    printf("}\n");
}

void print_var_decl_section(struct var_decl_sectionNode* varDeclSection)
{
    printf("VAR\n");
    if (varDeclSection->var_decl_list != NULL)
    {
        print_var_decl_list(varDeclSection->var_decl_list);
    }
}

void print_var_decl_list(struct var_decl_listNode* varDeclList)
{
    print_var_decl(varDeclList->var_decl);
    if (varDeclList->var_decl_list != NULL)
    {
        print_var_decl_list(varDeclList->var_decl_list);
    }
}

void print_var_decl(struct var_declNode* varDecl)
{
    print_id_list(varDecl->id_list);
    printf(": ");
    print_type_name(varDecl->type_name);
    printf(";\n");
}

void print_type_decl_section(struct type_decl_sectionNode* typeDeclSection)
{
    printf("TYPE\n");
    if (typeDeclSection->type_decl_list != NULL)
    {
        print_type_decl_list(typeDeclSection->type_decl_list);
    }
}

void print_type_decl_list(struct type_decl_listNode* typeDeclList)
{
    print_type_decl(typeDeclList->type_decl);
    if (typeDeclList->type_decl_list != NULL)
    {
        print_type_decl_list(typeDeclList->type_decl_list);
    }
}

void print_type_decl(struct type_declNode* typeDecl)
{
    print_id_list(typeDecl->id_list);
    printf(": ");
    print_type_name(typeDecl->type_name);
    printf(";\n");
}

void print_type_name(struct type_nameNode* typeName)
{
    if (typeName->type != ID)
    {
        printf("%s ", reserved[typeName->type]);
    }
    else
    {
        printf("%s ", typeName->id);
    }
}

void print_id_list(struct id_listNode* idList)
{
    printf("%s ", idList->id);
    if (idList->id_list != NULL)
    {
        printf(", ");
        print_id_list(idList->id_list);
    }
}

void print_stmt_list(struct stmt_listNode* stmt_list)
{
    print_stmt(stmt_list->stmt);
    if (stmt_list->stmt_list != NULL)
    {
        print_stmt_list(stmt_list->stmt_list);
    }
    
}

void print_assign_stmt(struct assign_stmtNode* assign_stmt)
{
    printf("%s ", assign_stmt->id);
    printf("= ");
    print_expression_prefix(assign_stmt->expr);
    printf("; \n");
}

void print_stmt(struct stmtNode* stmt)
{
    switch (stmt->stmtType)
    {
        case ASSIGN:
            print_assign_stmt(stmt->assign_stmt);
            break;
        case WHILE:
            print_while_stmt(stmt->while_stmt);
            break;
        case DO:
            print_do_stmt(stmt->while_stmt);
            break;
        case SWITCH:
            print_switch_stmt(stmt->switch_stmt);
            break;
    }
}

void print_expression_prefix(struct exprNode* expr)
{
    if (expr->tag == EXPR)
    {
        printf("%s ", reserved[expr->op]);
        print_expression_prefix(expr->leftOperand);
        print_expression_prefix(expr->rightOperand);
    }
    else if (expr->tag == PRIMARY)
    {
        if (expr->primary->tag == ID)
        {
            printf("%s ", expr->primary->id);
        }
        else if (expr->primary->tag == NUM)
        {
            printf("%d ", expr->primary->ival);
        }
        else if (expr->primary->tag == REALNUM)
        {
            printf("%.4f ", expr->primary->fval);
        }
    }
}

void print_while_stmt(struct while_stmtNode* while_stmt)
{
    //TODO: implement this for your own debugging purposes
    printf("while ");
    print_condition(while_stmt->condition);
    print_body(while_stmt->body);
}

void print_do_stmt(struct while_stmtNode* do_stmt)
{
    // TODO: implement this for your own debugging purposes
    printf("do ");
    print_body(do_stmt->body);
    printf("while ");
    print_condition(do_stmt->condition);
}

void print_condition(struct conditionNode* condition)
{
    // print left operand
    if (condition->left_operand->tag == ID)
    {
        printf("%s ", condition->left_operand->id);
    }
    else if (condition->left_operand->tag == NUM)
    {
        printf("%d ", condition->left_operand->ival);
    }
    else if (condition->left_operand->tag == REALNUM)
    {
        printf("%.4f ", condition->left_operand->fval);
    }

    // print relop
    if (condition->relop != NULL){
        if (condition->relop == GREATER){
        printf("> ");
        }
        else if (condition->relop == LESS){
            printf("< ");
        }
        else if (condition->relop == NOTEQUAL){
            printf("<> ");
        }
        else if (condition->relop == LTEQ){
            printf("<= ");
        }
        else if (condition->relop == GTEQ){
            printf(">= ");
        }
        else{
            printf("INVALID_RELOP ");
        }

        // print right operand
        if (condition->right_operand->tag == ID)
        {
            printf("%s ", condition->right_operand->id);
        }
        else if (condition->right_operand->tag == NUM)
        {
            printf("%d ", condition->right_operand->ival);
        }
        else if (condition->right_operand->tag == REALNUM)
        {
            printf("%.4f ", condition->right_operand->fval);
        }
    }

    printf(" ;\n");

}


void print_case(struct caseNode* cas)
{
    // TODO: implement this for your own debugging purposes
}

void print_case_list(struct case_listNode* case_list)
{
    // TODO: implement this for your own debugging purposes
}

void print_switch_stmt(struct switch_stmtNode* switc)
{
    // TODO: implement this for your own debugging purposes
}

/* -------------------- PARSING AND BUILDING PARSE TREE -------------------- */

// Note that the following function is not
// called case because case is a keyword in C/C++
struct caseNode* cas()
{
    // TODO: implement this for EC
    struct caseNode* cas = ALLOC(struct caseNode);
    t_type = getToken();
    if (t_type == CASE){
        t_type = getToken();
        if (t_type == NUM){
            cas->num = atoi(token);
            t_type = getToken();
            if (t_type == COLON){
                t_type = getToken();
                if (t_type == LBRACE){
                    ungetToken();
                    cas->body = body();
                    return cas;
                }
                else{
                    syntax_error("LBRACE expected");
                }
            }
            else{
                syntax_error("COLON expected");
            }
        }
        else{
            //cout << t_type << endl;
            syntax_error("NUM expected");
        }
    }
    else{
        syntax_error("CASE expected");
    }
}

struct case_listNode* case_list()
{
    struct case_listNode* cas_list = ALLOC(struct case_listNode);
    t_type = getToken();
    if (t_type == CASE){
        ungetToken();
        cas_list->cas = cas();
        t_type = getToken();
        if (t_type == CASE){
            ungetToken();
            cas_list->case_list = case_list();
        }
        else{
            ungetToken();
            cas_list->case_list = NULL;
        }
        return cas_list;
    }
    else{
        syntax_error("CASE expected");
    }
    return NULL;
}

struct switch_stmtNode* switch_stmt()
{
    struct switch_stmtNode* switch_stm = ALLOC(struct switch_stmtNode);
    t_type = getToken();
    if (t_type == SWITCH){
        t_type = getToken();
        if (t_type == ID){
            switch_stm->id = strdup(token);
            t_type = getToken();
            if (t_type == LBRACE){
                t_type = getToken();
                if (t_type == CASE){
                    ungetToken();
                    switch_stm->case_list = case_list();
                    t_type = getToken();
                    if (t_type == RBRACE){

                    }
                    else{
                        syntax_error("RBRACE expected!");
                    }
                }
                else{
                    syntax_error("case_list expected.");
                }
            }
            else{
                //cout << reserved[LBRACE];
                syntax_error("LBRACE expected.");
            }
        }
        else{
            syntax_error("ID expected");
        }
    }
    else{
        syntax_error("SWITCH expected.");
    }
    return switch_stm;
}

struct while_stmtNode* do_stmt()
{
    // TODO: implement this
    struct while_stmtNode* do_stmt = ALLOC(struct while_stmtNode);
    t_type = getToken();
    if (t_type == DO){
        do_stmt->body = body();
        t_type = getToken();
        if (t_type == WHILE){
            do_stmt->condition = condition();
            t_type = getToken();
            if (t_type != SEMICOLON){
                syntax_error("SEMICOLON expected");
            }
        }
        else{
            syntax_error("WHILE token expected.");
        }
    }
    else{
        syntax_error("DO token expected.");
    }
    return do_stmt;
}

struct primaryNode* primary()
{
    // TODO: implement this
    struct primaryNode* prim = ALLOC(struct primaryNode);
    t_type = getToken();
    if (t_type == NUM || t_type == REALNUM || t_type == ID){
        prim->tag = t_type;
        if (t_type == NUM){
            prim->ival = atoi(token);
        }
        else if (t_type == REALNUM){
            prim->fval = atof(token);
        }
        else if (t_type == ID){
            prim->id = strdup(token);
        }
        else{
            syntax_error("Invalid token type. Expected NUM, REALNUM, or ID");
        }
    }
    return prim;
}

struct conditionNode* condition()
{
    bool firstIsID = false;
    struct conditionNode* cond = ALLOC(struct conditionNode);
    t_type = getToken();
    if (t_type == ID) // determine if left_operand is an ID.
        firstIsID = true;
    ungetToken();
    cond->left_operand = primary();

    t_type = getToken();
    if (t_type == GREATER || t_type == LESS || t_type == GTEQ || t_type == LTEQ || t_type == NOTEQUAL){
        cond->relop = t_type;
        cond->right_operand = primary();
        //cout << reserved[cond->relop] << " " << reserved[cond->right_operand->tag] << endl;
    }
    else {
        ungetToken();
        cond->relop = NULL;
        cond->right_operand = NULL;
    }

    return cond;
}

struct while_stmtNode* while_stmt()
{
    // TODO: implement this
    struct while_stmtNode* while_stm = ALLOC(struct while_stmtNode);
    t_type = getToken();
    if (t_type == WHILE){
        while_stm->condition = condition();
        while_stm->body = body();
    }
    else{
        syntax_error("WHILE token expected.");
    }
    return while_stm;
}

struct exprNode* factor()
{
    struct exprNode* facto;
    
    t_type = getToken();
    if (t_type == LPAREN)
    {
        facto = expr();
        t_type = getToken();
        if (t_type == RPAREN)
        {
            return facto;
        }
        else
        {
            syntax_error("factor. RPAREN expected");
        }
    }
    else if (t_type == NUM)
    {
        facto = ALLOC(struct exprNode);
        facto->primary = ALLOC(struct primaryNode);
        facto->tag = PRIMARY;
        facto->op = NOOP;
        facto->leftOperand = NULL;
        facto->rightOperand = NULL;
        facto->primary->tag = NUM;
        facto->primary->ival = atoi(token);
        return facto;
    }
    else if (t_type == REALNUM)
    {
        facto = ALLOC(struct exprNode);
        facto->primary = ALLOC(struct primaryNode);
        facto->tag = PRIMARY;
        facto->op = NOOP;
        facto->leftOperand = NULL;
        facto->rightOperand = NULL;
        facto->primary->tag = REALNUM;
        facto->primary->fval = atof(token);
        return facto;
    }
    else if (t_type == ID)
    {
        facto = ALLOC(struct exprNode);
        facto->primary = ALLOC(struct primaryNode);
        facto->tag = PRIMARY;
        facto->op = NOOP;
        facto->leftOperand = NULL;
        facto->rightOperand = NULL;
        facto->primary->tag = ID;
        facto->primary->id = strdup(token);
        return facto;
    }
    else
    {
        syntax_error("factor. NUM, REALNUM, or ID, expected");
    }
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct exprNode* term()
{
    struct exprNode* ter;
    struct exprNode* f;
    
    t_type = getToken();
    if (t_type == ID || t_type == LPAREN || t_type == NUM || t_type == REALNUM)
    {
        ungetToken();
        f = factor();
        t_type = getToken();
        if (t_type == MULT || t_type == DIV)
        {
            ter = ALLOC(struct exprNode);
            ter->op = t_type;
            ter->leftOperand = f;
            ter->rightOperand = term();
            ter->tag = EXPR;
            ter->primary = NULL;
            return ter;
        }
        else if (t_type == SEMICOLON || t_type == PLUS ||
                 t_type == MINUS || t_type == RPAREN)
        {
            ungetToken();
            return f;
        }
        else
        {
            syntax_error("term. MULT or DIV expected");
        }
    }
    else
    {
        syntax_error("term. ID, LPAREN, NUM, or REALNUM expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct exprNode* expr()
{
    struct exprNode* exp;
    struct exprNode* t;
    
    t_type = getToken();
    if (t_type == ID || t_type == LPAREN || t_type == NUM || t_type == REALNUM)
    {
        ungetToken();
        t = term();
        t_type = getToken();
        if (t_type == PLUS || t_type == MINUS)
        {
            exp = ALLOC(struct exprNode);
            exp->op = t_type;
            exp->leftOperand = t;
            exp->rightOperand = expr();
            exp->tag = EXPR;
            exp->primary = NULL;
            return exp;
        }
        else if (t_type == SEMICOLON || t_type == MULT ||
                 t_type == DIV || t_type == RPAREN)
        {
            ungetToken();
            return t;
        }
        else
        {
            syntax_error("expr. PLUS, MINUS, or SEMICOLON expected");
        }
    }
    else
    {
        syntax_error("expr. ID, LPAREN, NUM, or REALNUM expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct assign_stmtNode* assign_stmt()
{
    struct assign_stmtNode* assignStmt;
    
    t_type = getToken();
    if (t_type == ID)
    {
        assignStmt = ALLOC(struct assign_stmtNode);
        assignStmt->id = strdup(token);
        t_type = getToken();
        if (t_type == EQUAL)
        {
            assignStmt->expr = expr();
            t_type = getToken();
            if (t_type == SEMICOLON)
            {
                return assignStmt;
            }
            else
            {
                syntax_error("asign_stmt. SEMICOLON expected");
            }
        }
        else
        {
            syntax_error("assign_stmt. EQUAL expected");
        }
    }
    else
    {
        syntax_error("assign_stmt. ID expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct stmtNode* stmt()
{
    struct stmtNode* stm;
    
    stm = ALLOC(struct stmtNode);
    t_type = getToken();
    if (t_type == ID) // assign_stmt
    {
        ungetToken();
        stm->assign_stmt = assign_stmt();
        stm->stmtType = ASSIGN;
    }
    else if (t_type == WHILE) // while_stmt
    {
        ungetToken();
        stm->while_stmt = while_stmt();
        stm->stmtType = WHILE;
    }
    else if (t_type == DO)  // do_stmt
    {
        ungetToken();
        stm->while_stmt = do_stmt();
        stm->stmtType = DO;
    }
    else if (t_type == SWITCH) // switch_stmt
    {
        ungetToken();
        stm->switch_stmt = switch_stmt();
        stm->stmtType = SWITCH;
    }
    else
    {
        syntax_error("stmt. ID, WHILE, DO or SWITCH expected");
    }
    return stm;
}

struct stmt_listNode* stmt_list()
{
    struct stmt_listNode* stmtList;
    
    t_type = getToken();
    if (t_type == ID || t_type == WHILE ||
        t_type == DO || t_type == SWITCH)
    {
        ungetToken();
        stmtList = ALLOC(struct stmt_listNode);
        stmtList->stmt = stmt();
        t_type = getToken();
        if (t_type == ID || t_type == WHILE ||
            t_type == DO || t_type == SWITCH)
        {
            ungetToken();
            stmtList->stmt_list = stmt_list();
            return stmtList;
        }
        else // If the next token is not in FOLLOW(stmt_list),
            // let the caller handle it.
        {
            ungetToken();
            stmtList->stmt_list = NULL;
            return stmtList;
        }
    }
    else
    {
        syntax_error("stmt_list. ID, WHILE, DO or SWITCH expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct bodyNode* body()
{
    struct bodyNode* bod;
    
    t_type = getToken();
    if (t_type == LBRACE)
    {
        bod = ALLOC(struct bodyNode);
        bod->stmt_list = stmt_list();
        t_type = getToken();
        if (t_type == RBRACE)
        {
            return bod;
        }
        else
        {
            syntax_error("body. RBRACE expected");
        }
    }
    else
    {
        syntax_error("body. LBRACE expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct type_nameNode* type_name()
{
    struct type_nameNode* tName;
    
    tName = ALLOC(struct type_nameNode);
    t_type = getToken();
    if (t_type == ID || t_type == INT || t_type == REAL ||
        t_type == STRING || t_type == BOOLEAN || t_type == LONG)
    {
        tName->type = t_type;
        if (t_type == ID)
        {
            tName->id = strdup(token);
        }
        else
        {
            tName->id = NULL;
        }
        return tName;
    }
    else
    {
        syntax_error("type_name. type name expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct id_listNode* id_list()
{
    struct id_listNode* idList;
    
    idList = ALLOC(struct id_listNode);
    t_type = getToken();
    if (t_type == ID)
    {
        idList->id = strdup(token);
        t_type = getToken();
        if (t_type == COMMA)
        {
            idList->id_list = id_list();
            return idList;
        }
        else if (t_type == COLON)
        {
            ungetToken();
            idList->id_list = NULL;
            return idList;
        }
        else
        {
            syntax_error("id_list. COMMA or COLON expected");
        }
    }
    else
    {
        syntax_error("id_list. ID expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct type_declNode* type_decl()
{
    struct type_declNode* typeDecl;
    
    typeDecl = ALLOC(struct type_declNode);
    t_type = getToken();
    if (t_type == ID)
    {
        ungetToken();
        typeDecl->id_list = id_list();
        t_type = getToken();
        if (t_type == COLON)
        {
            typeDecl->type_name = type_name();
            t_type = getToken();
            if (t_type == SEMICOLON)
            {
                return typeDecl;
            }
            else
            {
                syntax_error("type_decl. SEMICOLON expected");
            }
        }
        else
        {
            syntax_error("type_decl. COLON expected");
        }
    }
    else
    {
        syntax_error("type_decl. ID expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct var_declNode* var_decl()
{
    struct var_declNode* varDecl;
    
    varDecl = ALLOC(struct var_declNode);
    t_type = getToken();
    if (t_type == ID)
    {
        ungetToken();
        varDecl->id_list = id_list();
        t_type = getToken();
        if (t_type == COLON)
        {
            varDecl->type_name = type_name();
            t_type = getToken();
            if (t_type == SEMICOLON)
            {
                return varDecl;
            }
            else
            {
                syntax_error("var_decl. SEMICOLON expected");
            }
        }
        else
        {
            syntax_error("var_decl. COLON expected");
        }
    }
    else
    {
        syntax_error("var_decl. ID expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct var_decl_listNode* var_decl_list()
{
    struct var_decl_listNode* varDeclList;
    
    varDeclList = ALLOC(struct var_decl_listNode);
    t_type = getToken();
    if (t_type == ID)
    {
        ungetToken();
        varDeclList->var_decl = var_decl();
        t_type = getToken();
        if (t_type == ID)
        {
            ungetToken();
            varDeclList->var_decl_list = var_decl_list();
            return varDeclList;
        }
        else
        {
            ungetToken();
            varDeclList->var_decl_list = NULL;
            return varDeclList;
        }
    }
    else
    {
        syntax_error("var_decl_list. ID expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct type_decl_listNode* type_decl_list()
{
    struct type_decl_listNode* typeDeclList;
    
    typeDeclList = ALLOC(struct type_decl_listNode);
    t_type = getToken();
    if (t_type == ID)
    {
        ungetToken();
        typeDeclList->type_decl = type_decl();
        t_type = getToken();
        if (t_type == ID)
        {
            ungetToken();
            typeDeclList->type_decl_list = type_decl_list();
            return typeDeclList;
        }
        else
        {
            ungetToken();
            typeDeclList->type_decl_list = NULL;
            return typeDeclList;
        }
    }
    else
    {
        syntax_error("type_decl_list. ID expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct var_decl_sectionNode* var_decl_section()
{
    struct var_decl_sectionNode *varDeclSection;
    
    varDeclSection = ALLOC(struct var_decl_sectionNode);
    t_type = getToken();
    if (t_type == VAR)
    {
        // no need to ungetToken()
        varDeclSection->var_decl_list = var_decl_list();
        return varDeclSection;
    }
    else
    {
        syntax_error("var_decl_section. VAR expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct type_decl_sectionNode* type_decl_section()
{
    struct type_decl_sectionNode *typeDeclSection;
    
    typeDeclSection = ALLOC(struct type_decl_sectionNode);
    t_type = getToken();
    if (t_type == TYPE)
    {
        typeDeclSection->type_decl_list = type_decl_list();
        return typeDeclSection;
    }
    else
    {
        syntax_error("type_decl_section. TYPE expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct declNode* decl()
{
    struct declNode* dec;
    
    dec = ALLOC(struct declNode);
    dec->type_decl_section = NULL;
    dec->var_decl_section = NULL;
    t_type = getToken();
    if (t_type == TYPE)
    {
        ungetToken();
        dec->type_decl_section = type_decl_section();
        t_type = getToken();
        if (t_type == VAR)
        {
            // type_decl_list is epsilon
            // or type_decl already parsed and the
            // next token is checked
            ungetToken();
            dec->var_decl_section = var_decl_section();
        }
        else
        {
            ungetToken();
            dec->var_decl_section = NULL;
        }
        return dec;
    }
    else
    {
        dec->type_decl_section = NULL;
        if (t_type == VAR)
        {
            // type_decl_list is epsilon
            // or type_decl already parsed and the
            // next token is checked
            ungetToken();
            dec->var_decl_section = var_decl_section();
            return dec;
        }
        else
        {
            if (t_type == LBRACE)
            {
                ungetToken();
                dec->var_decl_section = NULL;
                return dec;
            }
            else
            {
                syntax_error("decl. LBRACE expected");
            }
        }
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct programNode* program()
{
    struct programNode* prog;
    
    prog = ALLOC(struct programNode);
    t_type = getToken();
    if (t_type == TYPE || t_type == VAR || t_type == LBRACE)
    {
        ungetToken();
        prog->decl = decl();
        prog->body = body();
        return prog;
    }
    else
    {
        syntax_error("program. TYPE or VAR or LBRACE expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

// void dup_error(string code, char* id){
//     printf("ERROR CODE %s %s\n", code, id.c_str());
//     exit(1);
// }

void populateTypeDictionary(unordered_map<string, type_nameNode* >* dict, unordered_map<string, bool>* idTypeDict, programNode* parseTree){
    type_decl_sectionNode* section = parseTree->decl->type_decl_section;
    if (section != NULL){
        ++line_num;
        type_decl_listNode* list = parseTree->decl->type_decl_section->type_decl_list;
        while (list != NULL){
            ++line_num;
            type_nameNode* type_name = list->type_decl->type_name;
            bool isId = false;
            string typeID = "";
            if (type_name->type == ID){
                // Add to custom type dictionary
                isId = true;
                if (!(*dict).count(type_name->id)){
                    (*idTypeDict).insert({{type_name->id, true}});
                    typeID = string(type_name->id);
                }
            }
            else{
                //(*idTypeDict).insert({{reserved[type_name->type], true}});
            }

            id_listNode* idList = list->type_decl->id_list;
            while (idList != NULL){
                
                if ((*dict).count(idList->id)){
                    // Explicit type redeclared explicitly
                    printf("ERROR CODE 1.1 %s\n", idList->id);
                    exit(1);
                }
                else if ((*idTypeDict).count(idList->id) && typeID.compare(idList->id) == 0){
                    // do nothing
                    idList = idList->id_list;
                } 
                else if ((*idTypeDict).count(idList->id)){
                    // Implicit type redeclared explicitly
                    printf("ERROR CODE 1.2 %s\n", idList->id);
                    exit(1);
                }
                else{
                    if (isId && (*dict).count(type_name->id)){
                        // If type of type has been explicitly declared, set type of type to type of type's type.
                        (*dict).insert({{idList->id, (*dict).at(type_name->id)}});
                    }
                    else{
                        // Else, use current type
                        (*dict).insert({{idList->id, type_name}});
                    }
                    
                    vals.push_back(idList->id);
                    idList = idList->id_list;
                }
            }
            if (type_name->type == ID){
                // Add to custom type dictionary
                if (!(*dict).count(type_name->id)){
                    vals.push_back(type_name->id);
                }
            }
            list = list->type_decl_list;
        }
    }

}

void populateVarDictionary(unordered_map<string, type_nameNode* >* varDict, unordered_map<string, type_nameNode* >* typeDict, unordered_map<string, bool>* idTypeDict, programNode* parseTree){
    var_decl_sectionNode* section = parseTree->decl->var_decl_section;
    if (section != NULL){
        ++line_num;
        var_decl_listNode* list = parseTree->decl->var_decl_section->var_decl_list;
        while (list != NULL){
            ++line_num;
            type_nameNode* type_name;
            bool idTypeAdded = false;
            if (list->var_decl->type_name->type == ID && (*typeDict).count(list->var_decl->type_name->id)){
                // Exists in typeDict. Get type of type.
                type_name = (*typeDict).at(list->var_decl->type_name->id);
            }
            else{
                type_name = list->var_decl->type_name;
            }
            if (type_name->type == ID){
                (*idTypeDict).insert({{type_name->id, true}});
                idTypeAdded = true;
            }

            else{
                //(*idTypeDict).insert({{reserved[type_name->type], true}});
            }
            id_listNode* idList = list->var_decl->id_list;
            while (idList != NULL){
                if ((*typeDict).count(idList->id) || (*idTypeDict).count(idList->id)){
                    // Programmer-defined type redeclared as variable
                    printf("ERROR CODE 1.3 %s\n", idList->id);
                    exit(1);
                }
                else if ((*varDict).count(idList->id)){
                    // Variable declared more than once
                    printf("ERROR CODE 2.1 %s\n", idList->id);
                    exit(1);
                }
                else if (type_name->type == ID && (*varDict).count(type_name->id)){
                    // Variable used as a type
                    printf("ERROR CODE 2.2 %s\n", type_name->id);
                    exit(1);
                }
                else{
                    (*varDict).insert({{idList->id, type_name}});
                    vals.push_back(idList->id);
                    idList = idList->id_list;
                }
            }
            // END
            if (type_name->type == ID && !(*typeDict).count(type_name->id) && !(*idTypeDict).count(type_name->id)){
                vals.push_back(type_name->id);
            }
            else if(idTypeAdded){
                vals.push_back(type_name->id);
            }
            list = list->var_decl_list;
        }
    }
}

void parse_Body(unordered_map<string, type_nameNode* >* varDict, unordered_map<string, type_nameNode* >* typeDict, unordered_map<string, bool>* idTypeDict, bodyNode* body){
    stmt_listNode* stmt_list = body->stmt_list;
    while (stmt_list != NULL){
        stmtNode* stmt = stmt_list->stmt;
        switch(stmt->stmtType){
            case WHILE: parse_While(varDict, typeDict, idTypeDict, stmt->while_stmt); break;
            case ASSIGN: parse_Assign(varDict, typeDict, idTypeDict, stmt->assign_stmt); break;
            case DO: parse_Do(varDict, typeDict, idTypeDict, stmt->while_stmt); break;
            case SWITCH: parse_Switch(varDict, typeDict, idTypeDict, stmt->switch_stmt); break;
        }
        //END
        stmt_list = stmt_list->stmt_list;
    }
}

void outputValues(unordered_map<string, type_nameNode* >* varDict, unordered_map<string, type_nameNode* >* typeDict, unordered_map<string, bool>* idTypeDict){
    unordered_map<string, bool> processed;

    for(auto it = vals.begin(); it != vals.end(); it++){
        if (!processed.count(*it)){
            processed.insert({{*it, true}});
            cout << *it;
            string iType;
            if ((*varDict).count(*it)){
                if ((*varDict).at(*it)->type == ID){
                    iType = (*varDict).at(*it)->id;
                }
                else{
                    // for idTypeDict...
                    if ((*varDict).at(*it)->type < 100){
                        // Normal type defined in enum (available in reserved[])
                        iType = strdup(reserved[(*varDict).at(*it)->type]) ;
                    }
                    else{
                        long long temp_LL = (*varDict).at(*it)->type;
                        iType = to_string(temp_LL);
                    }
                    
                }
            }
            else if ((*typeDict).count(*it)){
                if ((*typeDict).at(*it)->type == ID){
                    iType = (*typeDict).at(*it)->id;
                }
                else{
                    if ((*typeDict).at(*it)->type < 100){
                        iType = strdup(reserved[(*typeDict).at(*it)->type]) ;
                    }
                    else{
                        long long temp_LL = (*typeDict).at(*it)->type;
                        iType = to_string(temp_LL);
                    }
                }
            }
            else{
                iType = *it;
            }
            //cout << "!! " << iType << endl;
            for (auto it2 = it+1; it2 != vals.end(); it2++){
                if (!processed.count(*it2) && ((*varDict).count(*it2) || (*typeDict).count(*it2) || (*idTypeDict).count(*it2) )) {
                    string tmp;
                    if ((*varDict).count(*it2)){
                        if ((*varDict).at(*it2)->type == ID){
                            tmp = (*varDict).at(*it2)->id;
                        }
                        else{
                            if ((*varDict).at(*it2)->type < 100){
                                tmp = strdup(reserved[(*varDict).at(*it2)->type]) ;
                            }
                            else{
                                long long temp_LL = (*varDict).at(*it2)->type;
                                tmp = to_string(temp_LL);
                            }
                        }
                    }
                    else if ((*typeDict).count(*it2)){
                        if ((*typeDict).at(*it2)->type == ID){
                            tmp = (*typeDict).at(*it2)->id;
                        }
                        else{
                            if ((*typeDict).at(*it2)->type < 100){
                                tmp = strdup(reserved[(*typeDict).at(*it2)->type]) ;
                            }
                            else{
                                long long temp_LL = (*typeDict).at(*it2)->type;
                                tmp = to_string(temp_LL);
                            }
                        }
                    }
                    else if ((*idTypeDict).count(iType)) {
                        tmp = iType;
                    }
                    //cout << "  >> " << *it << "  " << tmp << endl;
                    
                    if ( (!processed.count(*it2)) && tmp.compare(iType) == 0) {
                        processed.insert({{*it2, true}});
                        cout << " " << *it2;
                    }
                    
                    
                    
                }
                
            }
            cout << " #"<<endl;
        }
    }
}

string getTypeString(type_nameNode* t){
    if (t->type == ID){
        return string(t->id);
    }
    else if (t->type < 100){
        string temp = string(reserved[t->type]);
        if (temp.compare("NUM") == 0){
            temp = "INT";
        }
        else if (temp.compare("REALNUM") == 0){
            temp = "REAL";
        }
        return string(temp);
    }
    else{
        return to_string((long long) t->type);
    }
}

int main()
{
    struct programNode* parseTree;
    parseTree = program();
    // TODO: remove the next line after you complete the parser
    //print_parse_tree(parseTree); // This is just for debugging purposes
    // TODO: do type checking & print output according to project specification
    unordered_map<string, type_nameNode* > typeDict;
    unordered_map<string, bool> idTypeDict;
    unordered_map<string, type_nameNode* > varDict;

    vals.push_back("BOOLEAN");
    vals.push_back("INT");
    vals.push_back("LONG");
    vals.push_back("REAL");
    vals.push_back("STRING");

    populateTypeDictionary(&typeDict, &idTypeDict, parseTree);
    populateVarDictionary(&varDict, &typeDict, &idTypeDict, parseTree);

    ++line_num;
    parse_Body(&varDict, &typeDict, &idTypeDict, parseTree->body);

    // cout << "Vals:" << endl;
    // for (auto it = vals.begin(); it < vals.end(); it++){
    //     cout << (*it) << " ";
    // }
    // cout << endl;
    // printf("TypeDict:\n");
    // for (auto it = typeDict.begin(); it != typeDict.end(); it++){
    //     char* testType;
    //     if (it->second->type == ID){
    //         testType = it->second->id;
    //     }
    //     else{
    //         if (it->second->type < 100){
    //             testType = strdup(reserved[it->second->type]);
    //         }
    //         else{
    //             testType = strdup(to_string((long long) it->second->type).c_str());
    //         }
    //     }
    //     printf("\t%s %s\n", it->first.c_str(), testType);
    // }
    // printf("\nidTypeDict:\n");
    // for (auto it = idTypeDict.begin(); it != idTypeDict.end(); it++){
    //     printf("\t%s \n", it->first.c_str());
    // }
    // printf("\nvarDict:\n");
    // for (auto it = varDict.begin(); it != varDict.end(); it++){
    //     char* testType;
    //     if (it->second->type == ID){
    //         testType = it->second->id;
    //     }
    //     else{
    //         if (it->second->type < 100){
    //             testType = strdup(reserved[it->second->type]);
    //         }
    //         else{
    //             testType = strdup(to_string((long long) it->second->type).c_str());
    //         }
    //     }
    //     printf("\t%s %s\n", it->first.c_str(), testType);
    // }
    // printf("\n");

    outputValues(&varDict, &typeDict, &idTypeDict);

    return 0;
}

void parse_Condition(unordered_map<string, type_nameNode* >* varDict, unordered_map<string, type_nameNode* >* typeDict, unordered_map<string, bool>* idTypeDict, conditionNode* condition){
    primaryNode* left = condition->left_operand;
    int relop = condition->relop;
    primaryNode* right = condition->right_operand;


    // inspect operand values
    if (left->tag == ID){
        // Verify variable isn't a type.
        if ((*typeDict).count(left->id) || (*idTypeDict).count(left->id)){
            printf("ERROR CODE 1.4 %s\n", left->id);
            exit(1);
        }
        else{
            // TODO: something
        }
    }

    if (right != NULL){
        if (right->tag == ID && ((*typeDict).count(right->id) || (*idTypeDict).count(right->id))){
            if (right->tag == ID){
                printf("ERROR CODE 1.4 %s\n", right->id);
            }
            
            exit(1);
        }
        else{
            // Get type_names of both operands
            type_nameNode* t1;
            type_nameNode* t2;
            if (left->tag == ID){
                t1 = parse_Primary(varDict, typeDict, idTypeDict, left);
            }
            else{
                t1 = ALLOC(struct type_nameNode);
                t1->type = left->tag;
                t1->id = NULL;
            }
            if (right->tag == ID){
                t2 = parse_Primary(varDict, typeDict, idTypeDict, right);
            }
            else{
                t2 = ALLOC(struct type_nameNode);
                t2->type = right->tag;
                t2->id = NULL;
            }
            string err = "C3";
            compareTypeNames(varDict, typeDict, idTypeDict, t1, t2, err);
        }
    }
    else if (left->tag != BOOLEAN){
        if (left->tag == ID && !(*varDict).count(left->id)){
            // implicit declaration. create variable and set type to BOOLEAN. Add to vals dict.
            type_nameNode* type_name_temp = ALLOC(struct type_nameNode);
            type_name_temp->type = BOOLEAN;
            type_name_temp->id = NULL;
            (*varDict).insert({{left->id, type_name_temp}}); // Add to variable dictionary
            vals.push_back(left->id);
        }
        else if (left->tag == ID && getTypeString((*varDict).at(left->id)).compare("BOOLEAN") == 0){
            // is of type boolean
        }
        else if(left->tag == ID && (*idTypeDict).count(getTypeString((*varDict).at(left->id)))){
            // implicit type. convert all of this type to BOOLEAN
            string type_to_replace = getTypeString((*varDict).at(left->id));
            for (auto it = (*varDict).begin(); it != (*varDict).end(); it++){
                string it_type = getTypeString(it->second);
                if (type_to_replace.compare(it_type) == 0){
                    it->second->type = BOOLEAN;
                    it->second->id = NULL;
                }

            }
            type_nameNode* new_type_name = ALLOC(struct type_nameNode);
            new_type_name->type = BOOLEAN;
            new_type_name->id = NULL;
            (*typeDict).insert({{type_to_replace, new_type_name}});
            (*idTypeDict).erase(type_to_replace);
        }
        else{
            type_mismatch_error("C4");
        }
        
    }
}

void parse_While(unordered_map<string, type_nameNode* >* varDict, unordered_map<string, type_nameNode* >* typeDict, unordered_map<string, bool>* idTypeDict, while_stmtNode* while_stmt){
   ++line_num;
   conditionNode* condition = while_stmt->condition;
   bodyNode* body = while_stmt->body;

   parse_Condition(varDict, typeDict, idTypeDict, condition);
   parse_Body(varDict, typeDict, idTypeDict, body);
   ++line_num;
}

type_nameNode* parse_Primary(unordered_map<string, type_nameNode* >* varDict, unordered_map<string, type_nameNode* >* typeDict, unordered_map<string, bool>* idTypeDict, primaryNode* primary){
    if (primary->tag == ID){
        char* prim_id = primary->id;
        if ((*varDict).count(prim_id)){
            // variable exists in dictionary. Return type_name
            type_nameNode* temp_type = (*varDict).at(prim_id);
            if (temp_type != NULL){
                return temp_type;
            }
            // TODO: handle implicit assignment. DO NOT UNCOMMENT!
            // else{
            //     cout << "variable used before type has been defined." << endl;
            //     exit(1);
            // }
        }
        else{
            type_nameNode* type_name_temp = ALLOC(struct type_nameNode);
            type_name_temp->type = imp_type_num;
            ++imp_type_num; // increment implicit type number counter
            type_name_temp->id = NULL;
            (*varDict).insert({{prim_id, type_name_temp}}); // Add to variable dictionary
            vals.push_back(prim_id);
            // cout << type_name_temp->type << endl;
            long long tmp_string = type_name_temp->type;
            (*idTypeDict).insert({{to_string(tmp_string), true}});
            return type_name_temp;
        }
    }
    else{
        type_nameNode* temp_type = ALLOC(struct type_nameNode);
        if (primary->tag == NUM){
            temp_type->type = INT;
        }
        else{
            temp_type->type = REAL;
        }
        
        temp_type->id = NULL;
        return temp_type;
    }
    return NULL;
}

void parse_Do(unordered_map<string, type_nameNode* >* varDict, unordered_map<string, type_nameNode* >* typeDict, unordered_map<string, bool>* idTypeDict, while_stmtNode* do_stmt){
    ++line_num;
    conditionNode* condition = do_stmt->condition;
    bodyNode* body = do_stmt->body;

    parse_Body(varDict, typeDict, idTypeDict, body);
    ++line_num;
    parse_Condition(varDict, typeDict, idTypeDict, condition);
}

void parse_Switch(unordered_map<string, type_nameNode* >* varDict, unordered_map<string, type_nameNode* >* typeDict, unordered_map<string, bool>* idTypeDict, switch_stmtNode* switch_stmt){
    ++line_num;
    if ((*varDict).count(switch_stmt->id)){
        // variable exists.
        string type = getTypeString((*varDict)[switch_stmt->id]);
        if ((*idTypeDict).count(type)){
            // implicit type. change all var of this type to INT

            for (auto it = (*varDict).begin(); it != (*varDict).end(); it++){
                string curr_type = getTypeString(it->second);
                if (type.compare(curr_type) == 0){
                    it->second->type = INT;
                    it->second->id = NULL;
                }
            }

            // erase from idTypeDict and add to typeDict
            type_nameNode* new_type_name = ALLOC(struct type_nameNode);
            new_type_name->type = INT;
            new_type_name->id = NULL;
            (*typeDict).insert({{type, new_type_name}});
            (*idTypeDict).erase(type);

        }
        else{
            // explicit type. 
            if ((*varDict)[switch_stmt->id]->type != INT && (*varDict)[switch_stmt->id]->type != NUM){
                // cout << (*varDict)[switch_stmt->id]->type;
                type_mismatch_error("C5");
            }
            else{
                // is a boolean
            }
        }
    }
    else{
        // Implicitly defined variable. create new var with type NUM
        type_nameNode* new_type_name = ALLOC(struct type_nameNode);
        new_type_name->type = INT;
        new_type_name->id = NULL;
        vals.push_back(switch_stmt->id);
        (*varDict).insert({{switch_stmt->id, new_type_name}});
    }

    ++line_num;
    parse_CaseList(varDict, typeDict, idTypeDict, switch_stmt->case_list);
    ++line_num;
}

void parse_CaseList(unordered_map<string, type_nameNode* >* varDict, unordered_map<string, type_nameNode* >* typeDict, unordered_map<string, bool>* idTypeDict, case_listNode* case_list){
    case_listNode* cas_list = case_list;
    while(cas_list != NULL){
        ++line_num;
        ++line_num;
        
        parse_Body(varDict, typeDict, idTypeDict, cas_list->cas->body);
        ++line_num;
        cas_list = cas_list->case_list;
    }
}

type_nameNode* parse_Expr(unordered_map<string, type_nameNode* >* varDict, unordered_map<string, type_nameNode* >* typeDict, unordered_map<string, bool>* idTypeDict, exprNode* expr){
    expression_type tag = expr->tag;
    if (tag == PRIMARY){
        primaryNode* temp_prim = expr->primary;
        type_nameNode* temp_type;
        if (temp_prim->tag == ID){
            if ((*typeDict).count(temp_prim->id) || (*idTypeDict).count(temp_prim->id)){
                printf("ERROR CODE 1.4 %s\n", temp_prim->id);
                exit(1);
            }
        }
        temp_type = parse_Primary(varDict, typeDict, idTypeDict, temp_prim);

        if (temp_type == NULL){
            // shouldn't happen anymore
        }
        return temp_type;
    }
    else{
        type_nameNode* t1 = parse_Expr(varDict, typeDict, idTypeDict, expr->leftOperand);
        type_nameNode* t2 = parse_Expr(varDict, typeDict, idTypeDict, expr->rightOperand);
        return compareTypeNames(varDict, typeDict, idTypeDict, t1, t2, "C2");
    }
}

void parse_Assign(unordered_map<string, type_nameNode* >* varDict, unordered_map<string, type_nameNode* >* typeDict, unordered_map<string, bool>* idTypeDict, assign_stmtNode* assign_stmt){
    ++line_num;
    char* id = assign_stmt->id;
    exprNode* expr = assign_stmt->expr;
    bool isImplicit = false;
    // Verify that ID isn't a type
    if ((*typeDict).count(id) || (*idTypeDict).count(id)){
        printf("ERROR CODE 1.4 %s\n", id);
        exit(1);
    }
    else if (!(*varDict).count(id)){
        // Variable is implicitly defined.
        isImplicit = true;
        type_nameNode* type_name_temp = ALLOC(struct type_nameNode);
        type_name_temp->type = imp_type_num;
        ++imp_type_num; // increment implicit type number counter
        type_name_temp->id = NULL;
        (*varDict).insert({{id, type_name_temp}}); // Add to variable dictionary
        vals.push_back(id);

        long long tmp_string = type_name_temp->type;
        (*idTypeDict).insert({{to_string(tmp_string), true}});
    }

    // Parse the expression
    type_nameNode* expr_type = parse_Expr(varDict, typeDict, idTypeDict, expr);

    if (isImplicit){
        // Set implicitly defined variable to type of expression
        (*varDict)[id] = expr_type;
    }
    else{
        type_nameNode* var_type = (*varDict)[id];
        if (var_type->type == ID && (*idTypeDict).count(var_type->id)){
            // Assigning to implicit type
            if (expr_type->type == ID){

            }
        }
        compareTypeNames(varDict, typeDict, idTypeDict, (*varDict)[id], expr_type, "C1");
    }
}

void type_mismatch_error(string constraint){
    cout << "TYPE MISMATCH " << line_num << " " << constraint;
    exit(1);
}

type_nameNode* compareTypeNames(unordered_map<string, type_nameNode* >* varDict, unordered_map<string, type_nameNode* >* typeDict, unordered_map<string, bool>* idTypeDict, type_nameNode* t1, type_nameNode* t2, string err){
    char* n1;
    char* n2;
    if (t1 != NULL && t2 != NULL){
        if (t1->type == ID){
            n1 = t1->id;
        }
        else{
            if (t1->type < 100){
                n1 = strdup(reserved[t1->type]);
            }
            else{
                // implicit type
                n1 = NULL;
            }

        }
        //printf(" >> N1: %s\n", n1);
        if (t2->type == ID){
            n2 = t2->id;
        }
        else{
            if (t2->type < 100){
                n2 = strdup(reserved[t2->type]);
            }
            else{
                // implicit type
                n2 = NULL;
            }

        }
        //printf(" >> N2: %s\n", n2);
        if (n1 != NULL && n2 != NULL){
            //cout << n1 << " " << n2 << endl;

            if (strcmp(n1, n2) == 0){
                // identical types
                return t1;
            }
            else if ((*idTypeDict).count(n1) && (*idTypeDict).count(n2)){
                // BOTH types are implicitly declared from TYPE/VAR secions. Make change all var with type of t2 to t1. Remove t2 from 
                // idTypeDict. Add it to typeDict.
                //cout << "BOTH n1 and n2 in idTypeDict" << endl;
                for(auto it = (*varDict).begin(); it != (*varDict).end(); it++){
                    if (it->second->type == t2->type){
                        if (it->second->type == ID){
                            if (it->second->id == t2->id){
                                it->second = t1;
                            }
                        }
                        else{
                            it->second = t1;
                        }
                        
                    }
                }
                for(auto it = (*typeDict).begin(); it != (*typeDict).end(); it++){
                    if (it->second->type == t2->type){
                        if (it->second->type == ID){
                            if (it->second->id == t2->id){
                                it->second = t1;
                            }
                        }
                        else{
                            it->second = t1;
                        }
                        
                    }
                }
                (*typeDict).insert({{n2, t1}});
                (*idTypeDict).erase(n2);
            }
            else if ((*idTypeDict).count(n1)){
                //cout << "n1 in idTypeDict" << endl;
                for(auto it = (*varDict).begin(); it != (*varDict).end(); it++){
                    if (it->second->type == ID){
                            if (it->second->id == t1->id){
                                //cout << "KEY " << it->first << " changed from " << it->second->id << " to " << reserved[t2->type] << endl;
                                it->second = t2;

                            }
                        }
                        else{
                            //cout << "KEY " << it->first << " changed from " << reserved[it->second->type] << " to " << t2->type << endl;
                            it->second = t2;
                        }
                }
                for(auto it = (*typeDict).begin(); it != (*typeDict).end(); it++){
                    //cout << "test"<< endl;
                    if (it->second->type == t1->type){

                        if (it->second->type == ID){
                            if (it->second->id == t1->id){
                                //cout << "KEY " << it->first << " changed from " << it->second->id << " to " << reserved[t2->type] << endl;
                                it->second = t2;
                                
                            }
                        }
                        else{
                            //cout << "KEY " << it->first << " changed from " << reserved[it->second->type] << " to " << reserved[t2->type] << endl;
                            it->second = t2;
                        }
                    }
                }
                (*idTypeDict).erase(n1);
                (*typeDict).insert({{n1, t2}});
                t1 = t2;

            }
            else if ((*idTypeDict).count(n2)){
                // type of t2 is currently implicitly defined.
                //cout << "n2 in idTypeDict" << endl;
                for(auto it = (*varDict).begin(); it != (*varDict).end(); it++){
                    if (it->second->type == t2->type){
                        if (it->second->type == ID){
                            if (it->second->id == t2->id){
                                //cout << "KEY " << it->first << " changed from " << it->second->id << " to " << reserved[t1->type] << endl;
                                it->second = t1;
                            }
                        }
                        else{
                            it->second = t1;
                        }
                    }
                }
                for(auto it = (*typeDict).begin(); it != (*typeDict).end(); it++){
                    if (it->second->type == t2->type){
                        //cout << "test" << endl;
                        if (it->second->type == ID){
                            if (it->second->id == t2->id){
                                //cout << "KEY " << it->first << " changed from " << it->second->id << " to " << t1->type << endl;
                                it->second = t1;
                                
                            }
                        }
                        else{
                            it->second = t1;
                        }
                    }
                }
                (*idTypeDict).erase(n2);
                (*typeDict).insert({{n2, t1}});
                t2 = t1;
            }
            else{
                type_mismatch_error(err);
            }
        }
        else{
            if (n1 == NULL && n2 != NULL){ 
                //cout << "n2: " << n2 << endl;
                for (auto it = (*varDict).begin(); it != (*varDict).end(); it++){
                    //cout << "TEST:  " << it->second->type << "  " << t1->type << endl;
                    if (it->second->type == t1->type){
                        
                        if (it->second->type == ID){
                            if (it->second->id == t1->id){
                                it->second = t2;
                            }
                        }
                        else{
                            it->second = t2;
                        }

                    }
                }
                t1 = t2;
                return t1;
            }
            else if (t1 != NULL && t2 == NULL){
                for (auto it = (*varDict).begin(); it != (*varDict).end(); it++){
                    if (it->second->type == t2->type){
                        if (it->second->type == ID){
                            if (it->second->id == t2->id){
                                it->second = t1;
                            }
                        }
                        else{
                            it->second = t1;
                        }
                    }
                }
                t2 = t1;
                return t1;
            }
            else{
                for (auto it = (*varDict).begin(); it != (*varDict).end(); it++){
                    if (it->second->type == t2->type){
                        
                        if (it->second->type == ID){
                            if (it->second->id == t2->id){
                                it->second = t1;
                                //cout << "KEY " << it->first << " changed from " << it->second->type << " to " << t1->type << endl;
                            }
                        }
                        else{
                            it->second = t1;
                            //cout << "KEY " << it->first << " changed from " << it->second->type << " to " << t1->type << endl;
                        }
                    }
                }
                t2 = t1;
                return t1;
            }
        }
    }



    return NULL;
}

void printEverything(unordered_map<string, type_nameNode* >* varDict, unordered_map<string, type_nameNode* >* typeDict, unordered_map<string, bool>* idTypeDict){
        cout << endl;
    cout << "Vals:" << endl;
    for (auto it = vals.begin(); it < vals.end(); it++){
        cout << (*it) << " ";
    }
    printf("\nTypeDict:\n");
    for (auto it = (*typeDict).begin(); it != (*typeDict).end(); it++){
        char* testType;
        if (it->second->type == ID){
            testType = it->second->id;
        }
        else{
            if (it->second->type < 100){
                testType = strdup(reserved[it->second->type]);
            }
            else{
                testType = strdup(to_string((long long) it->second->type).c_str());
            }
        }
        printf("\t%s %s\n", it->first.c_str(), testType);
    }
    printf("\nidTypeDict:\n");
    for (auto it = (*idTypeDict).begin(); it != (*idTypeDict).end(); it++){
        printf("\t%s \n", it->first.c_str());
    }
    printf("\nvarDict:\n");
    for (auto it = (*varDict).begin(); it != (*varDict).end(); it++){
        char* testType;
        if (it->second->type == ID){
            testType = it->second->id;
        }
        else{
            if (it->second->type < 100){
                testType = strdup(reserved[it->second->type]);
            }
            else{
                testType = strdup(to_string((long long) it->second->type).c_str());
            }
        }
        printf("\t%s %s\n", it->first.c_str(), testType);
    }
    printf("\n");
}