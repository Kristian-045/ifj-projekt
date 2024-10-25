

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

Token initToken();

NodePtr initNode();

void printTree(NodePtr root);

void trimFirstAndLastChar(Token token);

void validateType(Token token);