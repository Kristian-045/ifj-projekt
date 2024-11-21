/*code generator 2024 IFJ*/
#define _GNU_SOURCE
#define _XOPEN_SOURCE 700
#define _POSIX_C_SOURCE 200809L
#include "codegeneration.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>


//int count = 0;
int label_counter = 0;

//typedef struct Node* NodePtr;

char declared_variables[100][50];
int declared_variable_count = 0;

////pomocne funkcie///
bool is_variable_declared(const char *name) {
    for (int i = 0; i < declared_variable_count; i++) {
        if (strcmp(declared_variables[i], name) == 0) {
            return true;
        }
    }
    return false;
}

void add_variable_to_symbol_table(const char *name) {
    if (!is_variable_declared(name)) {
        strcpy(declared_variables[declared_variable_count++], name);
    }
}
//////

// inicializacia
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

    /*cg->buffer = string_init(malloc(sizeof(tString)));
    if (cg->buffer == NULL) {
        fprintf(stderr, "memory allocation error\n");
        fclose(cg->output);
        free(cg);
        exit(99);
    }*/

    return cg;
}

/*
void cg_free(CodeGenerator *cg) {
    if (cg != NULL) {
        if (cg->output != NULL) {
            fclose(cg->output);
        }
        string_free(cg->buffer);
        free(cg);*/


void* safe_realloc(void *ptr, size_t new_size) {
    void *new_ptr = realloc(ptr, new_size);
    if (new_ptr == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(99);
    }
    return new_ptr;
}

// generovanie headeru
void cg_generate_header() {
        printf(".IFJcode24\n");
        printf("GF@val1\n");
        printf("GF@val2\n");
        printf("JUMP $main\n");
}

//asi treba volat na konci mainu
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

// defvar
void cg_defvar(CodeGenerator *cg, const char *frame, const char *var_name) {
    cg_write_instruction(cg, "DEFVAR %s@%s\n", frame, var_name);
}

// move
void cg_move(CodeGenerator *cg, const char *dest_frame, const char *dest_var, const char *src_symb) {
    cg_write_instruction(cg, "MOVE %s@%s %s\n", dest_frame, dest_var, src_symb);
}

// funkcia call
void cg_call(CodeGenerator *cg, const char *label) {
    cg_write_instruction(cg, "CALL $%s\n", label);
}

// navrat z funkcie
void cg_return(CodeGenerator *cg) {
    cg_write_instruction(cg, "RETURN\n");
}

// framy
void cg_createframe(CodeGenerator *cg) {
    cg_write_instruction(cg, "CREATEFRAME\n");
}

void cg_pushframe(CodeGenerator *cg) {
    cg_write_instruction(cg, "PUSHFRAME\n");
}

void cg_popframe(CodeGenerator *cg) {
    cg_write_instruction(cg, "POPFRAME\n");
}

// skoky
void cg_label(CodeGenerator *cg, const char *label) {
    cg_write_instruction(cg, "LABEL $$%s\n", label);
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

// aritmeticke operaice
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

// praca so zaspbnikom
void cg_pushs(CodeGenerator *cg, const char *symb) {
    cg_write_instruction(cg, "PUSHS %s\n", symb);
}

void cg_pops(CodeGenerator *cg, const char *var) {
    cg_write_instruction(cg, "POPS %s\n", var);
}

// read a vrite
void cg_read(CodeGenerator *cg, const char *var, const char *type) {
    cg_write_instruction(cg, "READ %s %s\n", var, type);
}

void cg_write(CodeGenerator *cg, const char *symb) {
    cg_write_instruction(cg, "WRITE %s\n", symb);
}

//praca s retazcami
// STRLEN 
void cg_strlen(CodeGenerator *cg, const char *dest, const char *symb) {
    cg_write_instruction(cg, "STRLEN %s %s\n", dest, symb);
}

// GETCHAR 
void cg_getchar(CodeGenerator *cg, const char *dest, const char *symb1, const char *symb2) {
    cg_write_instruction(cg, "GETCHAR %s %s %s\n", dest, symb1, symb2);
}

// SETCHAR 
void cg_setchar(CodeGenerator *cg, const char *var, const char *symb1, const char *symb2) {
    cg_write_instruction(cg, "SETCHAR %s %s %s\n", var, symb1, symb2);
}

/*logicke operatory*/
void cg_lt(CodeGenerator *cg,const char *var, const char *symb1, const char *symb2) {
    cg_write_instruction(cg, "LT %s %s %s\n", var, symb1, symb2);
}

void cg_gt(CodeGenerator *cg,const char *var, const char *symb1, const char *symb2) {
    cg_write_instruction(cg, "GT %s %s %s\n", var, symb1, symb2);
}

void cg_eq(CodeGenerator *cg,const char *var, const char *symb1, const char *symb2) {
    cg_write_instruction(cg, "EQ %s %s %s\n", var, symb1, symb2);
}


void cg_and(CodeGenerator *cg,const char *var, const char *symb1, const char *symb2) {
    cg_write_instruction(cg, "AND %s %s %s\n", var, symb1, symb2);
}

void cg_or(CodeGenerator *cg,const char *var, const char *symb1, const char *symb2) {
    cg_write_instruction(cg, "OR %s %s %s\n", var, symb1, symb2);
}

void cg_not(CodeGenerator *cg,const char *var, const char *symb) {
    cg_write_instruction(cg, "NOT %s %s %s\n", var, symb);
}

// pomocna funkcia
void cg_write_instruction(CodeGenerator *cg, const char *format, ...) {
    if (cg == NULL || cg->output == NULL) {
        return;
    }
    va_list args;
    va_start(args, format);
    vfprintf(cg->output, format, args);
    va_end(args);
}

//premena stringov
char* rewrite_string(const char *input) {
    size_t buffer_size = 128; 
    size_t output_len = 0;
    char *output = (char *)malloc(buffer_size);

    if (output == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(99);
    }

    // "string@"
    const char *prefix = "string@";
    size_t prefix_len = strlen(prefix);
    if (prefix_len >= buffer_size) {
        buffer_size = prefix_len + 1;
        output = safe_realloc(output, buffer_size);
    }
    strcpy(output, prefix);
    output_len += prefix_len;

    for (size_t i = 0; input[i]; i++) {
        char c = input[i];

        // ak mame speci znak
        if (c == 35 || c == 92 || c <= 32) {
            char xyz[6];
            snprintf(xyz, sizeof(xyz), "\\%03d", c);

            // kontorla velkosti bufru
            size_t add_len = strlen(xyz);
            if (output_len + add_len >= buffer_size) {
                buffer_size = (output_len + add_len) * 2;
                output = safe_realloc(output, buffer_size);
            }

            strcat(output, xyz);
            output_len += add_len;
        } else {
            // obycajne pismeno
            if (output_len + 1 >= buffer_size) {
                buffer_size = output_len +  2;
                output = safe_realloc(output, buffer_size);
            }

            output[output_len++] = c;
            output[output_len] = '\0'; 
        }
    }

    return output;
}

//pomocna funckia
char *format_string(const char *format, ...) {
    va_list args;
    va_start(args, format);

    // zistim velkost
    size_t size = vsnprintf(NULL, 0, format, args) + 1;
    va_end(args);

    char *result = (char *)malloc(size);
    if (result == NULL) {
        fprintf(stderr, "memory allocation error\n");
        exit(99);
    }

    // buffer s retazcom
    va_start(args, format);
    vsnprintf(result, size, format, args);
    va_end(args);

    return result;
}

//generovanie literalov (int@25, float@xxx string@tralala nil@nil)
char* cg_literal(NodePtr node) {
    if (node == NULL) {
        return NULL;
    }

    char *result = NULL;

    switch(node->keyword) {
        case T_INT : {
            result = format_string("int@%d", node->data.int_val);
            break;
        }
        case T_FLOAT: {
            result = format_string("float@%a", node->data.float_val);
            break;
        }

        case T_STRING: {
            char *new_string = rewrite_string(node->data.string_val);
            result = format_string("string@%s", new_string);
            free(new_string);
            break;
        }
        case T_NULL: {
            const char *nil_string = "nil@nil";
            result = malloc(strlen(nil_string) + 1);
            if (result == NULL) {
                fprintf(stderr, "memory allocation error\n");
                exit(99);
            }
            strcpy(result, nil_string);
            break;
        }
        default: {
            fprintf(stderr, "doslo k cbzge: %d\n", node->keyword);
            exit(53);
        }
    }

    return result;
}


/*GENERATOR FUNKCIII*/
//generuje kod pre volanie funkxie
void generate_function_call(CodeGenerator *cg, NodePtr fn_node) {
    if (cg == NULL || fn_node == NULL || fn_node->data_type != STRING || fn_node->data.string_val == NULL) {
        fprintf(stderr, "invalid function call node\n");
        return;
    }

    // kod pre argumenty
    NodePtr arg_node = fn_node->left; // je toto argument list??
    //int arg_count = 0;

    while (arg_node != NULL) {
        //if (arg_node->data_type == STRING) 
            char *arg_value = cg_literal(arg_node);  //TO DO toto zrejme nebude fungovat
            //cg_write_instruction(cg, "PUSHFRAME\n");
            cg_write_instruction(cg, "DEFVAR LF@arg%d\n", label_counter);
            cg_write_instruction(cg, "MOVE LF@arg%d %s\n", label_counter, arg_value);
            free(arg_value);
            //arg_count++;
        
        arg_node = arg_node->right; 
    }

    cg_call(cg, fn_node->data.string_val);
}

//generuje label a setup
void cg_function_begin(CodeGenerator *cg, const char *fun_name) {
    //label
    cg_label(cg, fun_name);
    cg_createframe(cg);
    cg_pushframe(cg);
    //cg_defvar(cg, "LF", "retval");
}

//definuje parametre
void cg_function_def_params(CodeGenerator *cg, NodePtr param_list) {
    if (cg == NULL || param_list == NULL) {
        return;
    }

    int param_index = 0;
    NodePtr param_node = param_list;

    // iterujeme cez vsetky parametre a definujeme ich v local frame
    while (param_node != NULL && param_node->keyword == FN_PARAM) {
        if (param_node->data_type == STRING && param_node->data.string_val != NULL) {
            //char param_name[20];
            //snprintf(param_name, sizeof(param_name), "arg%d", param_index);
            cg_defvar(cg, "LF", param_node->data.string_val);        
            cg_write_instruction(cg, "MOVE LF@%s TF@arg%d\n", param_node->data.string_val, param_index); 
            param_index++;
        }
        param_node = param_node->right;
    }
}


//konec
void cg_fun_end(CodeGenerator *cg) {
    if (cg == NULL) {
        return;
    }
    cg_popframe(cg);
    cg_return(cg);
}

//budeme volat pravy podstrom od aktualneho uzla 
void generate_function(CodeGenerator *cg, NodePtr fun_node) {
    if (cg == NULL || fun_node == NULL) {
        return;
    }

    //printf("name of fun %s\n", fun_node->data.string_val);
    //generujeme LABEL
    if(fun_node->data_type == STRING  && fun_node->data.string_val != NULL) {
        cg_function_begin(cg, fun_node->data.string_val);
    }

    //generovanie parametrov
    NodePtr fn_data_node = fun_node->left;
    if (fn_data_node != NULL && fn_data_node->keyword == FN_DATA) {
        NodePtr param_list = fn_data_node->left;  
        cg_function_def_params(cg, param_list);

        NodePtr ret_node = fn_data_node->right;
            if(ret_node->keyword != T_VOID && ret_node != NULL) {
                cg_write_instruction(cg, "DEFVAR LF@retval\n");
            }
    }

    //generovanie tela funckie
    NodePtr body_node = fun_node->right;
    if(body_node != NULL) {
        generate_block(cg, body_node);
    }

    //koniec funckie

    cg_fun_end(cg);
}

//generovanie xxx = expression
void generate_assignment(CodeGenerator *cg, NodePtr assign_node) {
    int count = 0 ;
    if (cg == NULL || assign_node == NULL) {
        return;
    }

    //lava strana -> premenna do ktorej sa priradza
    NodePtr var_node = assign_node->left;
    if (var_node == NULL || var_node->data_type != STRING || var_node->data.string_val == NULL) {
        fprintf(stderr, "Invalid target variable\n");
        return;
    }

    //priradzovana premenna
    char *frame = "LF";
    char *var_name = var_node->data.string_val;

    if (!is_variable_declared(var_name)) {
        cg_defvar(cg, frame, var_name);
        add_variable_to_symbol_table(var_name);
    }



    //kod pre vyhodnotenie expression
    NodePtr expr_node = assign_node->right;
    if (expr_node == NULL) {
        fprintf(stderr, "Invalid expression\n");
        return;
    }
    
    char temp_var[20];
    sprintf(temp_var, "TF@temp%d", count++);
    cg_write_instruction(cg, "DEFVAR %s\n", temp_var);
    generate_expression(cg, expr_node, temp_var);

    //move 
    cg_move(cg, frame, var_name, temp_var);
}

void generate_expression(CodeGenerator *cg, NodePtr expr_node, char *result) {
    if (cg == NULL || expr_node == NULL || result == NULL) {
        return;
    }

    switch(expr_node->keyword) {
        case T_INT: {
            cg_write_instruction(cg, "MOVE %s int@%d\n", result, expr_node->data.int_val);
            break;
        }
        case(T_FLOAT) : {
            cg_write_instruction(cg, "MOVE %s float@%a\n", result, expr_node->data.float_val);
            break;
        }
        case(T_STRING) : {
            char* new_string = rewrite_string(expr_node->data.string_val);
            cg_write_instruction(cg, "MOVE %s string@%s\n", result, new_string);
            free(new_string);
            break;
        }
        case(T_ID) :{
            if(expr_node->data_type == STRING) {
                cg_write_instruction(cg, "MOVE TF@temp1 LF@%s\n", expr_node->data.string_val);
            }
            break;
        }
        case T_PLUS:
        case T_MINUS:
        case T_ASTERISK:
        case T_SLASH: {
            //binarne operacie
            const char *op1 = generate_temp_var(cg, expr_node->left);
            const char *op2 = generate_temp_var(cg, expr_node->right);

            switch(expr_node->keyword) {
                case T_PLUS:
                    cg_write_instruction(cg, "ADD TF@%s %s %s\n", result, op1, op2);
                    break;
                case T_MINUS:
                    cg_write_instruction(cg, "SUB TF@%s %s %s\n", result, op1, op2);
                    break;
                case T_ASTERISK:
                    cg_write_instruction(cg, "MUL TF@%s %s %s\n", result, op1, op2);
                    break;
                case T_SLASH:
                    cg_write_instruction(cg, "DIV TF@%s %s %s\n", result, op1, op2);
                    break;
                case T_ID:
                    //printf("right know %s %s %s", op1, op2, expr_node);
                    break;
                default:
                    fprintf(stderr, "Invalid %d\n", expr_node->keyword);
                    exit(2);
            }
            break;
        }
        case T_GREATER:
        case T_GREATEREQUAL:
        case T_LESS:
        case T_LESSEQUAL:
        case T_EQUALS:
        case T_NOTEQUAL: {
            // < > <= => Not 
            NodePtr left = expr_node->left;
            NodePtr right = expr_node->right;
            char *op1 = generate_temp_var(cg, left);
            char *op2 = generate_temp_var(cg, right);

            switch (expr_node->keyword) {
                case T_GREATER:
                    cg_gt(cg, result, op1, op2);
                    break;
                case T_GREATEREQUAL:
                    cg_lt(cg, result, op1, op2);
                    cg_not(cg, result, result); //  to get >=
                    break;
                case T_LESS:
                    cg_lt(cg, result, op1, op2);
                    break;
                case T_LESSEQUAL:
                    cg_gt(cg,  result, op1, op2);
                    cg_not(cg, result, result); // NOT the result to get <=
                    break;
                case T_EQUALS:
                    cg_eq(cg, result, op1, op2);
                    break;
                case T_NOTEQUAL:
                    cg_eq(cg, result, op1, op2);
                    cg_not(cg,result, result); // NOT the result to get !=
                    break;
                default:
                    fprintf(stderr, "invalid operation\n");
                    exit(2);
            }
            break;
        }
        default: 
            fprintf(stderr, "invalid type in expression %d\n", expr_node->keyword);
            exit(2);
        }
    }

//generation of return 
void generate_return(CodeGenerator *cg, NodePtr return_node) {
    int count = 0;
    if (cg == NULL || return_node == NULL) {
        return;
    }

    // Evaluate return expression, if any
    if (return_node->right != NULL) {
        char result[20];
        sprintf(result, "TF@ret%d", count++);
        cg_defvar(cg, "TF", result);
        generate_expression(cg, return_node->right, result);
        cg_move(cg, "LF", "retval", result);
    }
    
    cg_return(cg);
}


//pomocna funkcia na generovamie temp values
/*
char* generate_temp_var(CodeGenerator *cg, NodePtr node) {
    //count++;
    static int temp_count = 0;
    if(cg == NULL || node == NULL) {
        return NULL;
    }

    static char temp_var[20];
    sprintf(temp_var, "temp%d", temp_count++);
    cg_defvar(cg, "TF", temp_var);
    //generate_expression(cg, node, temp_var);
    return temp_var;
}*/

char *generate_temp_var(CodeGenerator *cg, NodePtr expr_node) {
    static int count = 0;
    char *temp_var = malloc(20 * sizeof(char));
    if (!temp_var) {
        fprintf(stderr, "Memory allocation error\n");
        exit(99);
    }

    switch (expr_node->keyword) {
        case T_INT: {
            snprintf(temp_var, 20, "TF@temp%d", count++);
            cg_write_instruction(cg, "DEFVAR %s\n", temp_var);
            cg_write_instruction(cg, "MOVE %s int@%d\n", temp_var, expr_node->data.int_val);
            break;
        }
        case T_FLOAT: {
            snprintf(temp_var, 20, "TF@temp%d", count++);
            cg_write_instruction(cg, "DEFVAR %s\n", temp_var);
            cg_write_instruction(cg, "MOVE %s float@%a\n", temp_var, expr_node->data.float_val);
            break;
        }
        case T_STRING: {
            snprintf(temp_var, 20, "TF@temp%d", count++);
            cg_write_instruction(cg, "DEFVAR %s\n", temp_var);
            char *rewritten_string = rewrite_string(expr_node->data.string_val);
            cg_write_instruction(cg, "MOVE %s string@%s\n", temp_var, rewritten_string);
            free(rewritten_string);
            break;
        }
        case T_ID: {
            snprintf(temp_var, 20, "TF@temp%d", count++);
            cg_write_instruction(cg, "DEFVAR %s\n", temp_var);
            cg_write_instruction(cg, "MOVE %s LF@%s\n", temp_var, expr_node->data.string_val);
            break;
        }
        default: {
            sprintf(temp_var, "temp%d", count++);
            cg_defvar(cg, "TF", temp_var);
        }
    }
    return temp_var;
}


void generate_if_else(CodeGenerator *cg, NodePtr if_node) {
    if( cg == NULL || if_node == NULL || if_node->keyword != T_IF) {
        fprintf(stderr, "invalid T_IF node\n");
        exit(99);
    }

    //aby sme mali originalne labels pre else a end
    int current_label = label_counter++;
    char else_label[20];
    char end_label[20];
    snprintf(else_label, sizeof(else_label), "$ELSE_%d", current_label);
    snprintf(end_label, sizeof(end_label), "$ENDIF_%d", current_label);

    //generovanie podmienky
    NodePtr if_data = if_node->left;  //trz sme na if_data
    if (if_data == NULL || if_data->keyword != IF_DATA) {
        fprintf(stderr, "invalid if_data node\n");
        exit(99);
    }

    NodePtr condition = if_data->left;    //lavy podstrom EXP
    if (condition == NULL) {
        fprintf(stderr, "Condition is missing\n");
        exit(99);
    }

    char *condition_result = generate_temp_var(cg, condition);
    generate_expression(cg, condition, condition_result);

    //podmieneny skok na $$ELSE ak podmienka je nepravdiva
    cg_write_instruction(cg, "JUMPIFNEQ %s %s bool@true\n", else_label, condition_result);


    //telo IF
    NodePtr if_body = if_node->right;
    if (if_body != NULL) {
        generate_block(cg, if_body);
    }
    
    NodePtr else_node = if_data->right; 
    if (else_node != NULL) {
        //nepodmieney skok na end
        cg_write_instruction(cg, "JUMP %s\n", end_label);
    }


    //else podstrom
    cg_write_instruction(cg, "LABEL $%s\n", else_label);
 
    if(else_node != NULL) {
        generate_block(cg, else_node);
    }

    //konec if else
    cg_write_instruction(cg, "LABEL $%s\n", end_label);
}

void generate_while(CodeGenerator *cg, NodePtr while_node) {
    if(cg == NULL || while_node == NULL || while_node->keyword != T_WHILE) {
        fprintf(stderr, "Invalid while_node\n");
        exit(99);
    }
    static int label_counter = 0;
    int current_label = label_counter++;
    char start_label[20];
    char end_label[20];
    snprintf(start_label, sizeof(start_label), "$WHILE_START_%d", current_label);
    snprintf(end_label, sizeof(end_label), "$WHILE_END_%d", current_label);

    //zaciatok loopu 
    cg_write_instruction(cg, "LABEL $%s\n", start_label) ;

    //generovanie podmienky
    NodePtr while_data = while_node->left; // DATA_WHILE
    if (while_data == NULL || while_data->keyword != WHILE_DATA) {
        fprintf(stderr, "Invalid WHILE DATA node\n");
        exit(99);
    }

    NodePtr condition = while_data->left; //podmienka
    if (condition == NULL) {
        fprintf(stderr, "Condition expression is missing\n");
        exit(99);
    }

    //printf("condition :  %d", condition->keyword);

    char *condition_result = generate_temp_var(cg, condition);
    generate_expression(cg, condition, condition_result);

    //podmieneny skok na ukoncenie while
    cg_write_instruction(cg, "JUMPIFNEQ %s %s bool@true\n", end_label, condition_result);


    //generovanie while body
    NodePtr while_body = while_node->right;
    printf("while_body %d", while_body->keyword);
    if (while_body != NULL) {
        generate_block(cg, while_body);
    }

    //skok na zaciatok loopu
    cg_write_instruction(cg, "JUMP %s\n", start_label);

    //label koenic smycky
    cg_write_instruction(cg, "LABEL $%s\n", end_label);

}

//generujeme deklaraciu
void generate_declaration(CodeGenerator *cg, NodePtr dcl_node) {
    if (cg == NULL || dcl_node == NULL) {
        fprintf(stderr, "invalid declaration node\n");
        exit(99);
    }

    if (dcl_node->keyword != T_VAR && dcl_node->keyword != T_CONST) {
        fprintf(stderr, "unexpected value\n");
        exit(99);
    }

    if (dcl_node->data_type != STRING) {
        fprintf(stderr, "invalid id\n");
        exit(99);
    }


    char *var_name = dcl_node->data.string_val;

    //novu premennu deklarujeme iba ak uz nie je deklarovana
    if (!is_variable_declared(var_name)) {
        // defvar instrukcia
        cg_defvar(cg, "LF", var_name);
        add_variable_to_symbol_table(var_name);
    }

    //cg_defvar(cg, var_frame, var_name);

    //ak existuje priradenie hodnoty tak ju vyskusame vygenerovat heh
    NodePtr equalSign = dcl_node->left;
    if (equalSign != NULL && equalSign->keyword == T_EQUALSIGN) {
        NodePtr value_node = equalSign->right;
        if (value_node != NULL) {
            char *value = cg_literal(value_node);
            cg_write_instruction(cg, "MOVE %s@%s %s\n", "LF", var_name, value);
            free(value);
        }
    }
}

//generaot tiel, asi nie je uplne done
void generate_block(CodeGenerator *cg, NodePtr block_node) {
     if (cg == NULL || block_node == NULL) {
        return;
    }

    NodePtr current = block_node;
    while (current != NULL) {
        switch (current->keyword) {
            case T_CONST:
            case T_VAR:
                // Deklaracia premennej ci?
                generate_declaration(cg, current);
                break;

            case T_ID:
                if (current->left != NULL && current->left->keyword == T_EQUALSIGN) {
                    // je to assignment??
                    generate_assignment(cg, current);
                } else if (current->right != NULL && current->right->keyword == T_LBRACKET) {
                    // inak to je asi function call??
                    generate_function_call(cg, current);
                } else {
                    fprintf(stderr, "Invalid use of identifier  %d \n", current->keyword );
                    //exit(99); 
                }
                break;

            case T_RETURN:
                generate_return(cg, current);
                break;

            case T_IF:
                // generuje,me if
                generate_if_else(cg, current->right);
                break;

            case T_ELSE:
                if (current->right != NULL) {
                // dalsi else
                static int label_counter = 0;
                int current_label = label_counter++;
                char else_label[20];
                snprintf(else_label, sizeof(else_label), "ELSE_%d", current_label);

                // zaciatok
                cg_label(cg, else_label);
                generate_block(cg, current);
                }

                break;

            case T_WHILE:
                generate_while(cg, current);
                break;

           case T_FN:
                generate_function(cg, current);
                break;
            case FN_CALL:
                generate_function_call(cg, current);
                break;
            case FN_DATA:
            case T_I32:
            case START:
            case T_VOID:
            case WHILE_DATA:
            case T_GREATER:
                    if(current->left != NULL) {
                generate_block(cg, current->left);} 
                else {
                    generate_block(cg, current->right);
                }
                break;
            case NEW_COMMAND:
                generate_block(cg, current->right);
                break;
    
            case T_LESS:  {
                const char *result = generate_temp_var(cg, current);
                char *op1 = generate_temp_var(cg, current->left);
                char *op2 = generate_temp_var(cg, current->right);
                cg_write_instruction(cg,"LT %s %s %s", result, op1, op2);
                break;}
            case T_LESSEQUAL:{
                
                char *result = generate_temp_var(cg, current);
                char *op1 = generate_temp_var(cg, current->left);
                char *op2 = generate_temp_var(cg, current->right);
                cg_gt(cg,  result, op1, op2);
                cg_not(cg, result, result);
                break;}
            case T_EQUALSIGN:
                generate_assignment(cg, current);
                break;
            default:
                fprintf(stderr, "Invalid command in block: %d\n", current->keyword);
                exit(99);
        }

        current = current->left;
    }
}


//nefunguje my strdupp
char* my_strdup(const char *str) {
    if (str == NULL) return NULL;
    size_t len = strlen(str) + 1;
    char *copy = malloc(len);
    if (copy != NULL) {
        memcpy(copy, str, len);
    }
    return copy;
}
