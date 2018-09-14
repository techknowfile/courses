#ifndef __SYNTAX__H__
#define __SYNTAX__H__
/*----------------------------------------------------------------------------
 Note: the code in this file is not to be shared with anyone or posted online.
 (c) Rida Bazzi, 2015, Adam Doupe, 2015
 ----------------------------------------------------------------------------*/

/* -------------------- PARSE TREE TYPES -------------------- */

typedef enum {
    PRIMARY = 100,
    EXPR,
    NOOP,
    ASSIGN
} expression_type;

struct programNode
{
    struct declNode* decl;
    struct bodyNode* body;
};

struct declNode
{
    // A NULL field means that the section is empty
    struct type_decl_sectionNode* type_decl_section;
    struct var_decl_sectionNode* var_decl_section;
};

struct type_decl_sectionNode
{
    struct type_decl_listNode* type_decl_list;
};

struct var_decl_sectionNode
{
    struct var_decl_listNode* var_decl_list;
};

struct type_decl_listNode
{
    struct type_declNode * type_decl;
    struct type_decl_listNode* type_decl_list;
};

struct var_decl_listNode
{
    struct var_declNode * var_decl;
    struct var_decl_listNode* var_decl_list;
};

struct type_declNode
{
    struct id_listNode* id_list;
    struct type_nameNode* type_name;
};

struct var_declNode
{
    struct id_listNode* id_list;
    struct type_nameNode* type_name;
};

struct type_nameNode
{
    int type; // INT, REAL, STRING, BOOLEAN, ID, LONG
    char* id; // actual string when type is ID
};

struct id_listNode
{
    char * id;
    struct id_listNode* id_list;
};

struct bodyNode
{
    struct stmt_listNode* stmt_list;
};

struct stmt_listNode
{
    struct stmtNode* stmt;
    struct stmt_listNode * stmt_list;
};

struct stmtNode
{
    int stmtType; // WHILE, ASSIGN, DO, or SWITCH
    
    // Note that while_stmt should be used for both
    // DO and WHILE statement types
    
    union
    {
        struct while_stmtNode* while_stmt;
        struct assign_stmtNode* assign_stmt;
        struct switch_stmtNode* switch_stmt;
    };
};

struct conditionNode
{
    int relop;
    struct primaryNode* left_operand;
    struct primaryNode* right_operand;
};

struct while_stmtNode
{
    struct conditionNode* condition;
    struct bodyNode* body;
};

struct assign_stmtNode
{
    char* id;
    struct exprNode* expr;
};

struct exprNode
{
    int op; // PLUS , MINUS, MULT, ... or NOOP
    expression_type tag; // PRIMARY or EXPR
    struct primaryNode* primary;
    struct exprNode * leftOperand;
    struct exprNode * rightOperand;
};

struct primaryNode
{
    int tag; // NUM, REALNUM or ID
    int ival;
    float fval;
    char *id;
};

struct caseNode
{
    int num;
    struct bodyNode* body;	// body of the case
};

struct case_listNode
{
    struct caseNode* cas;	// case is a keyword in C/C++
    struct case_listNode* case_list;
};

struct switch_stmtNode
{
    char* id;
    struct case_listNode* case_list;
};

/* -------------------- PARSE TREE FUNCTIONS -------------------- */

#define ALLOC(t) (t*) calloc(1, sizeof(t))

void print_decl(struct declNode* dec);
void print_body(struct bodyNode* body);

void print_type_decl_section(struct type_decl_sectionNode* typeDeclSection);
void print_type_decl_list(struct type_decl_listNode* typeDeclList);
void print_type_decl(struct type_declNode* typeDecl);

void print_var_decl_section(struct var_decl_sectionNode* varDeclSection);
void print_var_decl_list(struct var_decl_listNode* varDeclList);
void print_var_decl(struct var_declNode* varDecl);
void print_type_name(struct type_nameNode* typeName);
void print_id_list(struct id_listNode* idList);
void print_id_list(struct id_listNode* idList);

void print_stmt_list(struct stmt_listNode* stmt_list);
void print_stmt(struct stmtNode* stmt);
void print_assign_stmt(struct assign_stmtNode* assign_stmt);
void print_expression_prefix(struct exprNode* expr);
void print_while_stmt(struct while_stmtNode* while_stmt);
void print_do_stmt(struct while_stmtNode* do_stmt);
void print_condition(struct conditionNode* condition);
void print_case(struct caseNode* cas);
void print_case_list(struct case_listNode* case_list);
void print_switch_stmt(struct switch_stmtNode* switc);


/* -------------------- PARSING FUNCTIONS -------------------- */

struct programNode*           program();
struct declNode*              decl();
struct type_decl_sectionNode* type_decl_section();
struct var_decl_sectionNode*  var_decl_section();
struct type_decl_listNode*    type_decl_list();
struct var_decl_listNode*     var_decl_list();
struct var_declNode*          var_decl();
struct type_declNode*         type_decl();
struct id_listNode*           id_list();
struct type_nameNode*         type_name();
struct bodyNode*              body();
struct stmt_listNode*         stmt_list();
struct stmtNode*              stmt();
struct while_stmtNode*        while_stmt();
struct assign_stmtNode*       assign_stmt();
struct exprNode*              expr();
struct exprNode*              term();
struct exprNode*              factor();
struct conditionNode*         condition();
struct primaryNode*           primary();
struct caseNode*              cas();
struct case_listNode*         case_list();
struct switch_stmtNode*       switch_stmt();
struct while_stmtNode*        do_stmt();

#endif //__SYNTAX__H__
