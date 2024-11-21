#include "symtable.h"
#include <stdio.h>


size_t get_index(const char *str) {
    uint32_t h=0; // musí mít 32 bitů
    const unsigned char *p;
    for(p=(const unsigned char*)str; *p!='\0'; p++)
        h = 65599*h + *p;
    return h % HASH_TABLE_SIZE;
}

SymTable* init_sym_table(){
    SymTable *table = calloc(1,sizeof (SymTable));
    if(table == NULL){
        fprintf(stderr, "symtable.c init_sym_table: Chyba pri alokacii");
        exit(99);
    }
    table->current_frame = NULL;
    table->first_frame = NULL;
    table->frames_count = 0;
    return table;
}

/*
SymTableItemFrame* init_sym_table_item_frame(size_t hash_table_size){
    SymTableItemFrame *frame = calloc(1,sizeof (SymTableItemFrame));
    if(frame == NULL){
        fprintf(stderr, "symtable.c init_sym_table_item_frame: Chyba pri alokacii");
        exit(99);
    }

    TData** hash_table = calloc(hash_table_size,sizeof (TData*));
    if(hash_table == NULL){
        fprintf(stderr, "symtable.c init_sym_table_item_frame: Chyba pri alokacii");
        exit(99);
    }
    for (size_t i = 0; i < hash_table_size; ++i) {
        hash_table[i] = NULL;
    }
    frame->hash_table = hash_table;
    frame->next = NULL;
    frame->bucket_count = hash_table_size;
    frame->data_count = 0;
    return frame;
}

TData* init_tdata(){
    TData *data = calloc(1,sizeof (TData));
    if(data == NULL){
        fprintf(stderr, "symtable.c init_tdata: Chyba pri alokacii");
        exit(99);
    }
    data->next = NULL;
    data->data_type = NULL;
    data->variable_type = NULL;
    data->key = NULL;
    return data;
}

size_t sym_table_frames_count(SymTable *table){
    return table->frames_count;
}

TData* sym_table_get_current_frame(SymTable *table){
    return table->current_frame;
}




void sym_table_add_frame(SymTable *table,SymTableItemFrame *frame){
    if(table == NULL)
        return;

    table->first_frame =frame;
}
*/

void sym_table_next(SymTable *table){
    if(table== NULL && table->current_frame != NULL)
        return;
    table->current_frame = table->current_frame->next;
}
void sym_table_set_current_to_first(SymTable *table){
    if(table== NULL)
        return;
    table->current_frame = table->first_frame;
}


TData* sym_table_create_data(const char* key,char* scope_level,Frame_Type frame_type){
    if (key == NULL )
        return NULL;

    TData *new_frame = calloc(1,sizeof (TData));
    if (new_frame == NULL){
        fprintf(stderr, "symtable.c sym_table_create_data: Chyba pri alokacii");
        exit(99);
    }

    TData** hash_table = calloc(HASH_TABLE_SIZE,sizeof (TData*));
    if(hash_table == NULL){
        fprintf(stderr, "symtable.c sym_table_create_data: Chyba pri alokacii");
        exit(99);
    }
    for (size_t i = 0; i < HASH_TABLE_SIZE; ++i) {
        hash_table[i] = NULL;
    }
    new_frame->next = NULL;
    new_frame->key = key ? strdup(key) : NULL;
    new_frame->is_used = false;
    strncpy(new_frame->scope, scope_level, sizeof(new_frame->scope)-1);
    new_frame->scope[sizeof(new_frame->scope) - 1] = '\0';
    new_frame->hash_table = hash_table;
    new_frame->frame_type = frame_type;
    new_frame->number_of_inner_frames = 0;

    if (frame_type == FUNCTION_FRAME) {
        new_frame->function = (TData_Fn*)calloc(1,sizeof(TData_Fn));
        if (!new_frame->function) {
            free(new_frame->key);
            free(new_frame);
            fprintf(stderr, "symtable.c sym_table_create_data: Chyba pri alokacii");
            exit(99);

        }
        new_frame->function->params = NULL;
        new_frame->function->return_type = -1;
    } else if (frame_type == VARIABLE_FRAME) {
        new_frame->variable = (TData_Variable*)calloc(1,sizeof(TData_Variable));
        if (!new_frame->variable) {
            free(new_frame->key);
            free(new_frame);
            fprintf(stderr, "symtable.c sym_table_create_data: Chyba pri alokacii");
            exit(99);
        }
        new_frame->variable->data_type = -1;
        new_frame->variable->variable_type = -1;
    }
    return new_frame;
}

TData_Fn* create_tdata_fn(ReturnTypes return_type) {
    TData_Fn* fn = (TData_Fn*)calloc(1,sizeof(TData_Fn));
    if (fn == NULL) {
        fprintf(stderr, "symtable.c create_tdata_fn: Chyba pri alokacii");
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
        fprintf(stderr, "symtable.c create_tdata_variable: Chyba pri alokacii");
        exit(99);
    }
    variable->data_type = data_type;
    variable->variable_type = variable_type;
    return variable;
}

Fn_Params* create_fn_params(const char* name, DataTypeVariable data_type) {
    Fn_Params* param = (Fn_Params*)malloc(sizeof(Fn_Params));
    if (param == NULL) {
        fprintf(stderr, "symtable.c create_fn_params: Chyba pri alokacii");
        exit(99);
    }

    param->next = NULL;
    param->name = name ? strdup(name) : NULL;
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

    free(tdata->key);

    if (tdata->function) {
        free_fn_params(tdata->function->params);
        free(tdata->function);
    }

    if (tdata->variable) {
        free(tdata->variable);
    }

    free(tdata);
}

TData* sym_table_search(TData *frame,char* key){
    if(frame == NULL)
        return NULL;
    size_t index = get_index(key);
    TData *item = frame->hash_table[index];
    while (item != NULL){
        if(strcmp(item->key,key) == 0)
            return item;
        item = item->next;
    }
    return NULL;
}

void sym_table_insert(TData* frame,TData* new_frame){
    if(frame == NULL || new_frame == NULL || new_frame->key == NULL)
        return;

    size_t index = get_index(new_frame->key);
    TData *first = frame->hash_table[index];
    TData *existing = sym_table_search(frame,new_frame->key);


    if(existing == NULL){
        frame->hash_table[index] = new_frame;
        frame->hash_table[index]->next = first;;
        if(new_frame->frame_type == INNER_FRAME)
            frame->number_of_inner_frames++;
    }
    else{
        free_tdata(existing);
        existing = new_frame;
    }
}
void sym_table_create_insert_global_frame(SymTable *table){
    TData *global_frame = calloc(1,sizeof (TData));
    if (global_frame == NULL){
        fprintf(stderr, "symtable.c sym_table_create_insert_global_frame: Chyba pri alokacii");
        exit(99);
    }

    TData** hash_table = calloc(HASH_TABLE_SIZE,sizeof (TData*));
    if(hash_table == NULL){
        fprintf(stderr, "symtable.c init_sym_table_item_frame: Chyba pri alokacii");
        exit(99);
    }
    for (size_t i = 0; i < HASH_TABLE_SIZE; ++i) {
        hash_table[i] = NULL;
    }

    global_frame->key = "global";
    global_frame->next = NULL;
    global_frame->hash_table = hash_table;
    global_frame->frame_type = GLOBAL_FRAME;
    global_frame->function = NULL;
    global_frame->variable = NULL;
    //bool is_used = 0;
    memset(global_frame->scope, 0, sizeof(global_frame->scope));
    global_frame->scope[0] = 1;


    table->first_frame = global_frame;
}




/*
TData* create_tdata(char* key,DataType* type,Variable_Type * value){
    TData *data = init_tdata();
    data->key = key;
    data->data_type = type;
    data->variable_type = value;
    return data;
}
 */