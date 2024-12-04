/**
 * @file codegenerator.h
 * @brief Code generator for the IFJ project.
 *
 * @author Silvia Šlachtovská (xslachs00)
 *
 * @date 2024
 */
#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L

#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H
#include <stdio.h>
#include <string.h>
#include "scanner.h"
#include "parser.h"
#include "builtinfun.h"

//typedef struct Node* NodePtr;

//struct for code generator
typedef struct CodeGenerator {
    FILE *output;
} CodeGenerator;

// function declarations

// initializes the code generator 
CodeGenerator* cg_init();

// generates the header 
void cg_generate_header();

//generates built in functions
void gen_built_in_fun();

// Declares a variable in the specified frame.
void cg_defvar(CodeGenerator *cg, const char *frame, const char *var_name);

// Moves a symbol's value to a variable.
void cg_move(CodeGenerator *cg, const char *dest_frame, const char *dest_var, const char *src_symb);

// Generates CALL 
void cg_call(CodeGenerator *cg, const char *label);

// Generates RETURN
void cg_return(CodeGenerator *cg);

// Frame operations: create, push, or pop a frame.
void cg_createframe(CodeGenerator *cg);
void cg_pushframe(CodeGenerator *cg);
void cg_popframe(CodeGenerator *cg);

// Generates labels and jumps for control flow.
void cg_label(CodeGenerator *cg, const char *label);
void cg_jump(CodeGenerator *cg, const char *label);
void cg_jumpifeq(CodeGenerator *cg, const char *label, const char *symb1, const char *symb2);
void cg_jumpifneq(CodeGenerator *cg, const char *label, const char *symb1, const char *symb2);

// Arithmetic operations: ADD, SUB, MUL, DIV, IDIV.
void cg_add(CodeGenerator *cg, const char *dest, const char *symb1, const char *symb2);
void cg_sub(CodeGenerator *cg, const char *dest, const char *symb1, const char *symb2);
void cg_mul(CodeGenerator *cg, const char *dest, const char *symb1, const char *symb2);
void cg_div(CodeGenerator *cg, const char *dest, const char *symb1, const char *symb2);
void cg_idiv(CodeGenerator *cg, const char *dest, const char *symb1, const char *symb2);

// stack operations 
//PUSHS <symb>
void cg_pushs(CodeGenerator *cg, const char *symb);
//POPS <var>
void cg_pops(CodeGenerator *cg, const char *var);

// READ <var> <type> -> nacita typ a vrati vo var
void cg_read(CodeGenerator *cg, const char *var, const char *type);

//WRITE <symb>
void cg_write(CodeGenerator *cg, const char *symb);

// String operations
void cg_strlen(CodeGenerator *cg, const char *dest, const char *symb);
void cg_getchar(CodeGenerator *cg, const char *dest, const char *symb1, const char *symb2);
void cg_setchar(CodeGenerator *cg, const char *var, const char *symb1, const char *symb2);

// Logical operations: AND, OR, NOT, LT, GT, EQ.
void cg_lt(CodeGenerator *cg, const char *var, const char *symb1, const char *symb2);
void cg_gt(CodeGenerator *cg, const char *var, const char *symb1, const char *symb2);
void cg_eq(CodeGenerator *cg, const char *var, const char *symb1, const char *symb2);
void cg_and(CodeGenerator *cg, const char *var, const char *symb1, const char *symb2);
void cg_or(CodeGenerator *cg, const char *var, const char *symb1, const char *symb2);
void cg_not(CodeGenerator *cg,const char *var, const char *symb);

// generates function definition
void generate_function(CodeGenerator *cg, NodePtr fun_node);

// generates function call code
void generate_function_call(CodeGenerator *cg, NodePtr fn_node);

//generate built in function call code
void generate_builtin_call(CodeGenerator *cg, NodePtr ifj_callNode, NodePtr storeNode, char *result);

//generate params
void cg_function_def_params(CodeGenerator *cg, NodePtr param_list);

// Generates a block of code (e.g., within a loop or function). 
void generate_block(CodeGenerator *cg, NodePtr block_node);

// generates return statement
void generate_return(CodeGenerator *cg, NodePtr return_node);

// generates assignment statement
void generate_assignment(CodeGenerator *cg, NodePtr assign_node);

// generates if-else statement
void generate_if_else(CodeGenerator *cg, NodePtr if_node);

// generates while loop
void generate_while(CodeGenerator *cg, NodePtr while_node);

// generates variable or constant declaration
void generate_declaration(CodeGenerator *cg, NodePtr dcl_node);

// Evaluates and generates code for expressions.
void generate_expression(CodeGenerator *cg, NodePtr expr_node, char *result);

// Generates a temporary variable for storing intermediate results.
char* generate_temp_var(CodeGenerator *cg, NodePtr node);

// generates the string literal 
char* cg_literal(NodePtr node);

// Writes a formatted instruction to the output stream.
void cg_write_instruction(CodeGenerator *cg, const char *format, ...);

// Rewrites a string for escaping and formatting.
char* rewrite_string(const char *input);

// Duplicates a string with memory allocation.
char* my_strdup(const char *str);

// Checks if a variable is already declared.
bool is_variable_declared(const char *name);

// Adds a variable to the symbol table.
void add_variable_to_symbol_table(const char *name);

// Reallocates memory safely.
void* safe_realloc(void *ptr, size_t new_size);

// Formats a string dynamically and returns it.
char *format_string(const char *format, ...);

// Resets the symbol table for managing declared variables.
void reset_symbol_table();

// Adds a variable and its type to the type table.
void add_variable_to_type_table(const char *name, DataType type);

// Retrieves the type of a variable.
DataType get_variable_type(const char *name);

// Prints declared variables for debugging.
void  printDeclaredVariables();

//searches and generates variables for while body
void collect_and_generate_defvars(CodeGenerator *cg, NodePtr blockNode);

#endif // CODEGENERATOR_H
