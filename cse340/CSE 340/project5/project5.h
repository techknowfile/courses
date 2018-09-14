// MACROS
#define ALLOC(t) (t*) calloc(1, sizeof(t))

// Forward Declarations
struct StatementNode* parse_generate_intermediate_representation();
void parse_var_section();
void parse_id_list();
struct StatementNode* parse_body();
struct StatementNode* parse_stmt_list();
struct StatementNode* parse_stmt();
struct AssignmentStatement* parse_assign_stmt();
struct ValueNode* parse_id();
struct ValueNode* parse_primary();
struct PrintStatement* parse_print_stmt();
struct StatementNode* get_last_statement_node(struct StatementNode* stmt);
void parse_condition(struct IfStatement* condition);
struct IfStatement* parse_if_stmt();
struct StatementNode* parse_switch();
struct StatementNode* parse_case_list(struct ValueNode* val, struct StatementNode* noop_node);
struct StatementNode* parse_case(struct ValueNode* val);
struct ValueNode* num2ValNode();