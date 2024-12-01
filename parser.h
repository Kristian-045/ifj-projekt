#include <stdlib.h>

#ifndef IFJ_PROJEKT_PARSER_H
#define IFJ_PROJEKT_PARSER_H

/**
 * @file parser.h
 * @brief Header file for the parser module of the IFJ project.
 *
 * @authors
 * - Kristián Kaleta (xkaletk00)
 *
 * @date 2024
 */



// Enumeration for data types
typedef enum {
    INT,          // Integer data type
    FLOAT,        // Floating-point data type
    STRING,       // String data type
    ONLY_KEYWORD  // Represents a keyword without associated data
} DataType;

// Union for holding different data types
typedef union {
    int int_val;        // Integer value
    double float_val;   // Floating-point value
    char *string_val;   // String value
} DataValue;

// Structure for a binary tree node
typedef struct Node {
    DataType data_type;
    DataValue data;
    tType keyword;
    struct Node *left;
    struct Node *right;
} *NodePtr;

// Precedence enumeration for expression parsing
typedef enum {
    P_S,  // Shift (<)
    P_R,  // Reduce (>)
    P_E,  // Equal (=)
    P_X,  // Error
    P_END // End of expression
} Precedence;

// Stack node structures for tokens and nodes
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

// Precedence table
static const Precedence precedenceTable[14][14] = {
        //*    /    +    -    ==   !=   <    >    <=   >=   (    )    i    $
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
        {P_S, P_S, P_S, P_S, P_S, P_S, P_S, P_S, P_S, P_S, P_S, P_X, P_S, P_END} // $
};

/**
 * @brief Frees the memory used by a syntax tree.
 * @param root Pointer to the root node of the tree to be freed.
 */
void free_tree(NodePtr root);

/**
 * @brief Parses the Zig code into a syntax tree.
 * @return Pointer to the root node of the syntax tree.
 */
NodePtr parser();

/**
 * @brief Processes the prolog section of the Zig code.
 * @return Pointer to the root node representing the prolog.
 */
NodePtr process_prolog();

/**
 * @brief Processes the list of functions in the Zig code.
 * @return Pointer to the root node representing the function list.
 */
NodePtr process_function_list();

/**
 * @brief Processes a single function in the Zig code.
 * @return Pointer to the root node representing the function.
 */
NodePtr process_function();

/**
 * @brief Processes a parameter list in a function declaration.
 * @param first Indicates if this is the first parameter.
 * @return Pointer to the root node of the parameter list subtree.
 */
NodePtr process_parameter_list(int first);

/**
 * @brief Processes a block of code.
 * @return Pointer to the root node representing the block.
 */
NodePtr process_block();

/**
 * @brief Processes a return statement.
 * @return Pointer to the root node representing the return statement.
 */
NodePtr process_return();

/**
 * @brief Processes an expression ending with a specific keyword.
 * @param canBeNull Indicates if the expression can be null.
 * @param endKeyword Ending keyword of the expression.
 * @return Pointer to the root node representing the expression.
 */
NodePtr process_expression_one_keyword(bool canBeNull, tType endKeyword);

/**
 * @brief Processes a general expression.
 * @param canBeNull Indicates nullability of the expression.
 * @param endKeyword1 First ending keyword of the expression.
 * @param endKeyword2 Second ending keyword of the expression.
 * @return Pointer to the root node representing the expression.
 */
NodePtr process_expression(int canBeNull, tType endKeyword1, tType endKeyword2);

/**
 * @brief Processes a variable or constant declaration.
 * @return Pointer to the root node representing the declaration.
 */
NodePtr process_declaration();

/**
 * @brief Processes an assignment or a function call.
 * @return Pointer to the root node representing the assignment or call.
 */
NodePtr process_asgmt_or_fn();

/**
 * @brief Processes an assignment statement.
 * @return Pointer to the root node representing the assignment.
 */
NodePtr process_assignment();

/**
 * @brief Processes arguments of a function call.
 * @return Pointer to the root node representing the arguments.
 */
NodePtr process_function_call_arguments();

/**
 * @brief Processes an "if" statement.
 * @return Pointer to the root node representing the "if" statement.
 */
NodePtr process_if();

/**
 * @brief Processes an "else" statement.
 * @return Pointer to the root node representing the "else" statement.
 */
NodePtr process_else();

/**
 * @brief Processes a "while" loop.
 * @return Pointer to the root node representing the "while" loop.
 */
NodePtr process_while();

/**
 * @brief Processes an identifier inside a pipe expression.
 * @return Pointer to the node representing the identifier.
 */
NodePtr process_id_without_null();

/**
 * @brief Processes a built-in IFJ function call.
 * @return Pointer to the root node representing the function call.
 */
NodePtr process_ifj_call();

/**
 * @brief Processes a type declaration.
 * @return Pointer to the node representing the type declaration.
 */
NodePtr process_type();

/**
 * @brief Initializes a new token.
 * @return Pointer to the initialized token.
 */
Token init_token();

/**
 * @brief Initializes a new syntax tree node.
 * @return Pointer to the initialized node.
 */
NodePtr init_node();

/**
 * @brief Validates the type of the current token.
 */
void validate_type();

// Token stack operations
TokenStack* create_token_stack();
void token_stack_push(TokenStack* stack, Token pushToken);
Token token_stack_pop(TokenStack* stack);
Token token_stack_top(TokenStack* stack);
void free_token_stack(TokenStack* stack);

// Node stack operations
NodeStack* create_node_stack();
void node_stack_push(NodeStack* stack, NodePtr node);
NodePtr node_stack_pop(NodeStack* stack);
NodePtr node_stack_top(NodeStack* stack);
void free_node_stack(NodeStack* stack);

// Expression parsing functions
int get_precedence_index(tType token);
Precedence get_action(tType topToken, tType inputToken);
NodePtr create_value_node(Token token);
NodePtr create_operator_node(Token operator, NodePtr left, NodePtr right);
NodePtr parse_expression();

#endif
