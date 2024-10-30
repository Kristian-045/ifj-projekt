//
// Created by marko on 30. 10. 2024.
//
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifndef IFJ_PROJEKT_SYMTABLE_H
#define IFJ_PROJEKT_SYMTABLE_H

#ifndef HASH_TABLE_SIZE
#define HASH_TABLE_SIZE 1066 //prime expect to cover 75% of data
#endif

typedef enum {
    INT,
    FLOAT,
    STRING,
    ONLY_KEYWORD
} DataType;

// Union for holding different data types
typedef union {
    int int_val;
    double float_val;
    char *string_val;
} DataValue;

typedef struct tdata{
    struct tdata *next;
    DataType* data_type;
    DataValue* data;
    char* key;
}TData;

typedef struct sym_table_item_frame{
    size_t bucket_count;
    size_t data_count;
    struct sym_table_item_frame *next;
    TData** hash_table ;
} SymTableItemFrame;

typedef struct sym_table{
    SymTableItemFrame *current_frame;
    SymTableItemFrame *first_frame;
    size_t frames_count;

} SymTable;

size_t get_index(const char *str);
SymTable* init_sym_table();
SymTableItemFrame* init_sym_table_item_frame(const size_t hash_table_size);
TData* init_tdata();
size_t sym_table_frames_count(SymTable *table);
SymTableItemFrame* sym_table_get_current_frame(SymTable *table);
void sym_table_next(SymTable *table);
void sym_table_set_current_to_first(SymTable *table);

TData* sym_table_item_frame_search(SymTableItemFrame *frame,char* key);
void sym_table_item_frame_insert(SymTableItemFrame *frame,TData* data);

void sym_table_add_frame(SymTable *table,SymTableItemFrame *frame);

TData* create_tdata(char* key,DataType* type,DataValue* value);

#endif //IFJ_PROJEKT_SYMTABLE_H
