#include <stdlib.h>

#ifndef IFJ_PROJEKT_PARSER_H
#define IFJ_PROJEKT_PARSER_H

// Enumeration for data types
typedef enum {
    INT,
    FLOAT,
    STRING,
    ONLY_KEYWORD
} DataType;

// Union for holding different data types
typedef union {
    int int_val;
    double float_val;
    char *string_val;
} DataValue;

// Structure for a binary tree node
typedef struct Node {
    DataType data_type;        // Type of data in the node
    DataValue data;            // The actual data, held as a union
    tType keyword;
    struct Node *left;
    struct Node *right;
} *NodePtr;

//
//  for expression parser
//
typedef enum {
    P_S,  // Shift (<)
    P_R,  // Reduce (>)
    P_E,  // Equal (=)
    P_X,  // Error
    P_END //end of expression
} Precedence;

// Stack node structures for both types
typedef struct TokenStackItem {
    Token token;
    struct TokenStackItem *next;
} TokenStackItem;

typedef struct NodeStackItem {
    NodePtr node;
    struct NodeStackItem *next;
} NodeStackItem;

// Stack structures
typedef struct {
    TokenStackItem *top;
} TokenStack;

typedef struct {
    NodeStackItem *top;
} NodeStack;


// Updated precedence table based on the provided image
static const Precedence precedenceTable[14][14] = {
        //  *    /    +    -    ==   !=   <    >    <=   >=   (    )    i    $
        {P_R, P_R, P_R, P_R, P_R, P_R, P_R, P_R, P_R, P_R, P_S, P_R, P_S, P_R}, // *
        {P_R, P_R, P_R, P_R, P_R, P_R, P_R, P_R, P_R, P_R, P_S, P_R, P_S, P_R}, // /
        {P_S, P_S, P_R, P_R, P_R, P_R, P_R, P_R, P_R, P_R, P_S, P_R, P_S, P_R}, // +
        {P_S, P_S, P_R, P_R, P_R, P_R, P_R, P_R, P_R, P_R, P_S, P_R, P_S, P_R}, // -
        {P_S, P_S, P_S, P_S, P_X, P_X, P_X, P_X, P_X, P_X, P_S, P_R, P_S, P_R}, // ==
        {P_S, P_S, P_S, P_S, P_X, P_X, P_X, P_X, P_X, P_X, P_S, P_R, P_S, P_R}, // !=
        {P_S, P_S, P_S, P_S, P_X, P_X, P_X, P_X, P_X, P_X, P_S, P_R, P_S, P_R}, // <
        {P_S, P_S, P_S, P_S, P_X, P_X, P_X, P_X, P_X, P_X, P_S, P_R, P_S, P_R}, // >
        {P_S, P_S, P_S, P_S, P_X, P_X, P_X, P_X, P_X, P_X, P_S, P_R, P_S, P_R}, // <=
        {P_S, P_S, P_S, P_S, P_X, P_X, P_X, P_X, P_X, P_X, P_S, P_R, P_S, P_R}, // >=
        {P_S, P_S, P_S, P_S, P_S, P_S, P_S, P_S, P_S, P_S, P_S, P_E, P_S, P_X}, // (
        {P_R, P_R, P_R, P_R, P_R, P_R, P_R, P_R, P_R, P_R, P_X, P_R, P_X, P_R}, // )
        {P_R, P_R, P_R, P_R, P_R, P_R, P_R, P_R, P_R, P_R, P_X, P_R, P_X, P_R}, // i
        {P_S, P_S, P_S, P_S, P_S, P_S, P_S, P_S, P_S, P_S, P_S, P_X, P_S, P_END}  // $
};





// Function to free a node and its children
void free_tree(NodePtr root);


NodePtr parser();

NodePtr process_prolog();

NodePtr process_function_list();

NodePtr process_function();

NodePtr process_parameter_list(int first);

NodePtr process_block();

NodePtr process_return();

NodePtr process_expression_one_keyword(bool canBeNull, tType endKeyword);

NodePtr process_expression(int canBeNull, tType endKeyword1, tType endKeyword2);

NodePtr process_declaration();

NodePtr process_asgmt_or_fn();

NodePtr process_assignment();

NodePtr process_function_call_arguments();

NodePtr process_if();

NodePtr process_else();

NodePtr process_while();

NodePtr process_id_without_null();

NodePtr process_ifj_call();

NodePtr process_type();

Token init_token();

NodePtr init_node();

void validate_type();

#endif

//------------------------
//  For expression parser
//------------------------

// Token Stack Operations
TokenStack* create_token_stack();
void token_stack_push(TokenStack* stack, Token pushToken);
Token token_stack_pop(TokenStack* stack);
Token token_stack_top(TokenStack* stack);
void free_token_stack(TokenStack* stack);

// Node Stack Operations
NodeStack* create_node_stack();
void node_stack_push(NodeStack* stack, NodePtr node);
NodePtr node_stack_pop(NodeStack* stack);
NodePtr node_stack_top(NodeStack* stack);
void free_node_stack(NodeStack* stack);

// Expression Parsing Functions
int get_precedence_index(tType token);
Precedence get_action(tType topToken, tType inputToken);
NodePtr create_value_node(Token token);
NodePtr create_operator_node(Token operator, NodePtr left, NodePtr right);
NodePtr parse_expression();

