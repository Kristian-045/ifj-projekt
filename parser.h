/**
 * @file parser.h
 * @brief Header file for the parser module of the IFJ project.
 *
 * @author Kristián Kaleta (xkaletk00)
 *
 * @date 2024
 */
#include <stdlib.h>

#ifndef IFJ_PROJEKT_PARSER_H
#define IFJ_PROJEKT_PARSER_H




/**
 * @enum DataType
 * @brief Enumeration for which data tree have.
 */
typedef enum {
    INT,          // Integer data type
    FLOAT,        // Floating-point data type
    STRING,       // String data type
    ONLY_KEYWORD  // Represents a keyword without associated data
} DataType;

/**
* @union DataValue
* @brief Union for holding different data types.
*/
typedef union {
    int int_val;        // Integer value
    double float_val;   // Floating-point value
    char *string_val;   // String value
} DataValue;

/**
 * @struct Node
 * @brief Structure for a binary tree node.
 */
typedef struct Node {
    DataType data_type;
    DataValue data;
    tType keyword;
    struct Node *left;
    struct Node *right;
} *NodePtr;

/**
 * @enum Precedence
 * @brief Precedence enumeration for expression parsing.
 */
typedef enum {
    P_S,  // Shift (<)
    P_R,  // Reduce (>)
    P_E,  // Equal (=)
    P_X,  // Error
    P_END // End of expression
} Precedence;

/**
 * @struct TokenStackItem
 * @brief Stack node structure for tokens.
 */
typedef struct TokenStackItem {
    Token token;
    struct TokenStackItem *next;
} TokenStackItem;

/**
 * @struct NodeStackItem
 * @brief Stack node structure for nodes.
 */
typedef struct NodeStackItem {
    NodePtr node;
    struct NodeStackItem *next;
} NodeStackItem;

/**
 * @struct TokenStack
 * @brief Stack structure for tokens.
 */
typedef struct {
    TokenStackItem *top;
} TokenStack;

/**
 * @struct NodeStack
 * @brief Stack structure for nodes.
 */
typedef struct {
    NodeStackItem *top;
} NodeStack;

/**
 * @brief Precedence table for expression parsing.
 * A 14x14 table that defines the precedence of operators and actions during parsing.
 */
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
/**
 * @brief Creates a new empty token stack.
 *
 * @return TokenStack* - Pointer to the newly created token stack.
 */
TokenStack* create_token_stack();

/**
 * @brief Pushes a token onto the token stack.
 *
 * @param stack - The token stack to push the token onto.
 * @param pushToken - The token to push onto the stack.
 */
void token_stack_push(TokenStack* stack, Token pushToken);

/**
 * @brief Pops a token from the top of the token stack.
 *
 * @param stack - The token stack from which to pop the token.
 * @return Token - The token popped from the stack.
 */
Token token_stack_pop(TokenStack* stack);

/**
 * @brief Returns the token from the top of the token stack without removing it.
 *
 * @param stack - The token stack to peek at.
 * @return Token - The token at the top of the stack.
 */
Token token_stack_top(TokenStack* stack);

/**
 * @brief Frees the memory used by the token stack and its items.
 *
 * @param stack - The token stack to free.
 */
void free_token_stack(TokenStack* stack);

// Node stack operations
/**
 * @brief Creates a new empty node stack.
 *
 * @return NodeStack* - Pointer to the newly created node stack.
 */
NodeStack* create_node_stack();

/**
 * @brief Pushes a node onto the node stack.
 *
 * @param stack - The node stack to push the node onto.
 * @param node - The node to push onto the stack.
 */
void node_stack_push(NodeStack* stack, NodePtr node);

/**
 * @brief Pops a node from the top of the node stack.
 *
 * @param stack - The node stack from which to pop the node.
 * @return NodePtr - The node popped from the stack.
 */
NodePtr node_stack_pop(NodeStack* stack);

/**
 * @brief Returns the node from the top of the node stack without removing it.
 *
 * @param stack - The node stack to peek at.
 * @return NodePtr - The node at the top of the stack.
 */
NodePtr node_stack_top(NodeStack* stack);

/**
 * @brief Frees the memory used by the node stack and its items.
 *
 * @param stack - The node stack to free.
 */
void free_node_stack(NodeStack* stack);

// Expression parsing functions
/**
 * @brief Returns the precedence index for a given token.
 *
 * @param token - The token whose precedence index is to be determined.
 * @return int - The precedence index corresponding to the token.
 */
int get_precedence_index(tType token);

/**
 * @brief Determines the action to be taken based on the precedence of two tokens.
 *
 * @param topToken - The token at the top of the stack.
 * @param inputToken - The incoming token to be processed.
 * @return Precedence - The action based on the precedence comparison.
 */
Precedence get_action(tType topToken, tType inputToken);

/**
 * @brief Creates a syntax tree node for a value token (e.g., integers, floats, strings).
 *
 * @param token - The token representing the value to be encapsulated in a node.
 * @return NodePtr - A pointer to the newly created node.
 */
NodePtr create_value_node(Token token);

/**
 * @brief Creates a syntax tree node for an operator.
 *
 * @param operator - The token representing the operator.
 * @param left - Pointer to the left child node.
 * @param right - Pointer to the right child node.
 * @return NodePtr - A pointer to the newly created operator node.
 */
NodePtr create_operator_node(Token operator, NodePtr left, NodePtr right);

/**
 * @brief Parses an expression and constructs its syntax tree.
 *
 * @return NodePtr - Pointer to the root of the syntax tree.
 *
 * @details Implements a precedence-based parser using two stacks (for tokens and nodes).
 */
NodePtr parse_expression();

#endif
