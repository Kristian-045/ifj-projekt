#include "scanner.h"
#include "parser.h"
#include "visualization.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * @file parser.c
 * @brief Main file for the IFJ project.
 *
 * @authors
 * - Kristián Kaleta (xkaletk00)
 *
 * @date 2024
 */

Token token = NULL;
int nextTokenMustBeElse = 0;

/**
 * @brief Constructs whole tree representing zig code.
 *
 * @return NodePtr - pointer to root node representing the whole Zig code.
 */
NodePtr parser() {
    // Initialize the token that will be used during parsing.
    token = init_token();
    // Create the root node of the syntax tree.
    NodePtr node = init_node();
    node->data_type = ONLY_KEYWORD;
    node->keyword = START;

    // Process the prolog part of the Zig code and attach it to the right child
    node->right = process_prolog();
    // Process the list of functions and attach it to the left child
    node->left = process_function_list();

    free(token);
    return node;
}

/**
 * @brief Constructs subtree representing the all functions in Zig code.
 *
 * @return NodePtr - pointer to node representing all functions in Zig code.
 */
NodePtr process_function_list() {
    // Root node of subtree is new command
    NodePtr rootNode = init_node();
    rootNode->data_type = ONLY_KEYWORD;
    rootNode->keyword = NEW_COMMAND;

    // Process the first function and attach it as the right child.
    rootNode->right = process_function();
    NodePtr lastNode = rootNode;

    // Loop through and process the rest of the functions in the list.
    while (lastNode->right != NULL) {
        NodePtr node = init_node();
        node->data_type = ONLY_KEYWORD;
        node->keyword = NEW_COMMAND;

        // Attach the new command node as the left child of the last node (previous function).
        lastNode->left = node;

        // Process the next function and attach it as the right child of the new command node.
        node->right = process_function();

        lastNode = node;
    }
    return rootNode;
}

/**
 * @brief Processes and constructs a syntax tree for a single function declaration in Zig code.
 *
 * @return NodePtr - Pointer to the root node representing the function.
 */
NodePtr process_function() {
    // Initialize the root node representing the function name.
    NodePtr functionName = init_node();

    // Initialize a node for function metadata (e.g., parameters and return type).
    NodePtr dataFn = init_node();
    dataFn->data_type = ONLY_KEYWORD;
    dataFn->keyword = FN_DATA;
    functionName->left = dataFn;

    // Parse the "pub" keyword if present.
    get_token(token);
    if (token->type != T_PUB) {
        // Handle end of file case.
        if (token->type == T_EOF) {
            return NULL;
        }
        fprintf(stderr, "Expected pub\n");
        exit(2);
    }

    // Parse the "fn" keyword.
    get_token(token);
    if (token->type != T_FN) {
        fprintf(stderr, "Expected fn\n");
        exit(2);
    }
    functionName->data_type = STRING;
    functionName->keyword = T_FN;

    // Parse the function identifier (name).
    get_token(token);
    if (token->type != T_ID) {
        fprintf(stderr, "Expected id\n");
        exit(2);
    }
    functionName->data.string_val = token->data;

    // Parse the opening parenthesis '(' for the parameter list.
    get_token(token);
    if (token->type != T_LBRACKET) {
        fprintf(stderr, "Expected (\n");
        exit(2);
    }

    // Process the parameter list and attach it to the left child of dataFn.
    dataFn->left = process_parameter_list(true);

    // Initialize a node for the return type.
    NodePtr returnType = init_node();
    returnType->data_type = ONLY_KEYWORD;

    NodePtr questionNode = NULL;
    // Optional: Parse the '?' indicating an optional return type.
    get_token(token);
    if (token->type == T_QUESTIONMARK) {
        questionNode = init_node();
        questionNode->data_type = ONLY_KEYWORD;
        questionNode->keyword = T_QUESTIONMARK;
        get_token(token);
    }

    // Parse the return type, which can be void or a specific type.
    if (token->type != T_VOID) {
        validate_type();
        returnType->keyword = token->type;
    } else {
        // Ensure '?' is not used with void.
        if (questionNode != NULL) {
            fprintf(stderr, "Unexpected ?\n");
            exit(2);
        }
        returnType->keyword = T_VOID;
    }

    // Attach the '?' node to the left child of the returnType node.
    returnType->left = questionNode;

    // Attach the returnType node to the right child of dataFn.
    dataFn->right = returnType;

    // Parse the opening brace '{' for the function body.
    get_token(token);
    if (token->type != T_CLBRACKET) {
        // Handle a function declaration without a body.
        if (token->type == T_SEMICOLON) {
            fprintf(stderr, "Function without body\n");
            exit(3);
        }
        fprintf(stderr, "Expected {\n");
        exit(2);
    }

    // Process the function body and attach it to the right child of functionName.
    functionName->right = process_block();

    return functionName;
}

/**
 * @brief Processes and constructs a syntax tree for a parameter list in a function declaration.
 *
 * @param first - A flag indicating if this is the first parameter in the list
 *                (1 for the first parameter, 0 otherwise).
 * @return NodePtr - Pointer to the root node of the parameter list subtree.
 */
NodePtr process_parameter_list(int first) {
    NodePtr node = init_node();
    NodePtr questionNode = NULL;

    // Handle the comma if this is not the first parameter in the list.
    get_token(token);
    if (!first && token->type == T_COMMA) {
        get_token(token);
    }

    // Parse the parameter name (identifier).
    if (token->type != T_ID) {
        // Check end of parameter list.
        if (token->type == T_RBRACKET)return NULL;
        else {
            fprintf(stderr, "Invalid syntax in function list\n");
            exit(2);
        }
    }
    node->data_type = STRING;
    node->data.string_val = token->data;

    // Parse the colon ':'.
    get_token(token);
    if (token->type != T_COLON) {
        fprintf(stderr, "Expected :\n");
        exit(2);
    }

    // Optional: Parse the '?' indicating an optional parameter.
    get_token(token);
    if (token->type == T_QUESTIONMARK) {
        questionNode = init_node();
        questionNode->data_type = ONLY_KEYWORD;
        questionNode->keyword = T_QUESTIONMARK;
        get_token(token);
    }

    // Validate the parameter's type.
    validate_type();
    node->keyword = token->type;

    node->left = questionNode;
    // Recursively process the remaining parameters and attach them to the right child.
    node->right = process_parameter_list(false);

    return node;
}

/**
 * @brief Processes and constructs a syntax tree for a block of code.
 *
 * @return NodePtr - Pointer to the root node representing the parsed block.
 */
NodePtr process_block() {
    NodePtr node = init_node();
    node->keyword = NEW_COMMAND;
    node->data_type = ONLY_KEYWORD;
    get_token(token);

    // Ensure proper "else" handling
    if (nextTokenMustBeElse == 1 && token->type != T_ELSE) {
        fprintf(stderr, "Expected else\n");
        exit(2);
    } else if (nextTokenMustBeElse == 0 && token->type == T_ELSE) {
        fprintf(stderr, "Unexpected else\n");
        exit(2);
    }
    nextTokenMustBeElse = 0;

    // Determine the type of statement and process it accordingly.
    switch (token->type) {
        case T_CONST:
        case T_VAR:
            // Handle a variable or constant declaration.
            node->right = process_declaration();
            break;
        case T_ID:
            // Handle an assignment or a function call.
            node->right = process_asgmt_or_fn();
            break;
        case T_IFJ:
            // Handle a specific IFJ function call.
            node->right = process_ifj_call();
            break;
        case T_RETURN:
            // Handle a return statement.
            node->right = process_return();
            break;
        case T_IF:
            // Handle an "if" statement and set the "else" expectation flag.
            node->right = process_if();
            nextTokenMustBeElse = 1;
            break;
        case T_ELSE:
            // Ensure "else" follows an "if" statement.
            node->right = process_else();
            break;
        case T_WHILE:
            // Handle a "while" loop.
            node->right = process_while();
            break;
        case T_CRBRACKET:
            // End of the current block, free the node and return NULL.
            free(node);
            return NULL;
        default:
            // Handle invalid syntax within the block.
            fprintf(stderr, "Invalid syntax in body\n");
            exit(2);
    }

    // Recursively process the next block
    node->left = process_block();
    return node;
}

/**
 * @brief Processes and constructs a syntax tree for an IFJ function call.
 *
 * @return NodePtr - Pointer to the root node representing the IFJ function call.
 */
NodePtr process_ifj_call() {
    NodePtr node = init_node();
    node->data_type = ONLY_KEYWORD;
    node->keyword = token->type;

    // Parse the expected dot (.) after the IFJ call keyword.
    get_token(token);
    if (token->type != T_DOT) {
        fprintf(stderr, "Expected .\n");
        exit(2);
    }

    // Parse the function name (identifier) following the dot.
    get_token(token);
    if (token->type != T_ID) {
        fprintf(stderr, "Expected id\n");
        exit(2);
    }

    // Create a node for the function name and store its value.
    NodePtr functionName = init_node();
    functionName->data_type = STRING;
    functionName->data.string_val = token->data;
    functionName->keyword = token->type;

    // Parse the opening parenthesis '(' for the function call arguments.
    get_token(token);
    if (token->type != T_LBRACKET) {
        fprintf(stderr, "Expected (\n");
        exit(2);
    }

    // Attach the function name to the left child of the node.
    node->left = functionName;

    // Process the function call arguments and attach them to the right child.
    node->right = process_function_call_arguments();

    // Parse the semicolon ';' marking the end of the function call.
    get_token(token);
    if (token->type != T_SEMICOLON) {
        fprintf(stderr, "Expected ;\n");
        exit(2);
    }

    return node;
}

/**
 * @brief Processes and constructs a syntax tree for a "while" loop.
 *
 * @return NodePtr - Pointer to the root node representing the "while" loop.
 */
NodePtr process_while() {
    NodePtr whileNode = init_node();
    whileNode->data_type = ONLY_KEYWORD;
    whileNode->keyword = token->type;

    // Initialize a data node to represent the "while" expression and id without null (optional).
    NodePtr whileDataNode = init_node();
    whileDataNode->data_type = ONLY_KEYWORD;
    whileDataNode->keyword = WHILE_DATA;

    // Attach the "while" data node to the left child of the "while" node.
    whileNode->left = whileDataNode;

    // Parse the opening parenthesis '(' for the condition.
    get_token(token);
    if (token->type != T_LBRACKET) {
        fprintf(stderr, "Expected (\n");
        exit(2);
    }

    // Parse the condition of the "while" loop and attach it to the left child of the data node.
    whileDataNode->left = process_expression_one_keyword(false, T_RBRACKET);

    // Parse the identifier (optional additional data) and attach it to the right child of the data node.
    whileDataNode->right = process_id_without_null();

    // Ensure the opening curly bracket '{' for the loop body is present.
    if (token->type != T_CLBRACKET) {
        fprintf(stderr, "Expected {\n");
        exit(2);
    }

    // Process the loop body and attach it to the right child of the "while" node.
    whileNode->right = process_block();

    return whileNode;
}

/**
 * @brief Processes and constructs a syntax tree for an "if" statement.
 *
 * @return NodePtr - Pointer to the root node representing the "if" statement.
 */
NodePtr process_if() {
    NodePtr ifNode = init_node();
    ifNode->data_type = ONLY_KEYWORD;
    ifNode->keyword = token->type;

    // Initialize a data node to represent expression and id without null (optional) about the "if" statement.
    NodePtr ifDataNode = init_node();
    ifDataNode->data_type = ONLY_KEYWORD;
    ifDataNode->keyword = IF_DATA;

    ifNode->left = ifDataNode;

    // Parse the opening parenthesis '(' for the condition.
    get_token(token);
    if (token->type != T_LBRACKET) {
        fprintf(stderr, "Expected (\n");
        exit(2);
    }

    // Parse the condition of the "if" statement and attach it to the left child of the data node.
    ifDataNode->left = process_expression_one_keyword(false, T_RBRACKET);
    // Parse the identifier (optional additional data) and attach it to the right child of the data node.
    ifDataNode->right = process_id_without_null();

    // Ensure the opening curly bracket '{' for the body of the "if" statement.
    if (token->type != T_CLBRACKET) {
        fprintf(stderr, "Expected {\n");
        exit(2);
    }

    // Process the body of the "if" statement and attach it to the right child of the "if" node.
    ifNode->right = process_block();;

    return ifNode;
}

/**
 * @brief Processes and constructs a syntax tree for an "else" statement.
 *
 * @return NodePtr - Pointer to the root node representing the "else" statement.
 */
NodePtr process_else() {
    NodePtr elseNode = init_node();
    elseNode->data_type = ONLY_KEYWORD;
    elseNode->keyword = T_ELSE;

    // Parse the opening curly bracket '{' for the block that follows the "else" keyword.
    get_token(token);
    if (token->type != T_CLBRACKET) {
        fprintf(stderr, "Expected {\n");
        exit(2);
    }

    // Process the block of statements inside the "else" body and attach it to the right child of the "else" node.
    elseNode->right = process_block();
    return elseNode;
}

/**
 * @brief Processes and constructs a node for an identifier in a pipe-separated
 * expression. |identifier|
 *
 * @return NodePtr - Pointer to the node representing the identifier, or NULL
 *                   if the pipe symbol was not found.
 */
NodePtr process_id_without_null() {
    // Parse the first pipe symbol '|' to indicate the start of the identifier.
    get_token(token);
    if (token->type != T_PIPE) {
        return NULL;
    }

    // Parse the identifier that follows the pipe symbol.
    get_token(token);
    if (token->type != T_ID) {
        fprintf(stderr, "Expected id\n");
        exit(2);
    }

    // Initialize a node for the identifier and store its value.
    NodePtr node = init_node();
    node->keyword = T_ID;
    node->data_type = STRING;
    node->data.string_val = token->data;

    // Parse the second pipe symbol '|' after the identifier.
    get_token(token);
    if (token->type != T_PIPE) {
        fprintf(stderr, "Expected |\n");
        exit(2);
    }

    get_token(token);
    return node;
}

/**
 * @brief Processes an identifier that is either part of an assignment or a
 * function call. This function handles both cases and constructs the appropriate
 * syntax tree nodes for each.
 *
 * @return NodePtr - Pointer to the node representing the assignment or function call.
 */
NodePtr process_asgmt_or_fn() {
    // Initialize the node for the identifier (which will be part of either an assignment or function call).
    NodePtr node = init_node();
    node->keyword = T_ID;
    node->data_type = STRING;
    node->data.string_val = token->data;

    get_token(token);

    // Switch on the token type to determine the next parsing step.
    switch (token->type) {
        case T_EQUALSIGN: {
            // If the token is '=', it's an assignment. Process the assignment.
            NodePtr id_node = node;
            node = process_assignment();
            node->left = id_node;
            break;
        }
        case T_LBRACKET:
            // If the token is '(', it's a function call. Process the function call arguments.
            node->keyword = FN_CALL;
            node->left = process_function_call_arguments();

            // Expect a semicolon after the function call.
            get_token(token);
            if (token->type != T_SEMICOLON) {
                fprintf(stderr, "Expected ;\n");
                exit(2);
            }
            break;
        default:
            // If the token is neither '=' nor '(', print an error and exit.
            fprintf(stderr, "invalid token after id\n");
            exit(2);
    }
    return node;
}

/**
 * @brief Processes the arguments of a function call. This function handles
 * expressions that appear inside the parentheses of a function call.
 *
 * @return NodePtr - Pointer to the root node representing the function call
 *                   arguments, or NULL if no arguments are provided.
 */
NodePtr process_function_call_arguments() {
    //processing arguments
    NodePtr rootNode = init_node();

    // Process the first expression (argument) in the function call.
    //2 is indicating that first can not be null but second keyword can be null
    NodePtr expNode = process_expression(2, T_COMMA, T_RBRACKET);
    if (expNode != NULL) {
        rootNode->data_type = ONLY_KEYWORD;
        rootNode->keyword = FN_PARAM;
        rootNode->right = expNode;
    }

    NodePtr lastNode = rootNode;
    // Process additional arguments separated by commas.
    while (token->type == T_COMMA) {
        tType lastKeyword = token->type;

        // Process the next argument expression after the comma.
        expNode = process_expression(1, T_COMMA, T_RBRACKET);

        // Handle cases with two consecutive commas (,,) without a parameter in between.
        if (lastKeyword == T_COMMA && token->type == T_COMMA && expNode == NULL) {
            fprintf(stderr, "2 commas without param\n");
            exit(2);
        }

        // Handle the case where a closing parenthesis ')' follows a comma with no argument.
        if (lastKeyword == T_COMMA && token->type == T_RBRACKET && expNode == NULL) {
            break;
        }

        // Create a new node for the argument and link it to the argument list.
        NodePtr node = init_node();
        node->data_type = ONLY_KEYWORD;
        node->keyword = FN_PARAM;

        // Attach the expression (argument) to the node.
        node->right = expNode;

        // Link the new node to the previous node.
        lastNode->left = node;
        lastNode = node;
    }

    // If no expression was processed (rootNode->right is NULL), free the root node and return NULL.
    if (rootNode->right == NULL) {
        free_tree(rootNode);
        return NULL;
    }
    return rootNode;
}

/**
 * @brief Processes and constructs a syntax tree for an assignment expression.
 *
 * @return NodePtr - Pointer to the root node representing the assignment expression.
 */
NodePtr process_assignment() {
    NodePtr node = init_node();
    node->data_type = ONLY_KEYWORD;
    node->keyword = token->type;

    // Process the expression on the right side of the assignment
    node->right = process_expression_one_keyword(false, T_SEMICOLON);

    return node;
}

/**
 * @brief Processes a variable or constant declaration and constructs the corresponding syntax tree.
 *
 * @return NodePtr - Pointer to the root node representing the declaration.
 */
NodePtr process_declaration() {
    // Initialize a node to represent the variable or constant declaration.
    NodePtr node = init_node();
    node->data_type = STRING;
    node->keyword = token->type;

    get_token(token);

    // Ensure the next token is an identifier (the name of the variable/constant).
    if (token->type != T_ID) {
        if (token->type == T_IFJ) {
            fprintf(stderr, "Redeclared variable\n");
            exit(5);
        } else {
            fprintf(stderr, "Expected id\n");
            exit(2);
        }
    }
    node->data.string_val = token->data;

    // Opional: Check if the next token is a colon (indicating a type specification).
    get_token(token);
    if (token->type == T_COLON) {
        node->left = process_type();
        get_token(token);
    }

    // Check if the next token is an equals sign (indicating an assignment).
    if (token->type != T_EQUALSIGN) {
        fprintf(stderr, "Expected =\n");
        exit(2);
    }

    // Create a new node to represent the assignment operator.
    NodePtr equalSign = init_node();
    equalSign->data_type = ONLY_KEYWORD;
    equalSign->keyword = T_EQUALSIGN;

    // Link the declaration node (variable/constant) as the left child of the equals sign.
    equalSign->left = node;

    // Process the expression on the right-hand side of the assignment (the initial value).
    equalSign->right = process_expression_one_keyword(false, T_SEMICOLON);

    return equalSign;
}

/**
 * @brief Processes a return statement and constructs the corresponding syntax tree.
 *
 * @return NodePtr - Pointer to the root node representing the return statement.
 */
NodePtr process_return() {
    NodePtr node = init_node();
    node->data_type = ONLY_KEYWORD;
    node->keyword = T_RETURN;
    node->right = process_expression_one_keyword(true, T_SEMICOLON);

    return node;
}

/**
 * @brief Processes an expression with one possible ending keyword and constructs the corresponding syntax tree.
 *
 * @param canBeNull - A flag indicating if the expression can be null.
 * @param endKeyword - The ending keyword that marks the end of the expression.
 * @return NodePtr - Pointer to the root node representing the expression.
 */
NodePtr process_expression_one_keyword(bool canBeNull, tType endKeyword) {
    return process_expression(canBeNull, endKeyword, INVALID_TOKEN);
}

/**
 * @brief Processes a general expression and constructs the corresponding syntax tree.
 *
 * @param canBeNull - A flag indicating if the expression can be null
 *                    (0: not null, 1: can be null, 2: not null if keyword1, can be null if keyword1 ).
 * @param endKeyword1 - The first keyword marking the end of the expression.
 * @param endKeyword2 - The second optional keyword marking the end of the expression.
 * @return NodePtr - Pointer to the root node representing the expression.
 */
NodePtr process_expression(int canBeNull, tType endKeyword1, tType endKeyword2) {
    // Parse the expression
    NodePtr expression = parse_expression();

    // If expression is not allowed to be null and no expression was parsed, raise an error
    if (canBeNull == 0 && expression == NULL) {
        fprintf(stderr, "Expression cant be null\n");
        exit(2);
    }

    // if expression is not allowed to be null only for endKeyword1
    if (canBeNull == 2 && expression == NULL && endKeyword1 == token->type) {
        fprintf(stderr, "Expression cant be null\n");
        exit(2);
    }

    // If the token doesn't match either of the expected end keywords, raise an error
    if (token->type != endKeyword1 && token->type != endKeyword2) {
        fprintf(stderr, "Unexpected token in expression\n");
        exit(2);
    }

    return expression;
}

/**
 * @brief Processes the prolog of the Zig code.
 *
 * @return NodePtr - Pointer to the root node representing the prolog structure.
 */
NodePtr process_prolog() {
    NodePtr node = init_node();

    // Expecting a 'const' keyword to begin the prolog
    get_token(token);
    if (token->type != T_CONST) {
        fprintf(stderr, "Expected const\n");
        exit(2);
    }
    node->keyword = T_CONST;

    // Expecting 'ifj' keyword after 'const'
    get_token(token);
    if (token->type != T_IFJ) {
        fprintf(stderr, "Expected ifj\n");
        exit(2);
    }
    node->data_type = STRING;
    node->data.string_val = token->data;

    // Expecting an '=' after 'ifj'
    get_token(token);
    if (token->type != T_EQUALSIGN) {
        fprintf(stderr, "Expected =\n");
        exit(2);
    }

    NodePtr eqNode = init_node();
    eqNode->data_type = ONLY_KEYWORD;
    eqNode->keyword = T_EQUALSIGN;

    // Link the previous node (const-ifj) to the equality sign node
    eqNode->left = node;

    // Expecting 'import' keyword after the equality sign
    get_token(token);
    if (token->type != T_IMPORT) {
        fprintf(stderr, "Expected import\n");
        exit(2);
    }

    NodePtr importNode = init_node();
    eqNode->right = importNode;
    importNode->data_type = ONLY_KEYWORD;
    importNode->keyword = token->type;

    // Expecting '(' after 'import'
    get_token(token);
    if (token->type != T_LBRACKET) {
        fprintf(stderr, "Expected (\n");
        exit(2);
    }

    // Expecting a string literal after '('
    get_token(token);
    if (token->type != T_STRING) {
        fprintf(stderr, "Expected string\n");
        exit(2);
    }

    NodePtr stringNode = init_node();
    // Link import node to string node
    importNode->right = stringNode;

    stringNode->data_type = STRING;
    stringNode->data.string_val = token->data;

    // Expecting ')' to close the import statement
    get_token(token);
    if (token->type != T_RBRACKET) {
        fprintf(stderr, "Expected )\n");
        exit(2);
    }

    // Expecting ';' to end the prolog
    get_token(token);
    if (token->type != T_SEMICOLON) {
        fprintf(stderr, "Expected ;\n");
        exit(2);
    }

    return eqNode;
}

/**
 * @brief Processes a type declaration, including optional handling of the
 *        `?` (nullable) keyword and validating the type.
 *
 * @return NodePtr - Pointer to the node representing the type declaration.
 */
NodePtr process_type() {

    NodePtr node = init_node();
    NodePtr questionNode = NULL;

    // Look ahead for the nullable '?' keyword
    get_token(token);
    if (token->type == T_QUESTIONMARK) {
        // Create a node for the '?' keyword
        questionNode = init_node();
        questionNode->data_type = ONLY_KEYWORD;
        questionNode->keyword = T_QUESTIONMARK;
        get_token(token);
    }

    // Validate the actual type (e.g., int, string, etc.)
    validate_type();

    // Set the current node as the type
    node->data_type = ONLY_KEYWORD;
    node->keyword = token->type;
    node->left = questionNode;

    return node;
}

/**
 * @brief Initializes a new node for the syntax tree.
 *
 * @return NodePtr - Pointer to the newly created node.
 */
NodePtr init_node() {
    NodePtr newNode = (NodePtr) malloc(sizeof(struct Node));
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(99);
    }
    newNode->left = NULL;
    newNode->right = NULL;

    return newNode;
}

/**
 * @brief Recursively frees a syntax tree starting from the root node.
 *        It frees the left and right subtrees and the memory used by any string data.
 *
 * @param root - Pointer to the root node of the tree to be freed.
 */
void free_tree(NodePtr root) {
    if (root == NULL) return;//return error
    free_tree(root->left);
    free_tree(root->right);

    if (root->data_type == STRING) {
        free(root->data.string_val);
    }

    free(root);
}

/**
 * @brief Initializes a new token.
 *        Allocates memory for the Token struct and sets default values.
 *
 * @return Token - Pointer to the newly created token.
 */
Token init_token() {
    Token newToken = malloc(sizeof(struct Token));  // Allocates memory for the Token struct
    if (newToken == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(99);
    }
    newToken->type = T_UNDEFINED;  // Sets default type
    newToken->data = NULL;         // Sets default data to NULL
    return newToken;
}

/**
 * @brief Validates the type of the current token.
 *        Ensures that the token is a valid type (T_I32, T_F64, []u8 )
 *
 * @note The function will terminate the program if an invalid type is found.
 */
void validate_type() {
    // If the token is neither T_I32 nor T_F64, check for array type
    if (token->type != T_I32 && token->type != T_F64) {
        // Check for [ for an array type
        if (token->type != T_SLBRACKET) {
            fprintf(stderr, "Invalid type\n");
            exit(2);
        }

        // Check for ]
        get_token(token);
        if (token->type != T_SRBRACKET) {
            fprintf(stderr, "Invalid type\n");
            exit(2);
        }

        // Check if the type is "u8"
        get_token(token);
        if (token->type != T_U8) {
            fprintf(stderr, "Invalid type\n");
            exit(2);
        }
    }
}

/*----------------------------------------------------
 *              EXPRESSION PARSER
 *---------------------------------------------------*/

/**
 * @brief Creates a new empty token stack.
 *
 * @return TokenStack* - Pointer to the newly created token stack.
 *
 * @details This function allocates memory for a `TokenStack` structure and initializes
 *          its top pointer to `NULL`, indicating an empty stack.
 */
TokenStack *create_token_stack() {
    TokenStack *stack = (TokenStack *) malloc(sizeof(TokenStack));
    if (stack == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(99);
    }
    stack->top = NULL;
    return stack;
}

/**
 * @brief Pushes a token onto the token stack.
 *
 * @param stack - The token stack to push the token onto.
 * @param pushToken - The token to push onto the stack.
 *
 * @details This function creates a new stack item, initializes it with the provided token,
 *          and adds it to the top of the stack.
 */
void token_stack_push(TokenStack *stack, Token pushToken) {
    if (stack == NULL) return;
    TokenStackItem *item = (TokenStackItem *) malloc(sizeof(TokenStackItem));
    if (item == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(99);
    };
    item->token = init_token();
    item->token->type = pushToken->type;
    item->token->data = pushToken->data;
    item->next = stack->top;
    stack->top = item;
}

/**
 * @brief Pops a token from the top of the token stack.
 *
 * @param stack - The token stack from which to pop the token.
 * @return Token - The token popped from the stack.
 *
 * @details This function removes the top item from the stack, retrieves the token from it,
 *          and frees the memory used by the item. If the stack is empty, it returns a
 *          token with the type `STACK_END`.
 */
Token token_stack_pop(TokenStack *stack) {
    if (stack == NULL || stack->top == NULL) {
        Token nullToken = init_token();
        token->type = STACK_END;
        return nullToken;
    }

    // Pop the top item from the stack.
    TokenStackItem *topItem = stack->top;
    Token popToken = topItem->token;

    // Update the top pointer to the next item in the stack.
    stack->top = topItem->next;

    free(topItem);
    return popToken;
}

/**
 * @brief Returns the token from the top of the token stack without removing it.
 *
 * @param stack - The token stack to peek at.
 * @return Token - The token at the top of the stack.
 *
 * @details This function returns the token at the top of the stack without modifying the stack.
 *          If the stack is empty, it returns a token with the type `STACK_END`.
 */
Token token_stack_top(TokenStack *stack) {
    if (stack == NULL || stack->top == NULL) {
        Token nullToken = init_token();
        nullToken->type = STACK_END;
        return nullToken;
    }
    return stack->top->token;
}

/**
 * @brief Frees the memory used by the token stack and its items.
 *
 * @param stack - The token stack to free.
 *
 * @details This function iterates through the stack, freeing each stack item and its associated
 *          token memory. It then frees the stack structure itself.
 */
void free_token_stack(TokenStack *stack) {
    if (stack == NULL) return;

    // Free each stack item.
    while (stack->top != NULL) {
        TokenStackItem *temp = stack->top;
        stack->top = stack->top->next;
        free(temp);
    }
    free(stack);
}

/**
 * @brief Creates a new empty node stack.
 *
 * @return NodeStack* - Pointer to the newly created node stack.
 *
 * @details This function allocates memory for a `NodeStack` structure and initializes
 *          its top pointer to `NULL`, indicating an empty stack.
 */
NodeStack *create_node_stack() {
    NodeStack *stack = (NodeStack *) malloc(sizeof(NodeStack));
    if (stack == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(99);
    }
    stack->top = NULL;
    return stack;
}

/**
 * @brief Pushes a node onto the node stack.
 *
 * @param stack - The node stack to push the node onto.
 * @param node - The node to push onto the stack.
 *
 * @details This function creates a new stack item, initializes it with the provided node,
 *          and adds it to the top of the stack.
 */
void node_stack_push(NodeStack *stack, NodePtr node) {
    if (stack == NULL) return;
    NodeStackItem *item = (NodeStackItem *) malloc(sizeof(NodeStackItem));
    if (item == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(99);
    }
    item->node = node;

    item->next = stack->top;
    stack->top = item;
}

/**
 * @brief Pops a node from the top of the node stack.
 *
 * @param stack - The node stack from which to pop the node.
 * @return NodePtr - The node popped from the stack.
 *
 * @details This function removes the top item from the stack, retrieves the node from it,
 *          and frees the memory used by the item. If the stack is empty, it returns `NULL`.
 */
NodePtr node_stack_pop(NodeStack *stack) {
    if (stack == NULL || stack->top == NULL) {
        return NULL;
    }

    // Pop the top item from the stack.
    NodeStackItem *topItem = stack->top;
    NodePtr node = topItem->node;

    // Update the top pointer to the next item in the stack.
    stack->top = topItem->next;

    free(topItem);
    return node;
}

/**
 * @brief Returns the node from the top of the node stack without removing it.
 *
 * @param stack - The node stack to peek at.
 * @return NodePtr - The node at the top of the stack.
 *
 * @details This function returns the node at the top of the stack without modifying the stack.
 *          If the stack is empty, it returns `NULL`.
 */
NodePtr node_stack_top(NodeStack *stack) {
    if (stack == NULL || stack->top == NULL) {
        return NULL;
    }
    return stack->top->node;
}

/**
 * @brief Frees the memory used by the node stack and its items.
 *
 * @param stack - The node stack to free.
 *
 * @details This function iterates through the stack, freeing each stack item and its associated
 *          node. It then frees the stack structure itself.
 */
void free_node_stack(NodeStack *stack) {
    if (stack == NULL) return;

    // Free each stack item.
    while (stack->top != NULL) {
        NodeStackItem *temp = stack->top;
        stack->top = stack->top->next;
        free(temp);
    }
    free(stack);
}

/**
 * @brief Returns the precedence index for a given token.
 *
 * @param token - The token whose precedence index is to be determined.
 * @return int - The precedence index corresponding to the token.
 *
 * @details This function assigns precedence levels to operators and tokens. Operators
 *          with higher precedence (e.g., multiplication and division) are given lower
 *          indices.
 */
int get_precedence_index(tType token) {
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
            // Invalid token, returning -1 for unknown precedence
            return -1;
    }
}

/**
 * @brief Determines the action to be taken based on the precedence of two tokens.
 *
 * @param topToken - The token at the top of the stack.
 * @param inputToken - The incoming token to be processed.
 * @return Precedence - The action based on the precedence comparison.
 *
 * @details This function compares the precedence of two tokens using a precedence table.
 *          It returns an action based on the relative precedence of the two tokens:
 *          - `P_X` for invalid or undefined precedence actions,
 *          - other actions (such as shift or reduce) based on the comparison.
 */
Precedence get_action(tType topToken, tType inputToken) {
    int topIdx = get_precedence_index(topToken);
    int inputIdx = get_precedence_index(inputToken);
    // If the input token has no precedence index, treat it as the stack end token.
    if (inputIdx == -1) {
        inputIdx = get_precedence_index(STACK_END);
    }
    // If the top token has an invalid precedence index, return the undefined action (P_X).
    if (topIdx == -1) return P_X;
    return precedenceTable[topIdx][inputIdx];
}

/**
 * @brief Creates a syntax tree node for a value token (e.g., integers, floats, strings).
 *
 * @param token - The token representing the value to be encapsulated in a node.
 * @return NodePtr - A pointer to the newly created node.
 */
NodePtr create_value_node(Token token) {
    NodePtr node = init_node();

    // Determine the token type and populate the node accordingly
    switch (token->type) {
        case T_INT:
            node->data_type = INT;
            node->keyword = T_INT;
            node->data.int_val = atoi(token->data);
            break;
        case T_FLOAT:
            node->data_type = FLOAT;
            node->keyword = T_FLOAT;
            node->data.float_val = atof(token->data);
            break;
        default:
            node->keyword = token->type;
            node->data_type = STRING;
            node->data.string_val = token->data;
    }

    return node;
}

/**
 * @brief Creates a syntax tree node for an operator.
 *
 * @param operator - The token representing the operator.
 * @param left - Pointer to the left child node.
 * @param right - Pointer to the right child node.
 * @return NodePtr - A pointer to the newly created operator node.
 *
 * @details This function constructs a syntax tree node for an operator,
 *          setting its type and linking it to its left and right child nodes.
 */
NodePtr create_operator_node(Token operator, NodePtr left, NodePtr right) {
    NodePtr node = init_node();
    node->data_type = ONLY_KEYWORD;
    node->keyword = operator->type;

    node->left = left;
    node->right = right;
    return node;
}

/**
 * @brief Parses an expression and constructs its syntax tree.
 *
 * @return NodePtr - A pointer to the root of the constructed syntax tree for the expression.
 *
 * @details This function implements a precedence-driven parser to process an expression.
 *          It uses two stacks: one for tokens and one for nodes.
 */
NodePtr parse_expression() {
    NodeStack *nodeStack = create_node_stack();
    TokenStack *tokenStack = create_token_stack();

    // Initialize a special end node
    NodePtr endNode = init_node();
    endNode->data_type = ONLY_KEYWORD;
    endNode->keyword = STACK_END;
    node_stack_push(nodeStack, endNode);

    // Initialize a special end token
    Token endToken = init_token();
    endToken->type = STACK_END;
    token_stack_push(tokenStack, endToken);

    NodePtr functionNode = NULL; // Stores function call nodes
    int openBracketCount = 0;    // Tracks open parentheses
    bool loadNewToken = 1;       // Determines whether to fetch a new token

    // Parsing loop
    while (1) {
        if (loadNewToken) {
            get_token(token);
        }

        // Handle function calls
        if (token_stack_top(tokenStack)->type == T_ID && token->type == T_LBRACKET) {
            token_stack_top(tokenStack)->type = FN_CALL;
            NodePtr node = init_node();
            node->keyword = FN_CALL;
            node->data_type = STRING;
            node->data.string_val = token_stack_top(tokenStack)->data;

            // Parse function call arguments
            node->left = process_function_call_arguments();

            // Expecting ')' to close function call
            if (token->type != T_RBRACKET) {
                fprintf(stderr, "Expected )\n");
                exit(2);
            }

            functionNode = node;

            get_token(token);
        }
        // Handle "ifj" functions
        else if (token_stack_top(tokenStack)->type == T_IFJ) {
            NodePtr node = init_node();
            node->data_type = ONLY_KEYWORD;
            node->keyword = T_IFJ;

            // Expect and process a dot followed by an ID
            if (token->type != T_DOT) {
                fprintf(stderr, "Expected .\n");
                exit(2);
            }
            get_token(token);
            if (token->type != T_ID) {
                fprintf(stderr, "Expected id\n");
                exit(2);
            }

            // Create a function name node
            NodePtr functionName = init_node();
            functionName->data_type = STRING;
            functionName->data.string_val = token->data;
            functionName->keyword = token->type;

            // Expect and process the opening bracket
            get_token(token);
            if (token->type != T_LBRACKET) {
                fprintf(stderr, "Expected (\n");
                exit(2);
            }

            // Attach the function name and arguments
            node->left = functionName;
            node->right = process_function_call_arguments();

            get_token(token);
            functionNode = node;
        }

        // Handle closing brackets
        if (token->type == T_RBRACKET) {
            if (openBracketCount == 0 && token_stack_top(tokenStack)->type == 57) {
                NodePtr root = node_stack_pop(nodeStack);

                // Validate the expression structure
                if (token_stack_top(tokenStack)->type != 57) {
                    fprintf(stderr, "Invalid expression\n");
                    exit(2);
                }
                if (node_stack_top(nodeStack) != NULL) {
                    if (node_stack_top(nodeStack)->keyword != 57) {
                        fprintf(stderr, "Invalid expression\n");
                        exit(2);
                    }
                }

                // Cleanup and return the root
                if (root->keyword == STACK_END) {
                    root = NULL;
                }
                free_token_stack(tokenStack);
                free_node_stack(nodeStack);
                return root;
            }
        }

        // Determine the precedence action
        int action = get_action(token_stack_top(tokenStack)->type, token->type);


        // Reduction
        if (action == P_R) {
            Token processToken = token_stack_pop(tokenStack);
            loadNewToken = 0;
            switch (processToken->type) {
                case T_INT:
                case T_FLOAT:
                case T_STRING:
                case T_ID:
                case T_NULL:
                    node_stack_push(nodeStack, create_value_node(processToken));
                    break;
                case FN_CALL:
                    if (functionNode != NULL && functionNode->keyword != FN_CALL) {
                        fprintf(stderr, "Invalid expression\n");
                        exit(2);
                    }
                    node_stack_push(nodeStack, functionNode);

                    break;
                case T_IFJ:
                    if (functionNode != NULL && functionNode->keyword != T_IFJ) {
                        fprintf(stderr, "Invalid expression\n");
                        exit(2);
                    }
                    node_stack_push(nodeStack, functionNode);
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
                case T_LESSEQUAL: {
                    NodePtr right = node_stack_pop(nodeStack);
                    NodePtr left = node_stack_pop(nodeStack);

                    // Validate operands
                    if (right->keyword == STACK_END || left->keyword == STACK_END) {
                        fprintf(stderr, "Invalid expression\n");
                        exit(2);
                    }

                    // Push operator node
                    node_stack_push(nodeStack,
                                    create_operator_node(processToken, left, right)
                    );
                    break;
                }
                default:
                    fprintf(stderr, "Invalid expression\n");
                    exit(2);
            }
        }
        // Shift
        else if (action == P_S) {
            if (token->type == T_LBRACKET) {
                openBracketCount++;
            }
            loadNewToken = 1;
            token_stack_push(tokenStack, token);
        }
        // Match brackets
        else if (action == P_E) {
            Token lBracToken = token_stack_pop(tokenStack);

            if (lBracToken->type != T_LBRACKET) {
                fprintf(stderr, "Invalid expression\n");
                exit(2);
            }
            if (openBracketCount == 0) {
                break;
            }
            openBracketCount--;

            // Move past the closing bracket
            get_token(token);
        }
        // End of parsing
        else if (action == P_END) {
            break;
        }
        // Invalid action
        else {
            fprintf(stderr, "Invalid expression\n");
            exit(2);
        }

    }

    // Final cleanup and validation
    NodePtr root = node_stack_pop(nodeStack);
    if (token_stack_top(tokenStack)->type != 57) {
        fprintf(stderr, "Invalid expression\n");
        exit(2);
    }
    if (node_stack_top(nodeStack) != NULL) {
        if (node_stack_top(nodeStack)->keyword != 57) {
            fprintf(stderr, "Invalid expression\n");
            exit(2);
        }
    }
    if (root->keyword == STACK_END) {
        root = NULL;
    }
    free_token_stack(tokenStack);
    free_node_stack(nodeStack);

    return root;
}

