/*code generator IFJ 2024*/
/*november 2024*/

#include "codegeneration.h"


tString main_buffer;
tString function_buffer;
tString expression_buffer;

tString* active_buffer;

char* last_function;

int count = 0;

const char* frame_n;
/*
#define set_frame(fr) \
    do { \
    switch(fr) { \
        case fr_global:  \
            frame_n = "GF";  \
            break;  \
        case fr_local:  \
            frame_n = "LF"; \
            break;  \
        case fr_temp:  \
            frame_n = "TF";  \
            break;  \
    }  \
     } while(0);
*/

void print_header() {
    printf(".IFJcode24\n");
    printf("DEFVAR GF@res\n");
    printf("DEFVAR GF@tmp1\n");
    printf("DEFVAR GF@tmp2\n");
    printf("DEFVAR GF@tmp3\n");
    printf("DEFVAR GF@tmp4\n");
    printf("DEFVAR GF@bin\n");
    printf("JUMP $main");
    string_init(&main_buffer);
    string_init(&function_buffer);
    string_init(&expression_buffer);
    active_buffer = &main_buffer;
}

//prints beginning of generated code
void print_main() {
    printf("LABEL $$main\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
}

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

/*void debug() {
    char* s = "bambambam";
    string_push(&main_buffer, s);
    string_concat(&main_buffer, "tralala");
    printf("prve  %s\n", main_buffer.data);
    char* xyz = "ok";
    string_concat(&main_buffer, xyz);
    printf("druhe  %s\n", main_buffer.data);
    printf("%s", xyz);
}
*/
void print_footer() {
    
    //printf("%s\n", main_buffer.data);
     if (main_buffer.data != NULL && main_buffer.lenght > 0) {
        printf("%s\n", main_buffer.data);
    } else {
        printf("No data in main_buffer.\n");
    }
    printf("POPFRAME\n");
    string_free(&main_buffer);
    string_free(&function_buffer);
    string_free(&expression_buffer);

}

void pop_value(char* var) {
    string_push(active_buffer, "POPS GF@res\n");
    string_push(active_buffer, "MOVE TF@");
    string_push(active_buffer, var);
    string_push(active_buffer, " GF@res\n");
}


/* 
void gen_var_declar(char *var, Frame frame) {
    set_frame(frame);
    string_push(active_buffer, "DEFVAR ");
    string_push(active_buffer, frame_n);
    string_push(active_buffer, "@");
    string_push(active_buffer, var);
    string_push(active_buffer, "\n");
}*/

void gen_var_declar(char *var) {
    string_push(active_buffer, "DEFVAR ");
    string_push(active_buffer, "LF@");
    string_push(active_buffer, var);
    string_push(active_buffer, "\n");
}

void gen_move_int(char *var, int i) {
    string_push(active_buffer, "MOVE LF@");
    string_push(active_buffer, var);
    string_push(active_buffer, " int@");
    string_push_int(active_buffer, i);
    string_push(active_buffer, "\n");
}



void gen_return_val() {
    string_push(active_buffer, "DEFVAR LF@retval\n");
}


/*zmeni string na chceny tvar v ifjcode24*/
void rewrite_string(tString *output, char *input) {
    string_concat(output, "string@");
    for(int i = 0; input[i]; i++) {
        char c = input[i];
        if (c == 35 || c == 92 || c <= 32) {
            char xyz[6] = "\\";
            snprintf(xyz + 1, 5, "%03d", c);
            string_concat(output, xyz);
        } else {
            string_append(output, c);
        }
    }
    
}



void push_value(tString *s, NodePtr data) {
    switch(data->keyword) {
        case(T_INT): {
            char str[25] = "int@";
            snprintf(str + 4, 21, "%d", data->data.int_val );
            string_concat(s, str);
            string_push(active_buffer, s->data);
            string_push(active_buffer, "\n");
            break;
        }
        case(T_FLOAT) :{
            int len = snprintf(NULL, 0, "%a", data->data.float_val);
            int len_of_s = (s->lenght + strlen("float@") + len + 1);
            if (len_of_s > s->max_lenght ) {
                char *tmp = (char*)realloc(s->data, sizeof(char) * len_of_s);
                if (!tmp) {
                    //error to do
                    return;
                }
                s->data = tmp;
                s->max_lenght = len_of_s;
            }

            string_concat(s, "float@");
            snprintf(s->data + s->lenght, len + 1, "%a", data->data.float_val);
            string_push(active_buffer, s->data);
            string_push(active_buffer, "\n");
            s->lenght += len;
            break;
        }
        case(T_STRING): {
            int len = strlen(data->data.string_val);
            int len_of_s = (len + strlen("@string") +1 );
            if ( len_of_s > s->max_lenght) {
                char *tmp = (char*)realloc(s->data, sizeof(char) * len_of_s);
                if (!tmp) {
                    //error to do
                    return;
                }
                s->data = tmp;
                s->max_lenght = len_of_s ;
            }
            rewrite_string(s, data->data.string_val);
            string_push(active_buffer, s->data);
            string_push(active_buffer, "\n");
            break;
        }
        case(T_NULL): {
            string_push(active_buffer, "nil@nil\n");
            break;
        }
        case(T_ID): {
            string_push(active_buffer, "TF@");
            string_push(active_buffer, data->data.string_val);

        }
        /*case() TO DO!! co robit ked VAR??? a const????*/
        default:
            exit(EXIT_FAILURE); //to do
        } 
    }


/*FUNCTION GENERATOR */
void gen_call_fun(char* fun_name) {
    string_push(active_buffer, "CALL $");
    string_push(active_buffer, fun_name);
    string_push(active_buffer, "\n");
}

void gen_label_fun(char* fun_name) {
    string_push(active_buffer, "LABEL $$");
    string_push(active_buffer, fun_name);
    string_push(active_buffer, "\n");
}

void gen_fun_def(char *name) {
    string_push(active_buffer, "LABEL $$fun_");
    string_push(active_buffer, name);
    string_push(active_buffer, "\nPUSHFRAME\n");
}

void gen_fun_body(){
    active_buffer = &function_buffer;
}

//before call
void gen_call_fun_param(NodePtr node) {
    string_push(active_buffer, "PUSHS ");
    push_value(active_buffer, node);
    string_push(active_buffer, "\n");
}

//after function
void gen_fun_after_param( char* val) {
    string_push(active_buffer, "DEFVAR LF@");
    string_push(active_buffer, val);
    string_push(active_buffer, "\n");
    string_push(active_buffer, "POPS LF@");
    string_push(active_buffer, val);
    string_push(active_buffer, "\n");

}
void gen_fun_return(char *name, int argpos) {
    string_push(active_buffer, "DEFVAR LF@retval");
    string_push(active_buffer, name);
    string_push(active_buffer, "\nMOVE LF@retval");
    string_push(active_buffer, name);
    string_push(active_buffer, "  LF@arg");
    string_push_int(active_buffer, argpos);
    string_push(active_buffer, "\n");
}

void gen_fun_end(char *name) {

    string_push(active_buffer, "LABEL $$endfun_");
    string_push(active_buffer, "POPFRAME\n");
    string_push(active_buffer, "RETURN\n");
    string_push(active_buffer, name);
    string_push(active_buffer, "\n");
    
    string_push(&main_buffer, function_buffer.data);
    string_clear(&function_buffer);
    active_buffer = &main_buffer;
}

/*IF GENERATOR*/
int depth = -1;
int max = -1;
char cont[50];
void gen_if_begin() {
    depth++;
    max++;
    cont[depth] = max;
    string_push(active_buffer, "# if_");
    string_push_int(active_buffer, max);
    string_push(active_buffer, "\n");
    gen_temp_vars(1);
    
    string_push(active_buffer, "POPS LF@tmp_val1\n");
    string_push(active_buffer, "JUMPIFNEQ $else_");
    string_push_int(active_buffer, max);
    string_push(active_buffer, "LF@tmp_val1 bool@false\n");
}

void gen_if_else() {
    int i = cont[depth];
    string_push(active_buffer, "JUMP $end_");
    string_push_int(active_buffer, i);
    string_push(active_buffer, "\n");
    string_push(active_buffer, "LABEL $$else_");
    string_push_int(active_buffer, i);
    string_push(active_buffer, "\n");
}

void gen_if_end() {
    int i = cont[depth];
    string_push(active_buffer, "LABEL $$end_");
    string_push_int(active_buffer, i);
    string_push(active_buffer, "\n");
    depth--;
}

/*WHILE GENERATOR*/
void gen_while_start() {
    depth++; 
    max++;
    cont[depth] = max;
    gen_temp_vars(1);
    string_push(active_buffer, "LABEL $$while_");
    string_push_int(active_buffer, max);
    string_push(active_buffer, "\n");
}

void gen_while_middle() {
    int i = cont[depth];
    string_push(active_buffer,"POPS LF@tmp_val1\n");
    string_push(active_buffer,"JUMPIFEQ $end_while_");
    string_push_int(active_buffer,i);
    string_push(active_buffer,"LF@tmp_val1 bool@false\n");
}

void gen_while_end() {
    int i = cont[depth];
    string_push(active_buffer, "JUMP $while_");
    string_push_int(active_buffer, i);
    string_push(active_buffer, "\n");
    string_push(active_buffer, "LABEL $$end_while_");
    string_push_int(active_buffer, i);
    string_push(active_buffer, "\n");
    depth--;
}

void gen_temp_vars(int i) {
    for(int j = 0; j < i ; j++ ) {
        string_push(active_buffer, "DEFVAR LF@tmp_val");
        string_push_int(active_buffer, j + 1);
        string_push(active_buffer, "\n");
        count++;
    }
}


void push_var_s_s_ins(char* operation, char* op1, char* op2) {
    gen_return_val();
    string_push(active_buffer, operation);
    string_push(active_buffer, "LF@retval ");
    //string_push(active_buffer, var);
    string_push(active_buffer, " LF@");
    string_push(active_buffer, op1);
    string_push(active_buffer, " LF@");
    string_push(active_buffer, op2);
    string_push(active_buffer, "\n");
}
void gen_code_arithmetic(Instruction ins, char* op1, char* op2, char* var){
    switch(ins) {
    case(ins_add): {
            push_var_s_s_ins("ADD ", op1, op2);
            break;
        }
        case(ins_sub) : {
           push_var_s_s_ins("SUB ", op1, op2);
            break;
        }
        case(ins_mul) : {
          push_var_s_s_ins("MUL ", op1, op2);
            break;
        }
        case(ins_div): {
            push_var_s_s_ins("DIV ", op1, op2);
            break;
        }
        case(ins_idiv) : {  // TO DO nie je to dokoncene
            push_var_s_s_ins("IDIV ", op1, op2);
            break;
        }
}
}

void gen_code (Instruction ins, char* op1, char* op2, char* var, char* label, NodePtr node) {
    switch(ins) {
        case(ins_move) : {
            string_push(active_buffer,"MOVE LF@");
            string_push(active_buffer, var);
            string_push(active_buffer, " LF@");
            string_push(active_buffer, op1);
            string_push(active_buffer, "\n");
            break;
        }
        case(ins_create_frame): {
            string_push(active_buffer, "CREATEFRAME\n");
            break;
        }
        case(ins_push_frame): {
            string_push(active_buffer, "PUSHFRAME\n");
            break;
        }
        case(ins_pop_frame): {
            string_push(active_buffer, "POPFRAME\n");
            break;
        }
        case(ins_def_var): {
            gen_var_declar(var);
            break;
        }
        case(ins_call): {
            gen_call_fun(label);
            break;
        }
        case(ins_return) : {
            string_push(active_buffer, "RETURN\n");
            break;
        }
        case(ins_pushs): {  //toto este treba doriesit, push_value ig
            string_push(active_buffer, "PUSHS ");
            string_push(active_buffer, op1);
            string_push(active_buffer, "\n");
            break;
        }
        case(ins_pops ) : {
            string_push(active_buffer, "POPS ");
            string_push(active_buffer, var);
            string_push(active_buffer, "\n");
            break;
        }
        case(ins_clears) : {
            string_push(active_buffer, "CLEARES\n");
            break;
        }
        case(ins_adds) : {
            string_push(active_buffer, "ADDS\n");
            break;
        }
         case(ins_subs) : {
            string_push(active_buffer, "SUBS\n");
            break;
        }
        case(ins_muls) : {
            string_push(active_buffer, "MULS\n");
            break;
        }
        case(ins_divs) : {
            string_push(active_buffer, "DIVS\n");
            break;
        }
        case(ins_idivs) : {
            string_push(active_buffer, "IDIVS\n");
            break;
        }
        case(ins_lt) : {
            push_var_s_s_ins("LT ", op1, op2);
            break;
        }
        case(ins_gt) : {
            push_var_s_s_ins("GT ", op1, op2);
            break;
        }
        case(ins_eq) : {
            push_var_s_s_ins("EQ ", op1, op2);
            break;
        }
        case(ins_eqs) : {
            string_push(active_buffer, "EQS\n");
        }
        case(ins_gts) : {
            string_push(active_buffer, "GTS\n");
        }
        case(ins_lts) : {
            string_push(active_buffer, "LTS\n");
        }
        case(ins_and) : {
            push_var_s_s_ins("AND ", op1, op2);
            break;
        } 
        case(ins_or) : {
            push_var_s_s_ins("OR ", op1, op2);
            break;
        } 
        case(ins_not) : {
            push_var_s_s_ins("NOT ", op1, op2);
            break;
        }
        case(ins_ands) : {
            string_push(active_buffer, "ANDS\n");
            break;
        }
        case(ins_ors) : {
            string_push(active_buffer, "ORS\n");
            break;
        }
        case(ins_nots) : {
            string_push(active_buffer, "NOTS\n");
            break;
        }
        case(ins_str_2_int) : {
            push_var_s_s_ins("STRI2INT ", op1, op2);
            break;
        }
        case(ins_int_2_float) : {
            gen_return_val();
            string_push(active_buffer,"INT2FLOAT ");
            string_push(active_buffer,"LF@retval ");
            string_push(active_buffer,"LF@");
            string_push(active_buffer,op1);
            string_push(active_buffer,"\n");
            break;
        }
        case(ins_float_2_int) : {
            gen_return_val();
            string_push(active_buffer,"FLOAT2INT ");
            string_push(active_buffer,"LF@retval ");
            string_push(active_buffer,"LF@");
            string_push(active_buffer,op1);
            string_push(active_buffer,"\n");
            break;
        }
        case(ins_int_2_char) : {
            gen_return_val();
            string_push(active_buffer,"INT2CHAR ");
            string_push(active_buffer,"LF@retval ");
            string_push(active_buffer,"LF@");
            string_push(active_buffer,op1);
            string_push(active_buffer,"\n");
            break;
        }
        case(ins_int_2_floats) : {
            string_push(active_buffer, "INT2FLOATS\n");
            break;
        }
        case(ins_float_2_ints) : {
            string_push(active_buffer, "FLOAT2INTS\n");
            break;
        }
        case(ins_int_2_chars) : {
            string_push(active_buffer, "INT2CHARS\n");
            break;
        }
        case(ins_str_2_ints) : {
            string_push(active_buffer, "STRI2INTS\n");
            break;
        }
        default: {
            printf("pomoc");
            break;
        }
    } 
    
    }
