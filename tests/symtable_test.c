#include <stdio.h>
#include "../symtable.h"

int test_insert_one_FN_frame__exists(){
    SymTable *symTable = init_sym_table();
    sym_table_create_insert_global_frame(symTable);
    TData* fn_Frame =sym_table_create_data("main","\001",FUNCTION_FRAME);
    TData_Fn* fn = create_tdata_fn(RETURN_TYPE_VOID);
    add_fn_param(fn,"ASD",INT);
    add_fn_param(fn,"ASssD",FLOAT);
    fn_Frame->function = fn;

    sym_table_set_current_to_first(symTable);
    sym_table_insert(symTable->current_frame,fn_Frame);
    TData *curr = sym_table_search(symTable->current_frame,"main");
    if(curr->frame_type == FUNCTION_FRAME && strcmp(curr->function->params->name,"ASssD") == 0 &&
            curr->function->params->data_type == FLOAT &&
            strcmp(curr->function->params->next->name,"ASD") == 0 &&
            curr->function->params->next->data_type == INT
    )
        return  1;
    else
        return  0;
}

int test_insert_one_VARIABLE_frame_in_FN_frame_exists(){
    SymTable *symTable = init_sym_table();
    sym_table_create_insert_global_frame(symTable);
    TData* fn_Frame =sym_table_create_data("main","\001",FUNCTION_FRAME);
    TData_Fn* fn = create_tdata_fn(RETURN_TYPE_VOID);
    add_fn_param(fn,"ASD",INT);
    add_fn_param(fn,"ASssD",FLOAT);
    fn_Frame->function = fn;
    sym_table_set_current_to_first(symTable);
    sym_table_insert(symTable->current_frame,fn_Frame);
    TData *curr = sym_table_search(symTable->current_frame,"main");

    TData* variable_Frame =sym_table_create_data("i","\002",VARIABLE_FRAME);
    TData_Variable * var = create_tdata_variable(INT,CONST);
    variable_Frame->variable = var;
    sym_table_insert(curr,variable_Frame);
    TData *curr_var = sym_table_search(curr,"i");

    if(curr_var->frame_type == VARIABLE_FRAME &&curr_var->variable->data_type == INT &&
    curr_var->variable->variable_type == CONST)
        return  1;
    else
        return  0;
}


int test_insert_multiple_VARIABLE_frame_in_FN_frame_exists(){
    SymTable *symTable = init_sym_table();
    sym_table_create_insert_global_frame(symTable);
    TData* fn_Frame =sym_table_create_data("main","\001",FUNCTION_FRAME);
    TData_Fn* fn = create_tdata_fn(RETURN_TYPE_VOID);
    add_fn_param(fn,"ASD",INT);
    add_fn_param(fn,"ASssD",FLOAT);
    fn_Frame->function = fn;
    sym_table_set_current_to_first(symTable);
    sym_table_insert(symTable->current_frame,fn_Frame);
    TData *curr = sym_table_search(symTable->current_frame,"main");

    TData* variable_Frame =sym_table_create_data("i","\002",VARIABLE_FRAME);
    TData_Variable * var = create_tdata_variable(INT,CONST);
    variable_Frame->variable = var;
    sym_table_insert(curr,variable_Frame);

    TData* variable_Frame2 =sym_table_create_data("b","\002",VARIABLE_FRAME);
    TData_Variable * var2 = create_tdata_variable(INT,CONST);
    variable_Frame2->variable = var2;
    sym_table_insert(curr,variable_Frame2);

    TData *curr_var = sym_table_search(curr,"b");
    TData *curr_vari = sym_table_search(curr,"i");
    if(curr_var->frame_type == VARIABLE_FRAME &&curr_var->variable->data_type == INT &&
       curr_var->variable->variable_type == CONST && strcmp(curr_vari->key,"i") == 0 )
        return  1;
    else
        return  0;
}

int test_insert_multiple_VARIABLE_frame_in_new_INNER_FRAME_in_FN_frame_exists(){
    SymTable *symTable = init_sym_table();
    sym_table_create_insert_global_frame(symTable);
    TData* fn_Frame =sym_table_create_data("main","\001",FUNCTION_FRAME);
    TData_Fn* fn = create_tdata_fn(RETURN_TYPE_VOID);
    add_fn_param(fn,"ASD",INT);
    add_fn_param(fn,"ASssD",FLOAT);
    fn_Frame->function = fn;
    sym_table_set_current_to_first(symTable);
    sym_table_insert(symTable->current_frame,fn_Frame);
    TData *curr = sym_table_search(symTable->current_frame,"main");

    TData* inner_Frame =sym_table_create_data("frame_1","\002",INNER_FRAME);
    sym_table_insert(curr,inner_Frame);
    TData *curr_frame = sym_table_search(curr,"frame_1");

    char * sco =curr_frame->scope;
    int len =strlen(sco);
    sco[strlen(sco)] = 1;
    TData* variable_Frame =sym_table_create_data("i",sco,VARIABLE_FRAME);
    TData_Variable * var = create_tdata_variable(INT,CONST);
    variable_Frame->variable = var;
    sym_table_insert(curr,variable_Frame);

    TData* variable_Frame2 =sym_table_create_data("b",sco,VARIABLE_FRAME);
    TData_Variable * var2 = create_tdata_variable(INT,CONST);
    variable_Frame2->variable = var2;
    sym_table_insert(curr,variable_Frame2);

    TData *curr_var = sym_table_search(curr,"b");
    TData *curr_vari = sym_table_search(curr,"i");
    if(curr_var->frame_type == VARIABLE_FRAME &&curr_var->variable->data_type == INT &&
       curr_var->variable->variable_type == CONST && strcmp(curr_vari->key,"i") == 0  &&
            strcmp(curr_var->scope, "\002\001") == 0 )
        return  1;
    else
        return  0;
}

int main(){
    if(test_insert_one_FN_frame__exists() != 1)
        fprintf(stderr,"error test_insert_one_FN_frame__exists not expected");
    if(test_insert_one_VARIABLE_frame_in_FN_frame_exists() != 1)
        fprintf(stderr,"error test_insert_one_VARIABLE_frame_in_FN_frame_exists not expected");
    if(test_insert_multiple_VARIABLE_frame_in_FN_frame_exists() != 1)
        fprintf(stderr,"error test_insert_multiple_VARIABLE_frame_in_FN_frame_exists not expected");
    if(test_insert_multiple_VARIABLE_frame_in_new_INNER_FRAME_in_FN_frame_exists() != 1)
        fprintf(stderr,"error test_insert_multiple_VARIABLE_frame_in_new_INNER_FRAME_in_FN_frame_exists not expected");


    return 0;
}

