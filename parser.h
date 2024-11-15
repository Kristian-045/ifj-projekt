

#include <stdlib.h>

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
//    int isConst;
    struct Node *left;
    struct Node *right;
} *NodePtr;

typedef enum {
    P_S,  // Shift (<)
    P_R,  // Reduce (>)
    P_E,  // Equal (=)
    P_X,  // Error
    P_END //end of expression
} Precedence;


// Function to create a new node
//NodePtr createNode(DataType type, DataValue data);

// Function to free a node and its children
void freeTree(NodePtr root);


int parser();

NodePtr process_prolog();

NodePtr process_function_list();

NodePtr process_function();

NodePtr process_parameter_list(int first);

NodePtr process_block();

NodePtr process_return();

NodePtr process_expression_k1(bool canBeNull, tType endKeyword);

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


Token initToken();

NodePtr initNode();

void printTree(NodePtr root);

void trimFirstAndLastChar(Token token);

void validateType();

//------------------------
//  For expression parser
//------------------------

// Get precedence level for token
Precedence getTokenPrecedence();

// Create node for values
NodePtr createValueNode();

// Create node for operators
NodePtr createOperatorNode(Token operator, NodePtr left, NodePtr right);

// Parse an expression with a given precedence level
NodePtr parseExpression();
