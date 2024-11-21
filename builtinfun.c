/*builtin functions*/
/*ifj 2024*/

#include "builtinfun.h"
#include <stdio.h>


/*pub fn ifj.readstr() ?[]u8*/
void read_string() {
    printf("JUMP $end_reads\n");
    printf("LABEL $$reads\n");
    printf("PUSHFRAME\n");
    //printf("CREATEFRAME\n");

    printf("POPS GF@bin\n");
    printf("DEFVAR TF@output\n");
    printf("READ TF@output string\n");
    printf("PUSHS TF@out\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $$end_reads\n");
    printf("\n");
}

/*pub fn ifj.readi32() ?i32*/
void read_int() {
   printf("JUMP $end_readi\n");
    printf("LABEL $$readi\n");
    printf("PUSHFRAME\n");
    //printf("CREATEFRAME\n");

    printf("POPS GF@bin\n");
    printf("DEFVAR TF@output\n");
    printf("READ TF@output int\n");
    printf("PUSHS TF@out\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $$end_readi\n");
    printf("\n");
}

/*pub fn ifj.readf64() ?f64*/
void read_float() {
   printf("JUMP $end_readf\n");
    printf("LABEL $$readf\n");
    printf("PUSHFRAME\n");
    //printf("CREATEFRAME\n");

    printf("POPS GF@bin\n");
    printf("DEFVAR TF@output\n");
    printf("READ TF@output float\n");
    printf("PUSHS TF@out\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $$end_readf\n");
    printf("\n");
}

/*pub fn ifj.write(term) void*/
void write_term() {
    printf("JUMP $write_end");
    printf("LABEL $$write_term");
    printf("PUSHFRAME\n");
    printf("CREATEFRAME\n");
    printf("PUSHS LF@retval\n");
    printf("TYPE LF@type_of_term LF@retval1\n");
    printf("POPS LF@type_of_term\n");

    printf("JUMPIFEQ $write_int LF@type_of_term string@int\n");
    printf("JUMPIFEQ $write_float LFF@type_of_term string@float\n");
    printf("JUMPIFEQ $write_null LF@type_of_term string@nil\n");


    printf("LABEL $$write_int\n");
    printf("WRITE int@\n\n");
    printf("JUMP $write_end\n");

    printf("LABEL $$write_float\n");
    printf("WRITE float@\n\n");
    printf("JUMP $write_end\n");

    printf("LABEL $$write_null\n");
    printf("WRITE nil@nil\n");

    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $$write_end\n");
    printf("\n");
}

/*pub fn ifj.i2f(term ∶ i32) f64*/
void convert_int2_float() {
    printf("JUMP $int2float_end\n");
    printf("LABEL $$int2float\n");
    printf("POPS GF@trash\n");
    printf("INT2FLOATS\n");
    printf("RETURN\n");

    printf("LABEL $$int2float_en\n");
}

/*pub fn ifj.f2i(term ∶ f64) i32 */
void convert_float2_int() {
    printf("JUMP $float2int_end\n");
    printf("LABEL $$float2int\n");
    printf("POPS GF@trash\n");
    printf("FLOAT2INTS\n");
    printf("RETURN\n");

    printf("LABEL $$float2int_en\n");
}

/*pub fn ifj.length(𝑠 : []u8) i32*/
void fun_length() {
    printf("JUMP $lenght_end\n");
    printf("LABEL $$lenght\n");
    printf("PUSHFRAME\n");
    printf("CREATEFRAME\n");
    printf("DEFVAR LF@result\n");               
    printf("STRLEN LF@result LF@slice\n");   
    printf("MOVE GF@retval LF@result\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $$lenght_end\n");
    printf("\n");
}

/*pub fn ifj.concat(𝑠1 : []u8, 𝑠2 : []u8) []u8*/
void fun_concat() {
    printf("JUMP $concat_end\n");
    printf("LABEL $$concat\n");
    printf("PUSHFRAME\n");
    printf("CREATEFRAME\n");
    printf("DEFVAR LF@result\n");
    printf("CONCAT LF@result LF@symb1 LF@symb2\n");
    printf("MOVE GF@retval LF@result\n");
     printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $$concat_end\n");
    printf("\n");
}

/*pub fn ifj.substring(𝑠 : []u8, 𝑖 : i32, 𝑗 : i32) ?[]u8*/
void fun_substring() {
    printf("JUMP $substring_end\n");
    printf("LABEL $$substring\n");
    printf("PUSHFRAME\n");
    printf("CREATEFRAME\n");
    printf("DEFVAR LF@s\n");
    printf("DEFVAR LF@i\n");
    printf("DEFVAR LF@j\n");
    printf("DEFVAR TF@boolval\n");
    printf("MOVE LF@s LF@1stpar\n");
    printf("MOVE LF@i LF@2ndpar\n");
    printf("MOVE LF@j LF@3rdpar\n");
    printf("DEFVAR LF@retval\n");
    //*kontrola vstupnych params i < 0 ;j < 0; i > j *//
    printf("LT TF@boolval LF@i int@0\n");
    printf("JUMPIFEQ $fun_error TF@boolval bool@true\n");
    printf("LT TF@boolval LF@j int@0\n");
    printf("JUMPIFEQ $fun_error TF@boolval bool@true\n");
    printf("GT TF@boolval LF@i LF@j\n");
    printf("JUMPIFEQ $fun_error TF@boolval bool@true\n");

    //vypocitame dlzku retazca
    printf("CREATEFRAME\n");
    printf("DEFVAR TF@_val\n");
    printf("MOVE TF@_val LF@s\n");
    printf("CALL $fun_lenght\n");
    printf("DEFVAR TF@len\n");
    printf("MOVE TF@len GF@result\n");

    //kontrola i >= ifj.length(s);j > ifj.length(s)
    printf("DEFVAR TF@boolval\n"); 
    printf("GT TF@boolval LF@i TF@len\n");
    printf("JUMPIFEQ $fun_error TF@boolval bool@true\n");
    printf("JUMPIFEQ $fun_error LF@i TF@len\n");
    printf("GT TF@boolval LF@j TF@len\n");
    printf("JUMPIFEQ $fun_error TF@boolval bool@true\n");

    //loop
    printf("DEFVAR TF@ind\n");
    printf("MOVE TF@ind LF@i\n");
    printf("MOVE LF@retval string@\n");
    printf("LABEL $fun_loop\n");
    printf("JUMPIFEQ $fun_end TF@ind LF@j\n");
    printf("GETCHAR TF@boolval LF@s TF@ind\n");
    printf("CONCAT LF@retval LF@retval TF@boolval\n");
    printf("ADD TF@ind TF@ind int@1\n");
    printf("JUMP $fun_loop\n");
    //the end
    printf("JUMP $fun_end\n");

    //pri chybe vratime nil@nil
    printf("LABEL $$fun_error\n");
    printf("MOVE LF@retval nil@nil\n");
    printf("LABEL $$fun_end\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $$concat_end\n");
    printf("\n");
}

/*pub fn ifj.strcmp(𝑠1 : []u8, 𝑠2 : []u8) i32*/
void fun_strcmp() {
    /*
     CREATEFRAME
    DEFVAR TF@s1
    DEFVAR TF@s2
    MOVE TF@s1 string@abc
    MOVE TF@s2 string@abd
    CALL $$strcmp
    */
    printf("JUMP $strcmp_end\n");
    printf("LABEL $$strcmp\n");

    printf("PUSHFRAME\n");
    printf("CREATEFRAME\n");

    printf("DEFVAR LF@result\n");
    printf("DEFVAR LF@i\n");
    printf("DEFVAR LF@len1\n");
    printf("DEFVAR LF@len2\n");
    printf("DEFVAR LF@char1\n");
    printf("DEFVAR LF@char2\n");

    printf("MOVE LF@result int@0\n");
    printf("MOVE LF@i int@0\n");

    printf("STRLEN LF@len1 LF@s1\n");
    printf("STRLEN LF@len2 LF@s2\n");

    printf("LABEL $$strcmp_loop\n");
    printf("LT LF@result LF@i LF@len1\n");
    printf("JUMPIFEQ $strcmp_end LF@result bool@false\n");
    printf("LT LF@result LF@i LF@len2\n");
    printf("JUMPIFEQ $strcmp_end LF@result bool@false\n");

    printf("GETCHAR LF@char1 LF@s1 LF@i\n");
    printf("GETCHAR LF@char2 LF@s2 LF@i\n");

    printf("JUMPIFEQ $strcmp_next LF@char1 LF@char2\n");
    printf("LT LF@result LF@char1 LF@char2\n");
    printf("JUMPIFEQ $strcmp_smaller LF@result bool@true\n");
    printf("JUMP $strcmp_greater\n");

    printf("LABEL $$strcmp_next\n");
    printf("ADD LF@i LF@i int@1\n");
    printf("JUMP $strcmp_loop\n");

    printf("LABEL $$strcmp_end\n");
    printf("LT LF@result LF@len1 LF@len2\n");
    printf("JUMPIFEQ $strcmp_smaller LF@result bool@true\n");
    printf("GT LF@result LF@len1 LF@len2\n");
    printf("JUMPIFEQ $strcmp_greater LF@result bool@true\n");

    printf("MOVE LF@result int@0\n");
    printf("JUMP $strcmp_return\n");

    printf("LABEL $$strcmp_smaller\n");
    printf("MOVE LF@result int@-1\n");
    printf("JUMP $strcmp_return");

    printf("LABEL $$strcmp_greater\n");
    printf("MOVE LF@result int@1\n");
    printf("JUMP $strcmp_return");

    printf("LABEL $$strcmp_return\n");
    printf("PUSHS LF@result\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $$strcmp_end\n");
    printf("\n");
}

/*pub fn ifj.ord(𝑠 : []u8, 𝑖 : i32) i32*/
void fun_ord_value() {
     printf("JUMP $ord_end\n");
    printf("LABEL $$ord\n");

    printf("PUSHFRAME\n");
    printf("CREATEFRAME\n");

    printf("DEFVAR LF@output\n");
    printf("MOVE LF@output nil@nil\n");
    printf("DEFVAR LF@cont\n");
    printf("LF LF@cont int@0 LF@i\n");
    printf("JUMPIFEQ $ord_er LF@cont bool@false\n");
    printf("DEFVAR LF@strlen\n");
    printf("STRLEN LF@strlen LF@str\n");
    printf("ADD LF@strlen LF@strlen int@1\n");
    printf("LT LF@cont LF@i LF@strlen\n");
    printf("JUMPIFEQ $ord_er LF@check bool@false\n");
    printf("SUB LF@i LF@i int@1\n");
    printf("STR2INT LF@output LF@str LF@in\n");
    printf("LABEL $$ord_er\n");
    printf("PUSHS LF@output\n");

    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $$ord_end\n");
    printf("\n");


}

/*pub fn ifj.chr(𝑖 : i32) []u8*/
void fun_char() {
    printf("JUMP $char_end\n");
    printf("LABEL $$ord\n");

    printf("PUSHFRAME\n");
    printf("CREATEFRAME\n");


    printf("DEFVAR LF@val1\n");
    printf("DEFVAR LF@val2\n");

    printf("LT LF@val1 LF@i int@0\n");
    printf("GT LF@val2 LF@i inT@25\n5");
    printf("OR LF@val1 LF@val2\n");
    printf("JUMIFEQ $chr_a LF@val1 bool@false\n");
    printf("PUSHS nil@nil\n");
    printf("JUMP $chr_return\n");
    printf("LABEL $$chr_a\n");
    printf("PUSHS LF@i\n");
    printf("INT2CHARS\n");
    printf("LABEL $$chr_return\n");

    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $$char_end\n");
    printf("\n");;
}

