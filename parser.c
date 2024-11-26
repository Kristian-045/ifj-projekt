#include "scanner.h"
#include "parser.h"
#include "visualization.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

Token token = NULL;
NodePtr lastProcessedNode = NULL;
int nextTokenMustBeElse = 0;

NodePtr parser() {
    /*Token token = malloc(sizeof(struct Token));
    token->type = T_UNDEFINED;
    token->data = NULL;
    //one line of code
    get_token(token);
    get_token(token);
    get_token(token);
    get_token(token);
    get_token(token);
    get_token(token);
    get_token(token);
    get_token(token);

    printf("----------------\n");

    get_token(token);
    get_token(token);
    get_token(token);
    get_token(token);
    get_token(token);
    get_token(token);
    get_token(token);
    get_token(token);
    get_token(token);
    get_token(token);
    get_token(token);
    free(token);*/
    token = initToken();
    NodePtr node = initNode();
    node->data_type = ONLY_KEYWORD;
    node->keyword = START;
    node->right = process_prolog();
    node->left = process_function_list();
//    printBinaryTree(node->left->right->right->right);


    free(token);
    return node;



    return 0;
}

//now only for 1 function
NodePtr process_function_list() {
    NodePtr rootNode = initNode();
    rootNode->data_type = ONLY_KEYWORD;
    rootNode->keyword = NEW_COMMAND;
    rootNode->right = process_function();
    NodePtr lastNode = rootNode;

    while (lastNode->right != NULL) {
        NodePtr node = initNode();
        node->data_type = ONLY_KEYWORD;
        node->keyword = NEW_COMMAND;

        lastNode->left = node;
        node->right = process_function();
        lastNode = node;


    }
//    printBinaryTree(rootNode);
    return rootNode;
}

//now only for 1 function  then it
NodePtr process_function() {
    NodePtr functionName = initNode();

    NodePtr dataFn = initNode();
    dataFn->data_type = ONLY_KEYWORD;
    dataFn->keyword = FN_DATA;
    functionName->left = dataFn;

    //pub
    get_token(token);
    if (token->type != T_PUB) {
        if (token->type == T_EOF) {
            return NULL;
        }
        fprintf(stderr,"Expected pub\n");
        exit(2);
    }
    // fn
    get_token(token);
    if (token->type != T_FN) {
        fprintf(stderr,"Expected fn\n");
        exit(2);
    }
    functionName->data_type = STRING;
    functionName->keyword = T_FN;
    // id
    get_token(token);
    if (token->type != T_ID) {
        fprintf(stderr,"Expected id\n");
        exit(2);
    }
    functionName->data.string_val = token->data;
    // (
    get_token(token);
    if (token->type != T_LBRACKET) {
        fprintf(stderr,"Expected (\n");
        exit(2);
    }
    dataFn->left = process_parameter_list(true);


    NodePtr returnType = initNode();
    returnType->data_type = ONLY_KEYWORD;

    NodePtr questionNode = NULL;
    //?
    get_token(token);
    if (token->type == T_QUESTIONMARK) {
        questionNode = initNode();
        questionNode->data_type = ONLY_KEYWORD;
        questionNode->keyword = T_QUESTIONMARK;
        get_token(token);
    }
    //void or type  (return type)
    if (token->type != T_VOID) {
        validateType();
        returnType->keyword = token->type;
    } else {
        if (questionNode != NULL) {
            fprintf(stderr,"Unexpected ?\n");
            exit(2);
        }
        returnType->keyword = T_VOID;
    }

    returnType->left = questionNode;
    dataFn->right = returnType;

    // {
    get_token(token);
    if (token->type != T_CLBRACKET) {
        if (token->type == T_SEMICOLON){
            fprintf(stderr,"Function without body\n");
            exit(3);
        }
        fprintf(stderr,"Expected {\n");
        exit(2);
    }
    functionName->right = process_block();

//    printBinaryTree(functionName);
    return functionName;
}

NodePtr process_parameter_list(int first) {
    NodePtr node = initNode();
    NodePtr questionNode = NULL;

    // ,
    get_token(token);
    if (!first && token->type == T_COMMA) {
        get_token(token);
    }

    // id
    if (token->type != T_ID) {
        if (token->type == T_RBRACKET)return NULL;
        else {
            fprintf(stderr,"Invalid syntax in function list\n");
            exit(2);
        }
    }
    node->data_type = STRING;
    node->data.string_val = token->data;
    // :
    get_token(token);
    if (token->type != T_COLON) {
        fprintf(stderr,"Expected :\n");
        exit(2);
    }

    // ?
    get_token(token);
    if (token->type == T_QUESTIONMARK) {
        questionNode = initNode();
        questionNode->data_type = ONLY_KEYWORD;
        questionNode->keyword = T_QUESTIONMARK;
        get_token(token);
    }

    // type
    validateType();

    node->keyword = token->type;
    node->left = questionNode;
    node->right = process_parameter_list(false);

    return node;
}

NodePtr process_block() {
//    printf("inside block\n");
    NodePtr node = initNode();
    node->keyword = NEW_COMMAND;
    node->data_type = ONLY_KEYWORD;
    get_token(token);

    if (nextTokenMustBeElse==1 && token->type != T_ELSE){
        fprintf(stderr,"Expected else\n");
        exit(2);
    }
    nextTokenMustBeElse=0;

    switch (token->type) {
        case T_CONST:
        case T_VAR:
            // declaration
            node->right = process_declaration();
            break;
        case T_ID:
            // assignment or function call
            node->right = process_asgmt_or_fn();
            break;
        case T_IFJ:
            // ifj function call
            node->right = process_ifj_call();
            break;
        case T_RETURN:
            node->right = process_return();
            break;
        case T_IF:
            node->right = process_if();
            nextTokenMustBeElse=1;
            break;
        case T_ELSE:
            //if last node processed was if
            if (lastProcessedNode->right->keyword != T_IF) {
                fprintf(stderr,"Else can be only after if\n");
                exit(2);
            }
            node->right = process_else();
//            printBinaryTree(node);
            break;
        case T_WHILE:
            node->right = process_while();
            break;
        case T_CRBRACKET:
            free(node);
            return NULL;

        default:
            fprintf(stderr,"Invalid syntax in body\n");
            exit(2);
    }
    lastProcessedNode = node;
    node->left = process_block();
    return node;
}

NodePtr process_ifj_call() {
    NodePtr node = initNode();
    node->data_type = ONLY_KEYWORD;
    node->keyword = token->type;

    // .
    get_token(token);
    if (token->type != T_DOT) {
        fprintf(stderr,"Expected .\n");
        exit(2);
    }
    // ID
    get_token(token);
    if (token->type != T_ID) {
        fprintf(stderr,"Expected id\n");
        exit(2);
    }
    NodePtr functionName = initNode();
    functionName->data_type = STRING;
    functionName->data.string_val = token->data;
    functionName->keyword = token->type;

    // (
    get_token(token);
    if (token->type != T_LBRACKET) {
        fprintf(stderr,"Expected (\n");
        exit(2);
    }
    node->left = functionName;
    node->right = process_function_call_arguments();

    // ;
    get_token(token);
    if (token->type != T_SEMICOLON) {
        fprintf(stderr,"Expected ;\n");
        exit(2);
    }

//    printBinaryTree(node);
    return node;
}

NodePtr process_while() {
    NodePtr whileNode = initNode();
    whileNode->data_type = ONLY_KEYWORD;
    whileNode->keyword = token->type;

    NodePtr whileDataNode = initNode();
    whileDataNode->data_type = ONLY_KEYWORD;
    whileDataNode->keyword = WHILE_DATA;

    whileNode->left = whileDataNode;

    // (
    get_token(token);
    if (token->type != T_LBRACKET) {
        fprintf(stderr,"Expected (\n");
        exit(2);
    }

    whileDataNode->left = process_expression_k1(false, T_RBRACKET);
    whileDataNode->right = process_id_without_null();

    //{
    if (token->type != T_CLBRACKET) {
        fprintf(stderr,"Expected {\n");
        exit(2);
    }
    whileNode->right = process_block();

//    printBinaryTree(whileNode);

    return whileNode;
}

NodePtr process_if() {
    NodePtr ifNode = initNode();
    ifNode->data_type = ONLY_KEYWORD;
    ifNode->keyword = token->type;

    NodePtr ifDataNode = initNode();
    ifDataNode->data_type = ONLY_KEYWORD;
    ifDataNode->keyword = IF_DATA;

    ifNode->left = ifDataNode;

    // (
    get_token(token);
    if (token->type != T_LBRACKET) {
        fprintf(stderr,"Expected (\n");
        exit(2);
    }

    ifDataNode->left = process_expression_k1(false, T_RBRACKET);
    ifDataNode->right = process_id_without_null();

    //{
    if (token->type != T_CLBRACKET) {
        fprintf(stderr,"Expected {\n");
        exit(2);
    }

    ifNode->right = process_block();;

//    printBinaryTree(ifNode);
    return ifNode;
}

NodePtr process_else() {
    NodePtr elseNode = initNode();
    elseNode->data_type=ONLY_KEYWORD;
    elseNode->keyword=T_ELSE;
    //{
    get_token(token);
    if (token->type != T_CLBRACKET) {
        fprintf(stderr,"Expected {\n");
        exit(2);
    }
    elseNode->right = process_block();
    return elseNode;
}

NodePtr process_id_without_null() {
    // |
    get_token(token);
    if (token->type != T_PIPE) {
        return NULL;
    }

    // ID
    get_token(token);
    if (token->type != T_ID) {
        fprintf(stderr,"Expected id\n");
        exit(2);
    }
    NodePtr node = initNode();
    node->keyword = T_ID;
    node->data_type = STRING;
    node->data.string_val = token->data;

    // |
    get_token(token);
    if (token->type != T_PIPE) {
        fprintf(stderr,"Expected |\n");
        exit(2);
    }


    get_token(token);
    return node;
}

NodePtr process_asgmt_or_fn() {
    NodePtr node = initNode();
    node->keyword = T_ID;
    node->data_type = STRING;
    node->data.string_val = token->data;

    get_token(token);
    switch (token->type) {
        case T_EQUALSIGN:{
            NodePtr id_node = node;
            node = process_assignment();
            node->left = id_node;
            break;
        }
        case T_LBRACKET:
            node->keyword = FN_CALL;
            node->left = process_function_call_arguments();
            // ;
            get_token(token);
            if (token->type != T_SEMICOLON) {
                fprintf(stderr,"Expected ;\n");
                exit(2);
            }
            break;
        default:
            fprintf(stderr,"invalid token after id\n");
            exit(2);
    }
//    printBinaryTree(node);
    return node;
}

NodePtr process_function_call_arguments() {
    //processing arguments
    NodePtr rootNode = initNode();

    NodePtr expNode = process_expression(2, T_COMMA, T_RBRACKET);
    if (expNode != NULL) {
        rootNode->data_type = ONLY_KEYWORD;
        rootNode->keyword = FN_PARAM;
        rootNode->right = expNode;

    }

    NodePtr lastNode = rootNode;
    while (token->type == T_COMMA) {
        tType lastKeyword = token->type;

        expNode = process_expression(1, T_COMMA, T_RBRACKET);

        // ,, for 2 commas without param
        if (lastKeyword == T_COMMA && token->type == T_COMMA && expNode == NULL) {
            fprintf(stderr,"2 commas without param\n");
            exit(2);
        }
        // ,) avoid making new node when after , is )
        if (lastKeyword == T_COMMA && token->type == T_RBRACKET && expNode == NULL) {
            break;
        }
        NodePtr node = initNode();
        node->data_type = ONLY_KEYWORD;
        node->keyword = FN_PARAM;
        node->right = expNode;

        lastNode->left = node;
        lastNode = node;
    }


    if (rootNode->right == NULL) {
        freeTree(rootNode);
        return NULL;
    }
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


    get_token(token);

    // id
    if (token->type != T_ID) {
        if (token->type == T_IFJ) {
            fprintf(stderr,"Redeclared variable\n");
            exit(5);
        } else {
            fprintf(stderr,"Expected id\n");
            exit(2);
        }
    }
    node->data.string_val = token->data;

    // : can be something else
    get_token(token);
    if (token->type == T_COLON) {
        node->left = process_type();
        get_token(token);
    }

    // =
    if (token->type != T_EQUALSIGN) {
        fprintf(stderr,"Expected =\n");
        exit(2);
    }
    NodePtr equalSign = initNode();
    equalSign->data_type = ONLY_KEYWORD;
    equalSign->keyword = T_EQUALSIGN;

    equalSign->left = node;

    equalSign->right = process_expression_k1(false, T_SEMICOLON);

    //(equalSign);
//    printBinaryTree(equalSign->right);
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
    return process_expression(canBeNull, endKeyword, INVALID_TOKEN);
}

NodePtr process_expression(int canBeNull, tType endKeyword1, tType endKeyword2) {

    NodePtr expression = parseExpression();

    if (canBeNull == 0 && expression == NULL) {
        fprintf(stderr,"Expression cant be null\n");
        exit(2);
    }

    if (canBeNull == 2 && expression == NULL && endKeyword1 == token->type) {
        fprintf(stderr,"Expression cant be null\n");
        exit(2);
    }

    if (token->type != endKeyword1 && token->type != endKeyword2) {
        fprintf(stderr,"Unexpected token in expression\n");
        exit(2);
    }

    return expression;
}

NodePtr process_prolog() {
    NodePtr node = initNode();

    get_token(token);
    if (token->type != T_CONST) {
        fprintf(stderr,"Expected const\n");
        exit(2);
    }
    node->keyword = T_CONST;

    get_token(token);
//    if (token->type != T_IFJ && token->type != T_ID) {
    if (token->type != T_IFJ ) {
        fprintf(stderr,"Expected ifj\n");
        exit(2);
    }
    node->data_type = STRING;
    node->data.string_val = token->data;

    get_token(token);
    if (token->type != T_EQUALSIGN) {
        fprintf(stderr,"Expected =\n");
        exit(2);
    }
    NodePtr eqNode = initNode();
    eqNode->data_type = ONLY_KEYWORD;
    eqNode->keyword = T_EQUALSIGN;
    eqNode->left = node;

    get_token(token);
    if (token->type != T_IMPORT) {
        fprintf(stderr,"Expected import\n");
        exit(2);
    }
    NodePtr importNode = initNode();
    eqNode->right = importNode;
    importNode->data_type = ONLY_KEYWORD;
    importNode->keyword = token->type;

    get_token(token);
    if (token->type != T_LBRACKET) {
        fprintf(stderr,"Expected (\n");
        exit(2);
    }

    get_token(token);
    if (token->type != T_STRING) {
        fprintf(stderr,"Expected string\n");
        exit(2);
    }
    NodePtr stringNode = initNode();
    importNode->right = stringNode;
    stringNode->data_type = STRING;
    trimFirstAndLastChar(token);
    stringNode->data.string_val = token->data;

    get_token(token);
    if (token->type != T_RBRACKET) {
        fprintf(stderr,"Expected )\n");
        exit(2);
    }

    get_token(token);
    if (token->type != T_SEMICOLON) {
        fprintf(stderr,"Expected ;\n");
        exit(2);
    }

    return eqNode;
}

NodePtr process_type() {

    NodePtr node = initNode();
    NodePtr questionNode = NULL;

    get_token(token);
    if (token->type == T_QUESTIONMARK) {
        questionNode = initNode();
        questionNode->data_type = ONLY_KEYWORD;
        questionNode->keyword = T_QUESTIONMARK;
        get_token(token);
    }

    // type
    validateType();

    node->data_type = ONLY_KEYWORD;
    node->keyword = token->type;
    node->left = questionNode;

    return node;
}

NodePtr initNode() {
    NodePtr newNode = (NodePtr) malloc(sizeof(struct Node));
    if (newNode==NULL) {
        fprintf(stderr,"Memory allocation error\n");
        exit(99);
    }
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
    Token newToken = malloc(sizeof(struct Token));  // Allocates memory for the Token struct
    if (newToken==NULL) {
        fprintf(stderr,"Memory allocation error\n");
        exit(99);
    }
    newToken->type = T_UNDEFINED;  // Sets default type
    newToken->data = NULL;         // Sets default data to NULL
    return newToken;
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

void validateType() {
    if (token->type != T_I32 && token->type != T_F64) {
        // for []u8
        // [
        if (token->type != T_SLBRACKET) {
            fprintf(stderr,"Invalid type\n");
            exit(2);
        }
        // ]
        get_token(token);
        if (token->type != T_SRBRACKET) {
            fprintf(stderr,"Invalid type\n");
            exit(2);
        }
        // u8
        get_token(token);
        if (token->type != T_U8) {
            fprintf(stderr,"Invalid type\n");
            exit(2);
        }
    }
}


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


// Token Stack operations
TokenStack *createTokenStack() {
    TokenStack *stack = (TokenStack *) malloc(sizeof(TokenStack));
    if (stack == NULL) {
        fprintf(stderr,"Memory allocation error\n");
        exit(99);
    }
    stack->top = NULL;
    return stack;
}

void tokenStackPush(TokenStack *stack, Token pushToken) {
    if (stack == NULL) return;
    TokenStackItem *item = (TokenStackItem *) malloc(sizeof(TokenStackItem));
    if (item == NULL) {
        fprintf(stderr,"Memory allocation error\n");
        exit(99);
    };
    item->token = initToken();
    item->token->type = pushToken->type;
    item->token->data = pushToken->data;
    item->next = stack->top;
    stack->top = item;
}

Token tokenStackPop(TokenStack *stack) {
    if (stack == NULL || stack->top == NULL) {
        Token nullToken = initToken();
        token->type=STACK_END;
        return nullToken;
    }
    TokenStackItem *topItem = stack->top;
    Token popToken = topItem->token;
    stack->top = topItem->next;
    free(topItem);
    return popToken;
}

Token tokenStackTop(TokenStack *stack) {
    if (stack == NULL || stack->top == NULL) {
        Token nullToken = initToken();
        nullToken->type = STACK_END;
        return nullToken;
    }
    return stack->top->token;
}

int isTokenStackEmpty(TokenStack *stack) {
    return (stack == NULL || stack->top == NULL);
}

void freeTokenStack(TokenStack *stack) {
    if (stack == NULL) return;
    while (stack->top != NULL) {
        TokenStackItem *temp = stack->top;
        stack->top = stack->top->next;
        free(temp);
    }
    free(stack);
}

// NodePtr Stack operations
NodeStack *createNodeStack() {
    NodeStack *stack = (NodeStack *) malloc(sizeof(NodeStack));
    if (stack == NULL) {
        fprintf(stderr,"Memory allocation error\n");
        exit(99);
    }
    stack->top = NULL;
    return stack;
}

void nodeStackPush(NodeStack *stack, NodePtr node) {
    if (stack == NULL) return;
    NodeStackItem *item = (NodeStackItem *) malloc(sizeof(NodeStackItem));
    if (item == NULL) {
        fprintf(stderr,"Memory allocation error\n");
        exit(99);
    }
    item->node = node;

    item->next = stack->top;
    stack->top = item;
}

NodePtr nodeStackPop(NodeStack *stack) {
    if (stack == NULL || stack->top == NULL) {
        return NULL;
    }
    NodeStackItem *topItem = stack->top;
    NodePtr node = topItem->node;
    stack->top = topItem->next;
    free(topItem);
    return node;
}

NodePtr nodeStackTop(NodeStack *stack) {
    if (stack == NULL || stack->top == NULL) {
        return NULL;
    }
    return stack->top->node;
}

int isNodeStackEmpty(NodeStack *stack) {
    return (stack == NULL || stack->top == NULL);
}

void freeNodeStack(NodeStack *stack) {
    if (stack == NULL) return;
    while (stack->top != NULL) {
        NodeStackItem *temp = stack->top;
        stack->top = stack->top->next;
        free(temp);
    }
    free(stack);
}



/*
 *  EXPRESSION PARSER
 * */

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

int getPrecedenceIndex(tType token) {
    switch (token) {
        case T_ASTERISK:
            return 0;
        case T_SLASH:
            return 1;
        case T_PLUS:
            return 2;
        case T_MINUS:
            return 3;
        case T_EQUALS:
            return 4;
        case T_NOTEQUAL:
            return 5;
        case T_LESS:
            return 6;
        case T_GREATER:
            return 7;
        case T_LESSEQUAL:
            return 8;
        case T_GREATEREQUAL:
            return 9;
        case T_LBRACKET:
            return 10;
        case T_RBRACKET:
            return 11;
        case T_INT:
        case T_FLOAT:
        case T_STRING:
        case T_ID:
        case T_IFJ:
        case FN_CALL:
        case T_NULL:
            return 12;
        case STACK_END:
            return 13;
        default:
            return -1;
    }
}

Precedence getAction(tType topToken, tType inputToken) {
    int topIdx = getPrecedenceIndex(topToken);
    int inputIdx = getPrecedenceIndex(inputToken);
    if (inputIdx == -1) {
        inputIdx = getPrecedenceIndex(STACK_END);
    }
    if (topIdx == -1) return P_X;
    return precedenceTable[topIdx][inputIdx];
}

// Create node for values
NodePtr createValueNode(Token token) {
    NodePtr node = initNode();

    switch (token->type) {
        case T_INT:
            node->data_type = INT;
            node->keyword=T_INT;
            node->data.int_val = atoi(token->data);
            break;
        case T_FLOAT:
            node->data_type = FLOAT;
            node->keyword=T_FLOAT;
            node->data.float_val = atof(token->data);
            break;
        default:
            node->keyword=token->type;
            node->data_type = STRING;
            node->data.string_val = strdup(token->data);
    }

    return node;
}

NodePtr createOperatorNode(Token operator, NodePtr left, NodePtr right) {
    NodePtr node = initNode();
    node->data_type = ONLY_KEYWORD;
    node->keyword = operator->type;

    node->left = left;
    node->right = right;
    return node;
}

NodePtr parseExpression() {
    NodeStack *nodeStack = createNodeStack();
    TokenStack *tokenStack = createTokenStack();

    NodePtr endNode = initNode();
    endNode->data_type = ONLY_KEYWORD;
    endNode->keyword = STACK_END;
    nodeStackPush(nodeStack, endNode);

    Token endToken = initToken();
    endToken->type = STACK_END;
    tokenStackPush(tokenStack, endToken);

    //node for function calls in expression
    NodePtr functionNode = NULL;
    int openBracketCount = 0;
    bool loadNewToken = 1;

//    printf("start of expression parser\n");
    while (1) {
//        printf("--------------------\n");

        if (loadNewToken) {
            get_token(token);
        }
      /*  if (firstToken) {
            if (token->type == T_NULL) {
                NodePtr node = initNode();
                node->data_type = ONLY_KEYWORD;
                node->keyword = T_NULL;
                get_token(token);
                return node;
            }
        }*/
//        firstToken = false;

        //process function
        if (tokenStackTop(tokenStack)->type == T_ID && token->type == T_LBRACKET) {
            tokenStackTop(tokenStack)->type = FN_CALL;
            NodePtr node = initNode();
            node->keyword = FN_CALL;
            node->data_type = STRING;
            node->data.string_val = tokenStackTop(tokenStack)->data;
            //TODO make this work
            node->left = process_function_call_arguments();

            while (token->type != T_RBRACKET) {
                get_token(token);
            }
            functionNode = node;

            get_token(token);
        } else if (tokenStackTop(tokenStack)->type == T_IFJ) {
            NodePtr node = initNode();
            node->data_type = ONLY_KEYWORD;
            node->keyword = T_IFJ;
            // .
            if (token->type != T_DOT) {
                fprintf(stderr,"Expected .\n");
                exit(2);
            }
            // ID
            get_token(token);
            if (token->type != T_ID) {
                fprintf(stderr,"Expected id\n");
                exit(2);
            }
            NodePtr functionName = initNode();
            functionName->data_type = STRING;
            functionName->data.string_val = token->data;
            functionName->keyword = token->type;
            // (
            get_token(token);
            if (token->type != T_LBRACKET) {
                fprintf(stderr,"Expected (\n");
                exit(2);
            }

            node->left = functionName;
            //TODO make parse arguments
            node->right = process_function_call_arguments();

            get_token(token);
            functionNode = node;

        }
        if (token->type == T_RBRACKET) {
            if (openBracketCount == 0 && tokenStackTop(tokenStack)->type == 57) {
                NodePtr root = nodeStackPop(nodeStack);
                if (tokenStackTop(tokenStack)->type != 57) {
                    fprintf(stderr,"Invalid expression\n");
                    exit(2);
                }
                if (nodeStackTop(nodeStack) != NULL) {
                    if (nodeStackTop(nodeStack)->keyword != 57) {
                        fprintf(stderr,"Invalid expression\n");
                        exit(2);
                    }
                }
                if (root->keyword == STACK_END) {
                    root = NULL;
                }
                freeTokenStack(tokenStack);
                freeNodeStack(nodeStack);
                return root;
            }
        }
        int action = getAction(tokenStackTop(tokenStack)->type, token->type);

        /*
         *  only printing action
         * */

        /*printf("stack:%d, input:%d\n", tokenStackTop(tokenStack)->type, token->type);
        if (action == 1) {
            printf("reduction\n");
        } else if (action == 0) {
            printf("shift\n");
        } else {
            printf("%d\n", action);
        }
*/

        //reduce
        if (action == P_R) {
            Token processToken = tokenStackPop(tokenStack);
            loadNewToken = 0;
            switch (processToken->type) {
                case T_INT:
                case T_FLOAT:
                case T_STRING:
                case T_ID:
                case T_NULL:
                    nodeStackPush(nodeStack, createValueNode(processToken));
                    break;
                case FN_CALL:
                    if (functionNode != NULL && functionNode->keyword != FN_CALL) {
                        fprintf(stderr,"Invalid expression\n");
                        exit(2);
                    }
                    nodeStackPush(nodeStack, functionNode);

                    break;
                case T_IFJ:
                    if (functionNode != NULL && functionNode->keyword != T_IFJ) {
                        fprintf(stderr,"Invalid expression\n");
                        exit(2);
                    }
                    nodeStackPush(nodeStack, functionNode);
                    break;
                case T_PLUS:
                case T_MINUS:
                case T_ASTERISK:
                case T_SLASH:
                case T_EQUALS:
                case T_NOTEQUAL:
                case T_GREATER:
                case T_GREATEREQUAL:
                case T_LESS:
                case T_LESSEQUAL:{
                    NodePtr right = nodeStackPop(nodeStack);
                    NodePtr left = nodeStackPop(nodeStack);
                    if (right->keyword == STACK_END || left->keyword == STACK_END) {
                        fprintf(stderr,"Invalid expression\n");
                        exit(2);
                    }
                    nodeStackPush(nodeStack,
                                  createOperatorNode(processToken, left, right)
                    );
                    break;
                }
                default:
                    //printf("undefined reduction\n");
                    fprintf(stderr,"Invalid expression\n");
                    exit(2);
                    break;
            }
        } else if (action == P_S) {
            if (token->type == T_LBRACKET) {
                openBracketCount++;
            }
            loadNewToken = 1;
            tokenStackPush(tokenStack, token);
            //printf("token top stack %d\n", tokenStackTop(tokenStack)->type);
        } else if (action == P_E) {
            Token lBracToken = tokenStackPop(tokenStack);

            if (lBracToken->type != T_LBRACKET) {
                fprintf(stderr,"Invalid expression\n");
                exit(2);
            }
            if (openBracketCount == 0) {
                break;
            }
            openBracketCount--;
            //move from )
            get_token(token);
            //printf("brackets\n");
        } else if (action == P_END) {
            break;
        } else {
            fprintf(stderr,"Invalid expression\n");
            exit(2);
        }

    }

    //printf("token top stack end %d\n", tokenStackTop(tokenStack)->type);

    NodePtr root = nodeStackPop(nodeStack);
    if (tokenStackTop(tokenStack)->type != 57) {
        fprintf(stderr,"Invalid expression\n");
        exit(2);
    }
    if (nodeStackTop(nodeStack) != NULL) {
        if (nodeStackTop(nodeStack)->keyword != 57) {
            fprintf(stderr,"Invalid expression\n");
            exit(2);
        }
    }
    if (root->keyword == STACK_END) {
        root = NULL;
    }
    freeTokenStack(tokenStack);
    freeNodeStack(nodeStack);
    return root;
}

