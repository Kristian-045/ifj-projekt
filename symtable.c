/**
 * @file symtable.c
 * @brief Symtable for the IFJ project.
 *
 * @author Marko Poľanský (xpolan12)
 *
 * @date 2024
 */


#include "symtable.h"
#include <stdio.h>
#include <string.h>

size_t get_index(const char *str) {
    uint32_t h=0;
    const unsigned char *p;
    for(p=(const unsigned char*)str; *p!='\0'; p++)
        h = 65599*h + *p;
    return h % HASH_TABLE_SIZE;
}
size_t get_step(size_t index){
    return 1 + (index % (HASH_TABLE_SIZE - 1));
}

SymTable* init_sym_table(){
    SymTable *table = calloc(1,sizeof (SymTable));
    if(table == NULL){
        fprintf(stderr, "symtable.c init_sym_table: Memory allocation failed!\n");
        exit(99);
    }
    table->global_frame = NULL;

    return table;
}



TData* sym_table_create_data(const char* key,char* scope_level,Frame_Type frame_type){
    if (key == NULL )
        return NULL;

    TData *new_frame = calloc(1,sizeof (TData));
    if (new_frame == NULL){
        fprintf(stderr, "symtable.c sym_table_create_data: Memory allocation failed!\n");
        exit(99);
    }

    TData** hash_table = calloc(HASH_TABLE_SIZE,sizeof (TData*));
    if(hash_table == NULL){
        fprintf(stderr, "symtable.c sym_table_create_data: Memory allocation failed!\n");
        exit(99);
    }
    for (size_t i = 0; i < HASH_TABLE_SIZE; ++i) {
        hash_table[i] = NULL;
    }
    new_frame->key = malloc(strlen(key) + 1);
    strcpy(new_frame->key, key);

    new_frame->is_used = false;
    new_frame->is_mutated= false;
    strncpy(new_frame->scope, scope_level, sizeof(new_frame->scope)-1);
    new_frame->scope[sizeof(new_frame->scope) - 1] = '\0';
    new_frame->hash_table = hash_table;
    new_frame->frame_type = frame_type;
    new_frame->number_of_inner_frames = 0;
    /*
     if (frame_type == FUNCTION_FRAME) {
         new_frame->function = (TData_Fn*)calloc(1,sizeof(TData_Fn));
         if (!new_frame->function) {
             //free(new_frame->key);
             free(new_frame);
             fprintf(stderr, "symtable.c sym_table_create_data: Memory allocation failed!\n");
             exit(99);

         }
         new_frame->function->params = NULL;
         new_frame->function->return_type = -1;
     } else if (frame_type == VARIABLE_FRAME) {
         new_frame->variable == NULL;

         new_frame->variable = (TData_Variable*)calloc(1,sizeof(TData_Variable));
         if (!new_frame->variable) {
             //free(new_frame->key);
             free(new_frame);
             fprintf(stderr, "symtable.c sym_table_create_data: Memory allocation failed!\n");
             exit(99);
         }
         new_frame->variable->data_type = -1;
         new_frame->variable->variable_type = -1;

     }
     */

     return new_frame;
 }

 TData_Fn* create_tdata_fn(ReturnTypes return_type) {
     TData_Fn* fn = (TData_Fn*)calloc(1,sizeof(TData_Fn));
     if (fn == NULL) {
         fprintf(stderr, "symtable.c create_tdata_fn: Memory allocation failed!\n");
         exit(99);
     }

     fn->params = NULL;
     fn->return_type = return_type;
     fn->params_count = 0;

     return fn;
 }

 TData_Variable* create_tdata_variable(DataTypeVariable data_type, Variable_Type variable_type) {
     TData_Variable* variable = (TData_Variable*)malloc(sizeof(TData_Variable));
     if (variable == NULL) {
         fprintf(stderr, "symtable.c create_tdata_variable: Memory allocation failed!\n");
         exit(99);
     }
     variable->data_type = data_type;
     variable->variable_type = variable_type;
     return variable;
 }

 Fn_Params* create_fn_params(const char* name, DataTypeVariable data_type) {
     Fn_Params* param = (Fn_Params*)malloc(sizeof(Fn_Params));
     if (param == NULL) {
         fprintf(stderr, "symtable.c create_fn_params: Memory allocation failed!\\n");
         exit(99);
     }

     param->next = NULL;
     param->name =  malloc(strlen(name) + 1);
     strcpy( param->name, name);
     param->data_type = data_type;
     param->is_used = false;
     return param;
 }


 bool add_fn_param(TData_Fn* function, const char* name, DataTypeVariable data_type) {
     if (function == NULL){
         return false;
     }

     Fn_Params* param = create_fn_params(name, data_type);
     param->next = function->params;
     function->params = param;
     function->params_count++;
     return true;
 }
 void free_fn_params(Fn_Params* params) {
     if (params == NULL)
         return;

     while (params) {
         Fn_Params* next = params->next;
         free(params->name);
         free(params);
         params = next;
     }
 }

 void free_tdata(TData* tdata) {
     if (tdata == NULL)
         return;

     for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
         if(tdata->hash_table[i] == NULL)
             continue;


         switch (tdata->hash_table[i]->frame_type) {
             case VARIABLE_FRAME:{
                 free(tdata->hash_table[i]->key);
                 free(tdata->hash_table[i]->variable);
                 free(tdata->hash_table[i]->hash_table);
                 free(tdata->hash_table[i]);
                 break;
             }
             case FUNCTION_FRAME:{
                 free_tdata(tdata->hash_table[i]);
                // if(strncmp(tdata->hash_table[i]->key, "ifj.", 4) == 0){

                 free_fn_params(tdata->hash_table[i]->function->params);
                 free(tdata->hash_table[i]->function);
                 free(tdata->hash_table[i]->key);
                 free(tdata->hash_table[i]->hash_table);
                 free(tdata->hash_table[i]);
                 break;
             }
             case INNER_FRAME:{
                 free_tdata(tdata->hash_table[i]);
                 free(tdata->hash_table[i]->key);
                 free(tdata->hash_table[i]->hash_table);
                 free(tdata->hash_table[i]);
                 break;
             }
             case GLOBAL_FRAME:{
                 break;
             }

         }
     }


 }

 void free_sym_table(SymTable* symtable){
     free_tdata(symtable->global_frame);
     free(symtable->global_frame->hash_table);
     free(symtable->global_frame);
     free(symtable);
 }

 TData* sym_table_search(TData *frame,char* key){
     if(frame == NULL)
         return NULL;

     size_t index = get_index(key);
     size_t step = get_step(index);


     for (int i = 0; i < HASH_TABLE_SIZE; i++) {
         if (frame->hash_table[index] == NULL) {
             return NULL;
         }
         else if(strcmp(frame->hash_table[index]->key , key) == 0){
             return frame->hash_table[index];
         }
         index = (index + step) % HASH_TABLE_SIZE;
     }

     return NULL;
 }

 void sym_table_insert(TData* frame,TData* new_frame){
     if(frame == NULL || new_frame == NULL || new_frame->key == NULL)
         return;

     size_t index = get_index(new_frame->key);
     size_t step = get_step(index);

     for (int i = 0; i < HASH_TABLE_SIZE; i++) {
         if (frame->hash_table[index] == NULL) {
             frame->hash_table[index] = new_frame;
             if(new_frame->frame_type == INNER_FRAME){
                 frame->number_of_inner_frames++;
             }
             break;
         }
         index = (index + step) % HASH_TABLE_SIZE;
     }

 }
 void sym_table_create_insert_global_frame(SymTable *table){
     TData *global_frame = calloc(1,sizeof (TData));
     if (global_frame == NULL){
         fprintf(stderr, "symtable.c sym_table_create_insert_global_frame: Memory allocation failed!\n");
         exit(99);
     }

     TData** hash_table = calloc(HASH_TABLE_SIZE,sizeof (TData*));
     if(hash_table == NULL){
         fprintf(stderr, "symtable.c init_sym_table_item_frame: Memory allocation failed!\n");
         exit(99);
     }
     for (size_t i = 0; i < HASH_TABLE_SIZE; ++i) {
         hash_table[i] = NULL;
     }

     global_frame->key = "global";
     global_frame->hash_table = hash_table;
     global_frame->frame_type = GLOBAL_FRAME;
     global_frame->function = NULL;
     global_frame->variable = NULL;
     //bool is_used = 0;
     memset(global_frame->scope, 0, sizeof(global_frame->scope));
     global_frame->scope[0] = 1;

     table->global_frame = global_frame;
 }

