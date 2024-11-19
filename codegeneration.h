/*code generator IFJ 2024*/
/*november 2024*/

#include "scanner.h"
#include "string.h"
#include "parser.h"
#include "builtinfun.h"
//#include "symtable.h"
//#include "stack.h"

#ifndef CODEGEN_H
#define CODEGEN_H

typedef enum {
    /*kazdy kod zacina s ".IFJcode24"*/
    //code_start,
    ////////*praca s frame*///////////////
    /*MOVE <var> <symb> */
    ins_move,

    /*vytvori novy docasny frame a zahodi obsah povodneho docasneho frame*/
    /*ziadne operandy*/
    ins_create_frame,

    /*presun docasneho ramca na zasobnik ramcov, ramec bude k dispozicii 
    cez LF a prekryje povodne ramce na zasobniku*/
    /*ziadne operandy*/
    ins_push_frame,

    /*presun aktualneho ramca do docasneho*/
    /*ziadne oparandy*/
    ins_pop_frame,

    /*DEFVAR <var>*/
    ins_def_var,

    /*CALL <label>*/
    ins_call,

    /*ziadne operandy*/
    ins_return,

    ////////////*praca s datovym zasobnikom*/////////////
    /*PUSHS <symb>*/
    ins_pushs,

    /*POPS <var>*/
    ins_pops,

    ins_clears,

    /////*aritmeticke, relacne, booleovske a konveryne ins*//////

    /*ADD <var> <op1> <op2>*/           //<var> je vzdy LF@retval
    ins_add,

    /*SUB <var> <op1> <op2>*/
    ins_sub,

    /*MUL <var> <op1> <op2>*/
    ins_mul,

    /*DIV <var> <op1> <op2>*/
    ins_div,

    /*IDIV <var> <op1> <op2>*/
    ins_idiv,

    //*ekvivalent pre zaspbnikovu verziu*/////
    ins_adds,

    ins_subs,

    ins_muls,

    ins_divs,

    ins_idivs,

    /*relacne*/
    /*LT/GT/EQ <var> <symb1> <symb2>*/
    ins_lt,
    
    ins_gt,

    ins_eq,
///////////////
    ins_lts,
    
    ins_gts,

    ins_eqs,
//////////////////
    /*AND/OR/NOT <var> <symb1> <symb2>*/
    ins_and,

    ins_or,

    ins_not,

    ins_ands,

    ins_ors,

    ins_nots,


    /*INT2FLOAT <var> <symb>*/
    ins_int_2_float,

    /*FLOAT2INT <var> <symb>*/
    ins_float_2_int,

    /*INT2CHAR <var> <symb>*/
    ins_int_2_char,

    /*STR2INT <var> <symb1> <symb2>*/
    ins_str_2_int,



    ins_int_2_floats,

    ins_float_2_ints,

    ins_int_2_chars,

    ins_str_2_ints,


    /*I/O instrukcie*/
    /*READ <var> <type>*/
    ins_read,

    /*WRITE <symb>*/
    ins_write,

    /*praca s retazcami*/

    /*CONCAT <var> <symb1> <symb2>*/
    ins_concat,

    /*GETCHAR <var> <symb1> <symb2>*/
    ins_get_char,

    /*SETCHAR <var> <symb1> <symb2>*/
    ins_set_char,

    /* STRLEN <var> <symb>*/
    ins_strlen,

    //////////////*praca s typmi*/////
    /*TYPE <var> <symb> - zisti typ daneho symbolu*/
    ins_type,

    /*riadenie toku programu*/

    /*LABEL <label> definicia navesti*/
    ins_label,

    /*JUMP <label>*/
    ins_jump,

    /*JUMPIFEQ <var> <symb1> <symb2>*/
    ins_jump_if_eq,

    /*JUMPIFNEQ <var> <symb1> <symb2>*/
    ins_jump_if_neq,

    /*zasobnikova verzia*/
    ins_jump_if_eqs,
    ins_jump_if_neqs,

    /*EXIT <symb> - ukoncenie interpretacie s navratovym kodom*/
    ins_exit,

    ////*debugging instruckie*///
    /*BREAK - vypis stavu interpretu na stderr*/
    ins_break,

    /*DPRINT <symb>*/
    ins_dprint,

    /*default hodnota*/
    ins_invalid,
} Instruction;
/*
typedef union {
    char* name;
    char *label; 
    tType type;
} Operand;
*/

typedef enum {
    fr_global,
    fr_local,
    fr_temp,
} Frame;

typedef struct {
    Frame frame;
    char* name;
}  Nonterminal;

/*instrukcie ig*/
void print_header();
void print_main();
void gen_var_declar(char *var);
void gen_move_int(char *var, int i);
void gen_temp_vars(int i);
void gen_code(Instruction ins, char* op1, char* op2, char* var, char* label, NodePtr node);
void gen_call_fun(char* fun_name);
void gen_fun_def(char *name);
void gen_fun_param(char *name, int argpos);
void gen_fun_body();
void gen_fun_end(char *name);
void rewrite_string(tString *output, char *input);
void print_footer();
void debug();

#endif
