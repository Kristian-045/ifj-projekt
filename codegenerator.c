/**
 * @file codegenerator.c
 * @brief Code generator for the IFJ project.
 *
 * @author Silvia Šlachtovská (xslachs00)
 *
 * @date 2024
 */

#define _GNU_SOURCE
#define _XOPEN_SOURCE 700
#define _POSIX_C_SOURCE 200809L

#include "codegenerator.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>



//typedef struct Node* NodePtr;

int labelCounter = 0;
static int count = 0;


char declaredVariables[100][50];
int declaredVariableCount = 0;


/**
 * Initializes a new instance of the CodeGenerator structure.
 *
 * @return A pointer to the new initialized CodeGenerator
 */
CodeGenerator* cg_init() {
    CodeGenerator *cg = (CodeGenerator *)malloc(sizeof(CodeGenerator));
    if (cg == NULL) {
        fprintf(stderr, "memory allocation error\n");
        exit(99);
    }

    cg->output = stdout;
    if (cg->output == NULL) {
        fprintf(stderr, "error opening output file\n");
        free(cg);
        exit(99);
    }

    return cg;
}

/**
 * Generates the header for the IFJcode24 program.
 *
 * This function outputs the initial setup for the IFJcode24 interpreter,
 * which are declaration of global variables and a jump to the `main` section.
 */
void cg_generate_header() {
    printf(".IFJcode24\n");

    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");

    printf("DEFVAR GF@val1\n");
    printf("DEFVAR GF@val2\n");
    printf("DEFVAR GF@bool1\n");
    printf("DEFVAR GF@bool2\n");

    printf("JUMP $main\n");
}

/**
 * Generates definitions for all built-in functions.
 */
void gen_built_in_fun(){
    read_string();
    read_int();
    read_float();
    write_term();
    convert_int2_float();
    convert_float2_int();
    fun_length();
    fun_concat();
    fun_substring();
    fun_strcmp();
    fun_ord_value();
    fun_char();
}

//Generates a DEFVAR instruction in IFJcode24.
void cg_defvar(CodeGenerator *cg, const char *frame, const char *variableName) {
    cg_write_instruction(cg, "DEFVAR %s@%s\n", frame, variableName);
}

// Generates a MOVE instruction to assign a value to a variable
void cg_move(CodeGenerator *cg, const char *destFrame, const char *dest_var, const char *src_symb) {
    cg_write_instruction(cg, "MOVE %s@%s %s\n", destFrame, dest_var, src_symb);
}

// Generates a CALL instruction to invoke a function by label
void cg_call(CodeGenerator *cg, const char *label) {
    cg_write_instruction(cg, "CALL $%s\n", label);
}

// Generates a RETURN instruction to exit a function
void cg_return(CodeGenerator *cg) {
    cg_write_instruction(cg, "RETURN\n");
}


/* Frame operations*/
void cg_createframe(CodeGenerator *cg) {
    cg_write_instruction(cg, "CREATEFRAME\n");
}

void cg_pushframe(CodeGenerator *cg) {
    cg_write_instruction(cg, "PUSHFRAME\n");
}

void cg_popframe(CodeGenerator *cg) {
    cg_write_instruction(cg, "POPFRAME\n");
}

/* Control flow: Labels and jumps */
void cg_label(CodeGenerator *cg, const char *label) {
    cg_write_instruction(cg, "LABEL $%s\n", label);
}

void cg_jump(CodeGenerator *cg, const char *label) {
    cg_write_instruction(cg, "JUMP %s\n", label);
}

void cg_jumpifeq(CodeGenerator *cg, const char *label, const char *symb1, const char *symb2) {
    cg_write_instruction(cg, "JUMPIFEQ %s %s %s\n", label, symb1, symb2);
}

void cg_jumpifneq(CodeGenerator *cg, const char *label, const char *symb1, const char *symb2) {
    cg_write_instruction(cg, "JUMPIFNEQ %s %s %s\n", label, symb1, symb2);
}

/* Arithmetic operations */
void cg_add(CodeGenerator *cg, const char *dest, const char *symb1, const char *symb2) {
    cg_write_instruction(cg, "ADD %s %s %s\n", dest, symb1, symb2);
}

void cg_sub(CodeGenerator *cg, const char *dest, const char *symb1, const char *symb2) {
    cg_write_instruction(cg, "SUB %s %s %s\n", dest, symb1, symb2);
}

void cg_mul(CodeGenerator *cg, const char *dest, const char *symb1, const char *symb2) {
    cg_write_instruction(cg, "MUL %s %s %s\n", dest, symb1, symb2);
}

void cg_div(CodeGenerator *cg, const char *dest, const char *symb1, const char *symb2) {
    cg_write_instruction(cg, "DIV %s %s %s\n", dest, symb1, symb2);
}

void cg_idiv(CodeGenerator *cg, const char *dest, const char *symb1, const char *symb2) {
    cg_write_instruction(cg, "IDIV %s %s %s\n", dest, symb1, symb2);
}

/* Stack Operations */
void cg_pushs(CodeGenerator *cg, const char *symb) {
    cg_write_instruction(cg, "PUSHS %s\n", symb);
}

void cg_pops(CodeGenerator *cg, const char *var) {
    cg_write_instruction(cg, "POPS %s\n", var);
}

/* Input and output operations */
void cg_read(CodeGenerator *cg, const char *var, const char *type) {
    cg_write_instruction(cg, "READ %s %s\n", var, type);
}

void cg_write(CodeGenerator *cg, const char *symb) {
    cg_write_instruction(cg, "WRITE %s\n", symb);
}

/*String operations*/

// Calculate the length of a string and store it in the destination variable
void cg_strlen(CodeGenerator *cg, const char *dest, const char *symb) {
    cg_write_instruction(cg, "STRLEN %s %s\n", dest, symb);
}

// Retrieve a character from a string at a specified position and store it in the destination variable 
void cg_getchar(CodeGenerator *cg, const char *dest, const char *symb1, const char *symb2) {
    cg_write_instruction(cg, "GETCHAR %s %s %s\n", dest, symb1, symb2);
}

// Replace a character in a string at a specified position with another character 
void cg_setchar(CodeGenerator *cg, const char *var, const char *symb1, const char *symb2) {
    cg_write_instruction(cg, "SETCHAR %s %s %s\n", var, symb1, symb2);
}

/* Logical Operations*/

// Compare two symbols for less-than condition
void cg_lt(CodeGenerator *cg,const char *var, const char *symb1, const char *symb2) {
    cg_write_instruction(cg, "LT LF@%s %s %s\n", var, symb1, symb2);
}

// Compare two symbols for greater-than condition
void cg_gt(CodeGenerator *cg,const char *var, const char *symb1, const char *symb2) {
    cg_write_instruction(cg, "GT LF@%s %s %s\n", var, symb1, symb2);
}

// Compare two symbols for equality
void cg_eq(CodeGenerator *cg,const char *var, const char *symb1, const char *symb2) {
    cg_write_instruction(cg, "EQ LF@%s %s %s\n", var, symb1, symb2);
}

// Perform logical AND operation
void cg_and(CodeGenerator *cg,const char *var, const char *symb1, const char *symb2) {
    cg_write_instruction(cg, "AND %s %s %s\n", var, symb1, symb2);
}

// Perform logical OR operation
void cg_or(CodeGenerator *cg,const char *var, const char *symb1, const char *symb2) {
    cg_write_instruction(cg, "OR %s %s %s\n", var, symb1, symb2);
}

// Perform logical NOT operation
void cg_not(CodeGenerator *cg,const char *var, const char *symb) {
    cg_write_instruction(cg, "NOT %s %s %s\n", var, symb);
}

/*
* Helper function to write an instruction to the code generator's output
*/
void cg_write_instruction(CodeGenerator *cg, const char *format, ...) {
    if (cg == NULL || cg->output == NULL) {
        return;
    }
    va_list args;
    va_start(args, format);

    //write the formatted instruction to the output stream
    vfprintf(cg->output, format, args);

    va_end(args);
}

/*
* String transformation function 
* changes string to IFJcode24 format
* @param input: The original string to transform. Must not be NULL.
*
*/
char* rewrite_string(const char *input) {
    size_t bufferSize = 128;
    size_t outputLength = 0;
    char *output = (char *)malloc(bufferSize);

    if (output == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(99);
    }

    // Add "string@" prefix
    const char *prefix = "string@";
    size_t prefixLength = strlen(prefix);
    if (prefixLength >= bufferSize) {
        bufferSize = prefixLength + 1;
        output = safe_realloc(output, bufferSize);
    }
    strcpy(output, prefix);
    outputLength += prefixLength;

    //process each character in the input string
    for (size_t i = 0; input[i]; i++) {
        char c = input[i];

        // handle special characters
        if (c == 35 || c == 92 || c <= 32) {
            char xyz[6];
            snprintf(xyz, sizeof(xyz), "\\%03d", c);

            // Ensure enough buffer space for the escaped character
            size_t addLength = strlen(xyz);
            if (outputLength + addLength >= bufferSize) {
                bufferSize = (outputLength + addLength) * 2;
                output = safe_realloc(output, bufferSize);
            }

            strcat(output, xyz);
            outputLength += addLength;
        } else {
            // handle regular characters
            if (outputLength + 1 >= bufferSize) {
                bufferSize = outputLength +  2;
                output = safe_realloc(output, bufferSize);
            }

            //apend character
            output[outputLength++] = c;

            // '\0' terminated
            output[outputLength] = '\0';
        }
    }

    return output;
}


/**
 * Generates a literal representation for IFJcode24 from a syntax tree node.
 *
 * @param node A pointer to the syntax tree node containing the data to be converted.
 *
 */
char* cg_literal(NodePtr node) {
    if (node == NULL) {
        return NULL;
    }

    char *result = NULL;

    // Handle different node types and generate the corresponding literal
    switch(node->keyword) {
        case T_INT : {
            // Generate an integer literal (e.g., int@25)
            result = format_string("int@%d", node->data.int_val);
            break;
        }
        case T_FLOAT: {
            // Generate a float literal using hexadecimal representation (e.g., float@0x1p+3)
            result = format_string("float@%a", node->data.float_val);
            break;
        }

        case T_STRING: {
            // Rewrite the string to escape special characters and generate a string literal
            char *new_string = rewrite_string(node->data.string_val);
            result = format_string("%s", new_string);
            free(new_string);
            break;
        }
        case T_NULL: {
            // Generate a nil literal (e.g., nil@nil)
            const char *nil_string = "nil@nil";
            result = malloc(strlen(nil_string) + 1);
            if (result == NULL) {
                fprintf(stderr, "memory allocation error\n");
                exit(99);
            }
            strcpy(result, nil_string);
            break;
        }
        case T_ID: {
            // Generate a variable reference (e.g., LF@varName)
            result = format_string("LF@%s", node->data.string_val);
            break;
        }
        default: {
            // Handle unexpected node types
            fprintf(stderr, "doslo k chybe: %d\n", node->keyword);
            exit(53);
        }
    }

    return result;
}


/**
 * Generates the code for calling a function.
 *
 * @param cg A pointer to the CodeGenerator
 * @param callNode A pointer to the syntax tree node representing the function call. 
 *        This node should contain the function name and arguments.
 */
void generate_function_call(CodeGenerator *cg, NodePtr callNode) {
    if (cg == NULL || callNode == NULL || callNode->data_type != STRING || callNode->data.string_val == NULL) {
        fprintf(stderr, "invalid function call node\n");
        return;
    }

    // Extract arguments list from the function call node
    NodePtr argNode = callNode->left;
    int argCount = 1;

    // Create a new temporary frame for function arguments
    cg_createframe(cg);

    NodePtr paramsNode = argNode->right;

    while (paramsNode != NULL) {
        if(paramsNode == NULL ) {
            fprintf(stderr, "Invalid argument node structure\n");
            return;
        }

        // Generate a literal or variable representation for the argument
        char *arg_value = cg_literal(paramsNode);

        // Define a variable for the argument in the temporary frame
        cg_write_instruction(cg, "DEFVAR TF@arg%d\n", argCount);

        // Move the argument value to the defined variable
        cg_write_instruction(cg, "MOVE TF@arg%d %s\n", argCount, arg_value);

        // Free the allocated memory for the argument value
        free(arg_value);

        // Move to the next argument
        argCount++;
        paramsNode = paramsNode->right;
    }

    // Call the function by its name
    cg_call(cg, callNode->data.string_val);

}

/**
 * Generates code for a built-in function call.
 *
 * @param cg A pointer to the CodeGenerator
 * @param ifj_callNode AST node representing the call to the built-in function
 *        - left child of the node should contain the function name
 *        - right child should contain the arguments as a linked list of `FN_PARAM` nodes
 */
void generate_builtin_call(CodeGenerator *cg, NodePtr ifj_callNode) {
    if (cg == NULL || ifj_callNode == NULL || ifj_callNode->keyword != T_IFJ) {
        fprintf(stderr, "Invalid T_IFJ node\n");
        exit(99);
    }

    // Create a new frame for the built-in function call
    cg_createframe(cg);

    // Extract the function name and arguments from the node
    NodePtr functionName = ifj_callNode->left;
    NodePtr arguments = ifj_callNode->right;

    if (functionName == NULL || functionName->data_type != STRING) {
        fprintf(stderr, "Invalid function name in T_IFJ node\n");
        exit(99);
    }

    const char *builtinFunName = functionName->data.string_val;

    // Process each argument and prepare it in the temporary frame
    int argIndex = 1;
    while (arguments != NULL && arguments->keyword == FN_PARAM) {
        char *arg_value = cg_literal(arguments->right);
        cg_write_instruction(cg, "DEFVAR TF@arg%d\n", argIndex);
        cg_write_instruction(cg, "MOVE TF@arg%d %s\n", argIndex, arg_value);
        free(arg_value);

        argIndex++;
        arguments = arguments->left;
    }


    // Generate a CALL instruction for the corresponding built-in function
    if (strcmp(builtinFunName, "ifj.readstr" ) == 0 || strcmp(builtinFunName, "readstr" ) == 0 ) {
        cg_write_instruction(cg, "CALL $reads");
    }
    else if (strcmp(builtinFunName, "ifj.readi32" ) == 0 || strcmp(builtinFunName, "readi32" ) == 0) {
        cg_write_instruction(cg, "CALL $readi");
    }
    else if (strcmp(builtinFunName, "ifj.readf64") == 0 || strcmp(builtinFunName, "readf64") == 0) {
        cg_write_instruction(cg, "CALL $readf\n");
    }
    else if (strcmp(builtinFunName, "ifj.write") == 0 || strcmp(builtinFunName, "write") == 0) {
        cg_write_instruction(cg, "CALL $write_term\n");
    }
    else if (strcmp(builtinFunName, "ifj.i2f") == 0 || strcmp(builtinFunName, "i2f") == 0) {
        cg_write_instruction(cg, "CALL $int2float\n");
    }
    else if (strcmp(builtinFunName, "ifj.f2i") == 0 || strcmp(builtinFunName, "f2i") == 0) {
        cg_write_instruction(cg, "CALL $float2int\n");
    }
    else if (strcmp(builtinFunName, "ifj.length") == 0 || strcmp(builtinFunName, "length") == 0) {
        cg_write_instruction(cg, "CALL $length\n");
    }
    else if (strcmp(builtinFunName, "ifj.concat") == 0 || strcmp(builtinFunName, "concat") == 0) {
        cg_write_instruction(cg, "CALL $concat\n");
    }
    else if (strcmp(builtinFunName, "ifj.substring") == 0 || strcmp(builtinFunName, "substring") == 0) {
        cg_write_instruction(cg, "CALL $substring\n");
    }
    else if (strcmp(builtinFunName, "ifj.strcmp") == 0 || strcmp(builtinFunName, "strcmp") == 0) {
        cg_write_instruction(cg, "CALL $strcmp\n");
    }
    else if (strcmp(builtinFunName, "ifj.ord") == 0 || strcmp(builtinFunName, "ord") == 0) {
        cg_write_instruction(cg, "CALL $ord\n");
    }
    else if (strcmp(builtinFunName, "ifj.char") == 0 || strcmp(builtinFunName, "char") == 0) {
        cg_write_instruction(cg, "CALL $char\n");
    }
    else {
        fprintf(stderr, "Unknown built-in function: %s\n", builtinFunName);
        exit(99);
    }
}

/**
 * Generates the initial setup for a function 
 *
 * @param cg A pointer to the CodeGenerator
 * @param funName The name of the function being defined, used as the label.
 */
void cg_function_begin(CodeGenerator *cg, const char *funName) {
    cg_label(cg, funName);
    cg_createframe(cg);
    cg_pushframe(cg);
}

/**
 * Sets up function parameters by transferring values from the TF to 
 * LF for the function being defined.
 *
 * @param cg A pointer to the CodeGenerator
 * @param parameterList A node representing the linked list of function parameters in the AST
 */
void cg_function_def_params(CodeGenerator *cg, NodePtr parameterList) {
    if (cg == NULL || parameterList == NULL) {
        return;
    }

    int paramIndex = 0;
    NodePtr paramNode = parameterList;

    // going through all parameters in the parameter list
    while (paramNode != NULL && paramNode->keyword == FN_PARAM) {
        // Ensure the current parameter node has a valid string name
        if (paramNode->data_type == STRING && paramNode->data.string_val != NULL) {
            // Define the parameter in the local frame
            cg_defvar(cg, "LF", paramNode->data.string_val);
            cg_write_instruction(cg, "MOVE LF@%s nil@nil", paramNode->data.string_val);

            // Move the parameter value from TF (argN) to LF  
            cg_write_instruction(cg, "MOVE LF@%s TF@%%%d\n", paramNode->data.string_val, paramIndex);

            // Increment the index for the next parameter
            paramIndex++;
        }
        // Move to the next parameter in the list
        paramNode = paramNode->right;
    }
}


/**
 * @brief Finalizes a function definition by popping the local frame and generating a return instruction
 * 
 * @param cg Pointer to the code generator structure.
 */
void cg_fun_end(CodeGenerator *cg) {
    if (cg == NULL) {
        return;
    }
    cg_popframe(cg);
}


/**
 * @brief Generates code for a user-defined function.
 *
 * This function handles the setup, parameter management, and body generation for a function. 
 *
 * @param cg Pointer to the code generator structure.
 * @param functionNode Pointer to the AST node representing the function definition.
 */
void generate_function(CodeGenerator *cg, NodePtr functionNode) {
    if (cg == NULL || functionNode == NULL) {
        return;
    }

    // Generate the function label and setup
    if(functionNode->data_type == STRING  && functionNode->data.string_val != NULL) {
        cg_function_begin(cg, functionNode->data.string_val);
    }

    // Handle function parameters and return value setup
    NodePtr funDataNode = functionNode->left;
    if (funDataNode != NULL && funDataNode->keyword == FN_DATA) {
        NodePtr parameterList = funDataNode->left;
        cg_function_def_params(cg, parameterList);

        NodePtr returnNode = funDataNode->right;
        if(returnNode->keyword != T_VOID && returnNode != NULL) {
            cg_write_instruction(cg, "DEFVAR LF@retval1\n");
            cg_write_instruction(cg, "MOVE LF@retval1 nil@nil\n");

        }
    }

    // Generate the body of the function
    NodePtr bodyNode = functionNode->right;
    if(bodyNode != NULL) {
        // Generate code for the function body
        generate_block(cg, bodyNode);
    }

    // Finalize the function with cleanup and return
    cg_fun_end(cg);

    if(functionNode->left->right->keyword != T_VOID) {
        cg_return(cg);
    }
}


/**
 * @brief Generates code for an assignment operation.
 *
 * This function handles the code generation for assigning the result of an expression to a variable
 *
 * @param cg Pointer to the code generator structure.
 * @param assignNode Pointer to the AST node representing the assignment operation.
 */
void generate_assignment(CodeGenerator *cg, NodePtr assignNode) {
    if (cg == NULL || assignNode == NULL) {
        return;
    }

    // Target variable on the left-hand side of the assignment
    NodePtr variableNode = assignNode->left;
    if (variableNode == NULL || variableNode->data_type != STRING || variableNode->data.string_val == NULL) {
        fprintf(stderr, "Invalid target variable\n");
        return;
    }

    // Frame and variable name for the target
    char *frame = "LF";
    char *variableName = variableNode->data.string_val;

    // Declare the variable if it has not already been declared
    if (!is_variable_declared(variableName)) {
        cg_defvar(cg, frame, variableName);
        cg_write_instruction(cg, "MOVE LF@%s nil@nil\n", variableName);
        add_variable_to_symbol_table(variableName);
    }

    //variable TO DO ABY TO NESLO CEZ TMP
    char *variable = NULL;
    variable = format_string("LF@%s", variableName);


    // Expression on the right-hand side of the assignment
    NodePtr expressionNode = assignNode->right;
    if (expressionNode == NULL) {
        fprintf(stderr, "Invalid expression\n");
        return;
    }

    // Temporary variable to store the result of the expression evaluation
    //char tempVariable[20];
    //sprintf(tempVariable, "LF@temp%d", count++);
    //cg_write_instruction(cg, "DEFVAR %s\n", tempVariable);
    //cg_write_instruction(cg, "MOVE %s nil@nil\n", tempVariable);

    // Generate code for the expression and store the result in the temporary variable
    generate_expression(cg, expressionNode, variable);


    // Move the result from the temporary variable to the target variable
    //cg_move(cg, frame, variableName, tempVariable);
}


/**
 * @brief Generates code for evaluating an expression and stores the result in a variable
 *
 * @param cg Pointer to the code generator structure
 * @param expressionNode Pointer to the AST node representing the expression
 * @param result The target variable where the result of the expression will be stored
 */
void generate_expression(CodeGenerator *cg, NodePtr expressionNode, char *result) {
    if (cg == NULL || expressionNode == NULL || result == NULL) {
        return;
    }

    switch(expressionNode->keyword) {
        case T_INT: {
            // Handle integer literals.
            cg_write_instruction(cg, "MOVE %s int@%d\n", result, expressionNode->data.int_val);
            break;
        }
        case(T_FLOAT) : {
            // Handle floating-point literals.
            cg_write_instruction(cg, "MOVE %s float@%a\n", result, expressionNode->data.float_val);
            break;
        }
        case(T_STRING) : {
            // Handle string literals.
            char* new_string = rewrite_string(expressionNode->data.string_val);
            cg_write_instruction(cg, "MOVE %s %s\n", result, new_string);
            free(new_string);
            break;
        }
        case(T_ID) :{
            // Handle variables.
            if(expressionNode->data_type == STRING) {
                cg_write_instruction(cg, "MOVE %s LF@%s\n", result, expressionNode->data.string_val);
            }
            break;
        }
        case T_PLUS:
        case T_MINUS:
        case T_ASTERISK:
        case T_SLASH: {
            // Handle arithmetic operations.
            const char *op1 = generate_temp_var(cg, expressionNode->left);
            const char *op2 = generate_temp_var(cg, expressionNode->right);

            switch(expressionNode->keyword) {
                case T_PLUS:
                    cg_write_instruction(cg, "ADD %s %s %s\n", result, op1, op2);
                    break;
                case T_MINUS:
                    cg_write_instruction(cg, "SUB %s %s %s\n", result, op1, op2);
                    break;
                case T_ASTERISK:
                    cg_write_instruction(cg, "MUL %s %s %s\n", result, op1, op2);
                    break;
                case T_SLASH:
                    cg_write_instruction(cg, "DIV %s %s %s\n", result, op1, op2);
                    break;
                default:
                    fprintf(stderr, "Invalid %d\n", expressionNode->keyword);
//                    exit(-1);
            }
            break;
        }
        case T_GREATER:
        case T_GREATEREQUAL:
        case T_LESS:
        case T_LESSEQUAL:
        case T_EQUALS:
        case T_NOTEQUAL: {
            // Handle relational operators. 
            NodePtr left = expressionNode->left;
            NodePtr right = expressionNode->right;
            char *op1 = generate_temp_var(cg, left);
            char *op2 = generate_temp_var(cg, right);

            switch (expressionNode->keyword) {
                case T_GREATER:
                    cg_gt(cg, result, op1, op2);
                    break;
                case T_GREATEREQUAL:
                    cg_lt(cg, result, op1, op2);
                    cg_not(cg, result, result);
                    break;
                case T_LESS:
                    cg_lt(cg, result, op1, op2);
                    break;
                case T_LESSEQUAL:
                    cg_gt(cg,  result, op1, op2);
                    cg_not(cg, result, result);
                    break;
                case T_EQUALS:
                    cg_eq(cg, result, op1, op2);
                    break;
                case T_NOTEQUAL:
                    cg_eq(cg, result, op1, op2);
                    cg_not(cg,result, result);
                    break;
                default:
                    fprintf(stderr, "invalid relational operation\n");
//                    exit(-1);
            }
            break;
        }
        default:
            fprintf(stderr, "invalid type in expression %d\n", expressionNode->keyword);
//            exit(-1);
    }
}

/**
 * @brief Generates code for a return statement, including evaluating the return expression.
 *
 * If the return statement includes an expression, it is evaluated, and the result is stored
 * in the `LF@retval1` variable
 * 
 * @param cg Pointer to the code generator structure
 * @param returnNode Pointer to the AST node representing the return statement.
 */
void generate_return(CodeGenerator *cg, NodePtr returnNode) {
    if (cg == NULL || returnNode == NULL) {
        return;
    }

    // Pointer to the expression node in the return statement
    NodePtr exprNode = returnNode->right;

    // Check if there is an expression to evaluate
    if (returnNode->right != NULL) {
        char result[20];
        snprintf(result, sizeof(result), "LF@retval1");

        // Generate code for the return expression and store the result in LF@retval1
        generate_expression(cg, exprNode, result);
    }

}


/**
 * @brief Generates a temporary variable in the temporary frame for evaluating expressions.
 *
 * @param cg Pointer to the code generator structure.
 * @param expressionNode Pointer to the AST node representing the expression to evaluate.
 */
char *generate_temp_var(CodeGenerator *cg, NodePtr expressionNode) {
    if (cg == NULL || expressionNode == NULL) {
        fprintf(stderr, "Invalid input to generate_temp_var\n");
        return NULL;
    }


    // Allocate memory for the temporary variable name
    char *tempVariable = malloc(20 * sizeof(char));
    if (!tempVariable) {
        fprintf(stderr, "Memory allocation error\n");
        exit(99);
    }

    // Generate code based on the type of the expression node
    switch (expressionNode->keyword) {
        case T_INT: {
            // Temporary variable for an integer literal
            snprintf(tempVariable, 20, "LF@temp%d", count++);
            cg_write_instruction(cg, "DEFVAR %s\n", tempVariable);
            cg_write_instruction(cg, "MOVE %s int@%d\n", tempVariable, expressionNode->data.int_val);
            break;
        }
        case T_FLOAT: {
            // Temporary variable for a float literal
            snprintf(tempVariable, 20, "LF@temp%d", count++);
            cg_write_instruction(cg, "DEFVAR %s\n", tempVariable);
            cg_write_instruction(cg, "MOVE %s float@%a\n", tempVariable, expressionNode->data.float_val);
            break;
        }
        case T_STRING: {
            // Temporary variable for a string literal
            snprintf(tempVariable, 20, "LF@temp%d", count++);
            cg_write_instruction(cg, "DEFVAR %s\n", tempVariable);
            char *rewritten_string = rewrite_string(expressionNode->data.string_val);
            cg_write_instruction(cg, "MOVE %s %s\n", tempVariable, rewritten_string);
            free(rewritten_string);
            break;
        }
        case T_ID: {
            // Temporary variable for an identifier
            snprintf(tempVariable, 20, "LF@temp%d", count++);
            cg_write_instruction(cg, "DEFVAR %s\n", tempVariable);
            cg_write_instruction(cg, "MOVE %s LF@%s\n", tempVariable, expressionNode->data.string_val);
            break;
        }
        default: {
            sprintf(tempVariable, "temp%d", count++);
            cg_defvar(cg, "LF", tempVariable);
            break;
        }
    }
    return tempVariable;
}

/**
 * @brief Generates code for an IF-ELSE block.
 *
 * @param cg Pointer to the code generator structure.
 * @param ifElseNode Pointer to the AST node representing the IF-ELSE structure.
 */
void generate_if_else(CodeGenerator *cg, NodePtr ifElseNode) {
    if( cg == NULL || ifElseNode == NULL || ifElseNode->keyword != NEW_COMMAND) {
        fprintf(stderr, "invalid T_IF node\n");
        exit(99);
    }

    // Generate unique labels for the ELSE and END sections
    int currentLabel = labelCounter++;
    char elseLabel[20];
    char endLabel[20];
    snprintf(elseLabel, sizeof(elseLabel), "ELSE_%d", currentLabel);
    snprintf(endLabel, sizeof(endLabel), "ENDIF_%d", currentLabel);

    // Move to the T_IF node
    NodePtr ifNode = ifElseNode->right;
    if (ifNode == NULL || ifNode->keyword != T_IF) {
        fprintf(stderr, "Expected T_IF node\n");
        exit(99);
    }

    // Get the IF_DATA node
    NodePtr ifDataNode = ifNode->left;
    if (ifDataNode == NULL || ifDataNode->keyword != IF_DATA) {
        fprintf(stderr, "invalid ifDataNode node\n");
        exit(99);
    }

    // Evaluate the condition
    NodePtr condition = ifDataNode->left;
    if (condition == NULL) {
        fprintf(stderr, "Condition is missing\n");
        exit(99);
    }

    //generate condition expression
    char *conditionResult = generate_temp_var(cg, condition);
    generate_expression(cg, condition, conditionResult);


    // Conditional jump to ELSE if the condition is false
    cg_write_instruction(cg, "JUMPIFNEQ $%s LF@%s bool@true\n", elseLabel, conditionResult);


    // Generate code for the IF body
    NodePtr ifBodyNode = ifNode->right;
    if (ifBodyNode != NULL) {
        generate_block(cg, ifBodyNode);
    }


    // Unconditional jump to END after IF body
    cg_write_instruction(cg, "JUMP $%s\n", endLabel);


    // Generate code for the ELSE body if it exists
    NodePtr elseNode = ifElseNode->left->right;
    if (elseNode != NULL) {
        cg_label(cg, elseLabel);

        NodePtr elseBodyNode = elseNode->right;
        if(elseBodyNode != NULL) {
            generate_block(cg, elseBodyNode);
        }
    }

    // Mark the END of the IF-ELSE structure
    cg_label(cg, endLabel);
}


/**
 * @brief Generates IFJcode24 instructions for a WHILE loop.
 *
 * @param cg Pointer to the code generator structure.
 * @param whileNode Pointer to the AST node representing the WHILE loop.
 */
void generate_while(CodeGenerator *cg, NodePtr whileNode) {
    if(cg == NULL || whileNode == NULL || whileNode->keyword != T_WHILE) {
        fprintf(stderr, "Invalid whileNode\n");
        exit(99);
    }

    // Generate unique labels for the WHILE start and end
    char startLabel[20];
    char endLabel[20];
    snprintf(startLabel, sizeof(startLabel), "WHILE_START_%d", labelCounter++);
    snprintf(endLabel, sizeof(endLabel), "WHILE_END_%d", labelCounter++);

    // Label for the start of the WHILE loop
    cg_label(cg, startLabel);

    // Retrieve the DATA_WHILE node
    NodePtr whileDataNode = whileNode->left;
    if (whileDataNode == NULL || whileDataNode->keyword != WHILE_DATA) {
        fprintf(stderr, "Invalid WHILE DATA node\n");
        exit(99);
    }

    // Retrieve the condition node
    NodePtr condition = whileDataNode->left;
    if (condition == NULL) {
        fprintf(stderr, "Condition expression is missing\n");
        exit(99);
    }

    // Generate code for the condition evaluation
    char *conditionResult = generate_temp_var(cg, condition);
    generate_expression(cg, condition, conditionResult);

    // Conditional jump to the end of the loop if the condition is false
    cg_write_instruction(cg, "JUMPIFNEQ $%s LF@%s bool@true\n", endLabel, conditionResult);


    // Generate code for the WHILE loop body
    NodePtr whileBodyNode = whileNode->right;
    if (whileBodyNode != NULL) {
        generate_block(cg, whileBodyNode);
    }

    // Unconditional jump back to the start of the loop
    cg_write_instruction(cg, "JUMP $%s\n", startLabel);

    // Label for the end of the WHILE loop
    cg_label(cg, endLabel);

}

/**
 * @brief Generates IFJcode24 instructions for variable or constant declarations
 *
 * @param cg Pointer to the code generator structure.
 * @param declarationNode Pointer to the AST node representing the declaration.
 */
void generate_declaration(CodeGenerator *cg, NodePtr declarationNode) {
    if (cg == NULL || declarationNode == NULL) {
        fprintf(stderr, "invalid declaration node\n");
        exit(99);
    }

    if (declarationNode->keyword != T_VAR && declarationNode->keyword != T_CONST) {
        fprintf(stderr, "unexpected value\n");
        exit(99);
    }

    if (declarationNode->data_type != STRING) {
        fprintf(stderr, "invalid id\n");
        exit(99);
    }

    // Extract the variable name
    char *variableName = declarationNode->data.string_val;
    //printf("som v declaracii: %s\n", variableName);


    // Check if the declaration includes an assignment
    NodePtr equalSign = declarationNode->left;   // Check for `=` operator
    if (equalSign != NULL && equalSign->keyword == T_EQUALSIGN) {

        // Handle the value assigned to the variable
        NodePtr value_node = equalSign->right;
        if (value_node != NULL) {

            // Generate the literal value for the assigned expression
            char *value = cg_literal(value_node);

            // Write the instruction to assign the value to the declared variable
            cg_write_instruction(cg, "MOVE %s@%s %s\n", "LF", variableName, value);
            free(value);
        }
    }
}


/**
 * @brief Generates IFJcode24 instructions for a block of AST nodes.
 *
 * This function iterates through a linked list of nodes in a block and generates code
 * for each node based on its type (e.g., declarations, assignments, loops, conditionals).
 *
 * @param cg Pointer to the code generator structure.
 * @param blockNode Pointer to the AST node representing the block to be processed.
 */
void generate_block(CodeGenerator *cg, NodePtr blockNode) {
    if (cg == NULL || blockNode == NULL) {
        return;
    }

    NodePtr current = blockNode;

    //debugging stuff
    /*printf("current keyword is: %d\n", current->keyword);
    if (current->left != NULL) {
        printf("current->left keyword is: %d\n", current->left->keyword);
    }
    else if (current->right != NULL) {
        printf("current-right keyword is: %d\n", current->right->keyword);
    }*/

    // Iterate through all nodes in the block
    while (current != NULL) {
        switch (current->keyword) {

            case T_CONST:
            case T_VAR:
                // Handle variable or constant declarations
                generate_declaration(cg, current);
                return;
                break;

            case T_ID:
                // Determine if the node represents an assignment or a function call
                if (current->left != NULL && current->left->keyword == T_EQUALSIGN) {
                    generate_assignment(cg, current);
                } else if (current->right != NULL && current->right->keyword == FN_CALL) {
                    generate_function_call(cg, current);
                } else {
                    fprintf(stderr, "Invalid use of identifier  %s \n", current->data.string_val );
                    exit(99);
                }
                return;
                break;

            case T_RETURN:
                // Handle return statement
                generate_return(cg, current);
                return;
                break;

            case T_IF:
                // Handle if-else blocks
                generate_if_else(cg, current);
                return;
                break;

            case T_WHILE:
                // Handle while loops
                generate_while(cg, current);
                return;
                break;

            case T_FN:
                // Handle function definitions
                generate_function(cg, current);
                return;
                break;
            case FN_CALL:
                // Handle function calls
                generate_function_call(cg, current);
                return;
                break;
            case T_IFJ:
                // Handle built-in function calls
                generate_builtin_call(cg, current);
                return;
                break;
            case T_U8:
            case T_ELSE:
            case FN_DATA:
            case WHILE_DATA:
            case T_F64:
            case T_I32:
            case START:
            case T_VOID:
            case IF_ELSE_BODY:
                // Process left and right subtrees for specific types
                if (current->left != NULL) {
                    generate_block(cg, current->left);
                }
                else if (current->right != NULL) {
                    generate_block(cg, current->right);
                }
                else {
                    break;
                }
                return;
                break;
            case T_GREATER:
            case T_AND:
            case T_NOT:
            case T_EQUALS:
            case T_PLUS:
            case T_MINUS:
            case T_ASTERISK:
            case T_SLASH:
            case T_NOTEQUAL:
            case T_GREATEREQUAL: {
                char *result = NULL;
                result = format_string("LF@%s", current->left->data.string_val);
                generate_expression(cg, current->left, result);
                return;
                break;
            }
            case NEW_COMMAND:
                if (current->right == NULL) {
                    return;
                }
                if (current->right->keyword == T_IF && current->left->keyword == NEW_COMMAND) {
                    generate_if_else(cg, current);
                } else if (current->right->keyword != T_IF) {
                    generate_block(cg, current->right);

                }
                // Avoid reprocessing `current->left` here.
                break;

            case T_LESS:  {
                // Handle less-than operator
                const char *result = generate_temp_var(cg, current);
                char *op1 = generate_temp_var(cg, current->left);
                char *op2 = generate_temp_var(cg, current->right);
                cg_write_instruction(cg,"LT %s %s %s", result, op1, op2);
                return;
                break;
            }

            case T_LESSEQUAL:{
                // Handle less-than-or-equal-to operator
                char *result = generate_temp_var(cg, current);
                char *op1 = generate_temp_var(cg, current->left);
                char *op2 = generate_temp_var(cg, current->right);
                cg_gt(cg,  result, op1, op2);
                cg_not(cg, result, result);
                return;
                break;
            }

            case T_EQUALSIGN:
                // Handle assignment or function call on the right-hand side
                if (current->right->keyword == FN_CALL) {
                    generate_function_call(cg, current->right);
                }
                else {
                    generate_assignment(cg, current);
                }
                return;
                break;
            default:
                fprintf(stderr, "Invalid command in block: %d\n", current->keyword);
                exit(99);
        }
        // Move to the next node in the block
        current = current->left;


    }
}


/**
 * Creates a duplicate of a given string.
 *
 * @param str The string to duplicate. Must not be NULL.
 * @return A pointer to the duplicated string (allocated on the heap), or NULL if allocation fails.
 */
char* my_strdup(const char *str) {
    if (str == NULL) return NULL;
    size_t len = strlen(str) + 1;
    char *copy = malloc(len);
    if (copy != NULL) {
        memcpy(copy, str, len);
    }
    return copy;
}


/** 
* checks if variable is already declared
* @param name The name of the variable to check.
* @return true if the variable is already declared, false otherwise.
*/
bool is_variable_declared(const char *name) {
    for (int i = 0; i < declaredVariableCount; i++) {
        if (strcmp(declaredVariables[i], name) == 0) {
            return true;
        }
    }
    return false;
}

/**
 * Adds a variable to the symbol table if it has not been declared yet.
 *
 * @param name The name of the variable to add.
 */
void add_variable_to_symbol_table(const char *name) {
    if (!is_variable_declared(name)) {
        strcpy(declaredVariables[declaredVariableCount++], name);
    }
}

/**
 * Safely reallocates memory to a new size.
 *
 * @param ptr The pointer to the memory block to be reallocated. It can be NULL.
 * @param new_size The new size in bytes for the memory block.
 * @return A pointer to the reallocated memory. If reallocation fails, the program exits with an error code 99.
 */
void* safe_realloc(void *ptr, size_t new_size) {
    void *new_ptr = realloc(ptr, new_size);
    if (new_ptr == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(99);
    }
    return new_ptr;
}

/**
 * Helper function to format a string with a variable argument list
 *
 * @param format The format string, similar to `printf`.
 * @param ... The variable arguments to format into the string.
 *
 */
char *format_string(const char *format, ...) {
    va_list args;
    va_start(args, format);

    // Calculate the size required for the formatted string
    size_t size = vsnprintf(NULL, 0, format, args) + 1;
    va_end(args);

    // Allocate memory for the formatted string
    char *result = (char *)malloc(size);
    if (result == NULL) {
        fprintf(stderr, "memory allocation error\n");
        exit(99);
    }

    // Format the string into the allocated buffer
    va_start(args, format);
    vsnprintf(result, size, format, args);
    va_end(args);

    return result;
}
