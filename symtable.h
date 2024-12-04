/**
 * @file symtable.h
 * @brief Symtable for the IFJ project.
 *
 * @author Marko Poľanský (xpolan12)
 *
 * @date 2024
 */
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#ifndef IFJ_PROJEKT_SYMTABLE_H
#define IFJ_PROJEKT_SYMTABLE_H

#ifndef HASH_TABLE_SIZE
/*
 * no more than 2000 items expected =>
 * filled table should be filled max up to 75% =>
 * table size should be prime number => 2671   todo check
 */

#define HASH_TABLE_SIZE 2671
#endif

#ifndef SCOPE_IDENTIFIER_SIZE
#define SCOPE_IDENTIFIER_SIZE 10
#endif

typedef enum {
    DATA_TYPE_INT = 0,
    DATA_TYPE_INT_CONVERTABLE = 1,
    DATA_TYPE_INT_NULLABLE = 2,
    DATA_TYPE_FLOAT = 3 ,
    DATA_TYPE_FLOAT_CONVERTABLE = 4,
    DATA_TYPE_FLOAT_NULLABLE = 5,
    DATA_TYPE_INT_FLOAT_CONVERTABLE = 6,
    DATA_TYPE_STRING,
    DATA_TYPE_STRING_NULLABLE,
    DATA_TYPE_NULL,
    DATA_TYPE_NONE,
    DATA_TYPE_BOOL,
    ERR = -1
} DataTypeVariable;

typedef enum {
    RETURN_TYPE_INT,
    RETURN_TYPE_FLOAT,
    RETURN_TYPE_STRING,
    RETURN_TYPE_INT_NULLABLE,
    RETURN_TYPE_FLOAT_NULLABLE,
    RETURN_TYPE_STRING_NULLABLE,
    RETURN_TYPE_VOID,
    RETURN_TYPE_UNEXPECTED
} ReturnTypes;


typedef enum {
    NONE,
    VAR,
    CONST
} Variable_Type;

typedef struct Fn_Params{
    struct Fn_Params* next;
    char* name;
    DataTypeVariable data_type;
    bool is_used;
}Fn_Params;

typedef struct tdata_Fn{
    int params_count;
    Fn_Params* params;
    ReturnTypes return_type;
    bool contains_return;
}TData_Fn;

typedef struct tdata_variable{
    DataTypeVariable data_type;
    Variable_Type variable_type;

}TData_Variable;

typedef enum {
    GLOBAL_FRAME,
    FUNCTION_FRAME,
    INNER_FRAME,
    VARIABLE_FRAME
} Frame_Type;

typedef struct tdata{
    size_t number_of_inner_frames;
    char* key;
    bool is_used;
    bool is_mutated;
    char scope[SCOPE_IDENTIFIER_SIZE];
    struct tdata** hash_table ;
    Frame_Type frame_type;
    union {
        TData_Fn* function;
        TData_Variable* variable;
    };
}TData;


typedef struct sym_table{
    TData *global_frame;
} SymTable;

size_t get_index(const char *str);
size_t get_step(size_t index);
SymTable* init_sym_table();


TData_Fn* create_tdata_fn(ReturnTypes return_type);
TData_Variable* create_tdata_variable(DataTypeVariable data_type, Variable_Type variable_type);
Fn_Params* create_fn_params(const char* name, DataTypeVariable data_type);
bool add_fn_param(TData_Fn* function, const char* name, DataTypeVariable data_type);
void free_fn_params(Fn_Params* params);



TData* sym_table_create_data(const char* key,char* scope,Frame_Type frame_type);
TData* sym_table_search(TData *frame,char* key);
void sym_table_insert(TData *frame,TData* new_frame);
void sym_table_create_insert_global_frame(SymTable *table);

void free_tdata(TData* tdata);
void free_sym_table(SymTable* symtable);

#endif //IFJ_PROJEKT_SYMTABLE_H
