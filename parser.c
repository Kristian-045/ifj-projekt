//#include "scanner.h"
//#include "parser.h"
#include "visualization.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

Token token = NULL;

int parser() {
    /*Token token = malloc(sizeof(struct Token));
    token->type = T_UNDEFINED;
    token->data = NULL;
    //one line of code
    getToken(token);
    getToken(token);
    getToken(token);
    getToken(token);
    getToken(token);
    getToken(token);
    getToken(token);
    getToken(token);

    printf("----------------\n");

    getToken(token);
    getToken(token);
    getToken(token);
    getToken(token);
    getToken(token);
    getToken(token);
    getToken(token);
    getToken(token);
    getToken(token);
    getToken(token);
    getToken(token);
    free(token);*/
    token = initToken();
    NodePtr node = initNode();
    node->data_type = ONLY_KEYWORD;
    node->keyword = START;
    node->right = process_prolog();
    node->left = process_function_list();
//    printBinaryTree(node);
    freeTree(node);
    free(token);

    return 0;
}

//now only for 1 function
NodePtr process_function_list() {
    NodePtr rootNode = initNode();
    rootNode->data_type = ONLY_KEYWORD;
    rootNode->keyword = NEW_COMMAND;
    rootNode->right = process_function();

    NodePtr lastNode = rootNode;
    while (false) {
        NodePtr node = initNode();
        node->data_type = ONLY_KEYWORD;
        node->keyword = NEW_COMMAND;
        lastNode->left = node;
        node->right = process_function();
        break;

    }
    printBinaryTree(rootNode);
    return rootNode;
}

//now only for 1 function  then it
NodePtr process_function() {
    NodePtr functionNane = initNode();

    //pub
    getToken(token);
    if (token->type != T_PUB) {
        exit(2);
    }
    // fn
    getToken(token);
    if (token->type != T_FN) {
        exit(2);
    }
    functionNane->data_type = STRING;
    functionNane->keyword = T_FN;
    // id
    getToken(token);
    if (token->type != T_ID) {
        exit(2);
    }
    functionNane->data.string_val = token->data;
    // (
    getToken(token);
    if (token->type != T_LBRACKET) {
        exit(2);
    }
    functionNane->left = process_parameter_list(true);

    NodePtr returnType = initNode();
    returnType->data_type = ONLY_KEYWORD;

    //void or type  (return type)
    getToken(token);
    if (token->type != T_VOID) {
        validateType(token);
        returnType->keyword = token->type;
    } else {
        returnType->keyword = T_VOID;
    }
    functionNane->right = returnType;

    // {
    getToken(token);
    if (token->type != T_CLBRACKET) {
        exit(2);
    }
    returnType->right = process_block();


//    process_function_list();
    return functionNane;
}

NodePtr process_parameter_list(int first) {
    NodePtr node = initNode();
    NodePtr questionNode = NULL;

    // ,
    getToken(token);
    if (!first && token->type == T_COMMA) {
        getToken(token);
    }

    // id
    if (token->type != T_ID) {
        if (token->type == T_RBRACKET)return NULL;
        else exit(2);
    }
    node->data_type = STRING;
    node->data.string_val = token->data;
    printf("after id\n");
    // :
    getToken(token);
    if (token->type != T_COLON) {
        exit(2);
    }
    printf("after :\n");
    // ?

    getToken(token);
    if (token->type == T_QUESTIONMARK) {
        questionNode = initNode();
        questionNode->data_type = ONLY_KEYWORD;
        questionNode->keyword = T_QUESTIONMARK;
        getToken(token);
    }

    // type
    validateType(token);
    printf("after type\n");


    node->keyword = token->type;
    node->left = questionNode;
    node->right = process_parameter_list(false);

    return node;
}

NodePtr process_block() {
    NodePtr node = initNode();
    node->keyword = NEW_COMMAND;
    node->data_type = ONLY_KEYWORD;
    
    getToken(token);
    switch (token->type) {
        case T_CONST:
        case T_VAR:
            node->right = process_declaration();
            break;
        case T_ID:
            node->right = process_asgmt_or_fn();
            break;
        case T_IFJ:
            //function_call
            break;
        case T_RETURN:
            node->right = process_return();
            break;
        case T_IF:
            break;
        case T_WHILE:
            break;
        case T_CRBRACKET:
            free(node);
            return NULL;
        default:
            exit(2);
    }
    node->left = process_block();
    return node;
}

NodePtr process_asgmt_or_fn() {
    NodePtr node = initNode();
    node->keyword = T_ID;
    node->data_type = STRING;
    node->data.string_val = token->data;

    getToken(token);
    switch (token->type) {
        case T_EQUALSIGN:
            NodePtr id_node = node;
            node = process_assignment();
            node->left = id_node;
            break;
        case T_LBRACKET:
            node->keyword = T_FN_CALL;
            node->left = process_function_call();
            break;
        default:
            exit(2);
    }
    return node;
}

NodePtr process_function_call() {
    NodePtr rootNode = initNode();
    rootNode->data_type = ONLY_KEYWORD;
    rootNode->keyword = T_FN_PARAM;
    rootNode->right = process_expression(2, T_COMMA, T_RBRACKET);

    NodePtr lastNode = rootNode;
    while (token->type == T_COMMA) {
        NodePtr node = initNode();
        node->data_type = ONLY_KEYWORD;
        node->keyword = T_FN_PARAM;
        node->right = process_expression(1, T_COMMA, T_RBRACKET);
        lastNode->left=node;
        lastNode=node;
    }

    // ;
    getToken(token);
    if(token->type != T_SEMICOLON){
        exit(2);
    }

//    exit(0);
    return rootNode;
}

NodePtr process_assignment() {
    NodePtr node = initNode();
    node->data_type = ONLY_KEYWORD;
    node->keyword = token->type;
    node->right = process_expression_k1(false, T_SEMICOLON);

    return node;
}

NodePtr process_declaration() {
    //save var/const
    NodePtr node = initNode();
    node->data_type = STRING;
    node->keyword = token->type;
    getToken(token);

    // id
    if (token->type != T_ID) {
        if (token->type == T_IFJ) {
            exit(5);
        } else {
            exit(2);
        }
    }
    node->data.string_val = token->data;

    // =
    getToken(token);
    if (token->type != T_EQUALSIGN) {
        exit(2);
    }
    NodePtr equalSign = initNode();
    equalSign->data_type = ONLY_KEYWORD;
    equalSign->keyword = T_EQUALSIGN;

    equalSign->left = node;

    equalSign->right = process_expression_k1(false, T_SEMICOLON);

    return equalSign;
}

NodePtr process_return() {
    NodePtr node = initNode();
    node->data_type = ONLY_KEYWORD;
    node->keyword = T_RETURN;
    node->right = process_expression_k1(true, T_SEMICOLON);

    return node;
}

NodePtr process_expression_k1(bool canBeNull, tType endKeyword) {
    process_expression(canBeNull, endKeyword, INVALID_TOKEN);
    return NULL;
}

NodePtr process_expression(int canBeNull, tType endKeyword1, tType endKeyword2) {
    int count = 0;

    getToken(token);
    while (token->type != endKeyword1 && token->type != endKeyword2) {
        getToken(token);
        count++;
    }

    printf("count: %d\n",count);
    if(canBeNull==0 && count==0){
        exit(2);
    }
    if(canBeNull==2 && count==0 && endKeyword1==token->type){
        exit(2);
    }

    return NULL;
}

NodePtr process_prolog() {
    NodePtr node = initNode();

    getToken(token);
    if (token->type != T_CONST) {
        exit(2);
    }
    node->keyword = T_CONST;

    getToken(token);
    if (token->type != T_IFJ && token->type != T_ID) {
        exit(2);
    }
    node->data_type = STRING;
    node->data.string_val = token->data;

    getToken(token);
    if (token->type != T_EQUALSIGN) {
        exit(2);
    }
    NodePtr eqNode = initNode();
    eqNode->data_type = ONLY_KEYWORD;
    eqNode->keyword = T_EQUALSIGN;
    eqNode->left = node;

    getToken(token);
    if (token->type != T_IMPORT) {
        exit(2);
    }
    NodePtr importNode = initNode();
    eqNode->right = importNode;
    importNode->data_type = ONLY_KEYWORD;
    importNode->keyword = token->type;

    getToken(token);
    if (token->type != T_LBRACKET) {
        exit(2);
    }

    getToken(token);
    if (token->type != T_STRING) {
        exit(2);
    }
    NodePtr stringNode = initNode();
    importNode->right = stringNode;
    stringNode->data_type = STRING;
    trimFirstAndLastChar(token);
    stringNode->data.string_val = token->data;

    getToken(token);
    if (token->type != T_RBRACKET) {
        exit(2);
    }

    getToken(token);
    if (token->type != T_SEMICOLON) {
        exit(2);
    }

    return eqNode;
}

NodePtr initNode() {
    NodePtr newNode = (NodePtr) malloc(sizeof(struct Node));
    if (!newNode) return NULL;
    newNode->left = NULL;
    newNode->right = NULL;

    return newNode;
}

// Function to free a node and its children
void freeTree(NodePtr root) {
    if (root == NULL) return;//return error
    freeTree(root->left);
    freeTree(root->right);

    if (root->data_type == STRING) {
        free(root->data.string_val);
    }

    free(root);
}

Token initToken() {
    Token token = malloc(sizeof(struct Token));  // Allocates memory for the Token struct
    if (!token) {
        exit(99);
    }
    token->type = T_UNDEFINED;  // Sets default type
    token->data = NULL;         // Sets default data to NULL
    return token;
}

void printTree(NodePtr root) {
    if (root == NULL) return;

    // Print left subtree
    printTree(root->left);

    // Print the current node based on its data type
    switch (root->data_type) {
        case INT:
            printf("INT: %d\n", root->data.int_val);
            break;
        case FLOAT:
            printf("DOUBLE: %f\n", root->data.float_val);
            break;
        case STRING:
            printf("STRING: %s\n", root->data.string_val);
            break;
        case ONLY_KEYWORD:
            token->type = root->keyword;
            printf("KEYWORD: %s\n", tokenToString(token));
            break;
    }

    // Print right subtree
    printTree(root->right);
}

void trimFirstAndLastChar(Token token) {
    if (token->data) {
        size_t len = strlen(token->data);
        if (len >= 2) {
            memmove(token->data, token->data + 1, len - 2);
            token->data[len - 2] = '\0';
        }
    }
}

void validateType(Token token) {
    if (token->type != T_I32 && token->type != T_F64) {
        if (token->type != T_SLBRACKET) {
            exit(2);
        }
        getToken(token);
        if (token->type != T_SRBRACKET) {
            exit(2);
        }
        getToken(token);
        if (token->type != T_U8) {
            exit(2);
        }
    }
}