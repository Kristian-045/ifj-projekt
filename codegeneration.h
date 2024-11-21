#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L

#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H
#include <stdio.h>
#include <string.h>
#include "scanner.h"
#include "parser.h"
#include "builtinfun.h"

typedef struct Node* NodePtr;
//struct for code generator
typedef struct CodeGenerator {
    FILE *output;
} CodeGenerator;

// function declarations

// initializes the code generator 
CodeGenerator* cg_init();

// frees 
//void cg_free(CodeGenerator *cg);

// fenerates the header .IFJcode2024
void cg_generate_header(CodeGenerator *cg);

// DEFVAR <var>
//param@ FRAME - "LF" "GF" "TF"
void cg_defvar(CodeGenerator *cg, const char *frame, const char *var_name);

// MOVE <var> <symb>
void cg_move(CodeGenerator *cg, const char *dest_frame, const char *dest_var, const char *src_symb);

// CALL
void cg_call(CodeGenerator *cg, const char *label);

// RETURN
void cg_return(CodeGenerator *cg);

// frame managment
void cg_createframe(CodeGenerator *cg);
void cg_pushframe(CodeGenerator *cg);
void cg_popframe(CodeGenerator *cg);

// label and jump 
void cg_label(CodeGenerator *cg, const char *label);
void cg_jump(CodeGenerator *cg, const char *label);
void cg_jumpifeq(CodeGenerator *cg, const char *label, const char *symb1, const char *symb2);
void cg_jumpifneq(CodeGenerator *cg, const char *label, const char *symb1, const char *symb2);

// arithmetic operations <var> <symb> <symb>
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

// logical operations
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

// generates code block  // to do asi by to trebalo troksu doladit
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

// generates expression
void generate_expression(CodeGenerator *cg, NodePtr expr_node, char *result);

// helper function for generating temporary variable names
char* generate_temp_var(CodeGenerator *cg, NodePtr node);

// generates the string literal -> int@5 float@xxx string@tralala
char* cg_literal(NodePtr node);

//easy way to write any instruction
void cg_write_instruction(CodeGenerator *cg, const char *format, ...);

//function for rewriting string
char* rewrite_string(const char *input);
///
char* my_strdup(const char *str);

#endif // CODEGENERATOR_H
