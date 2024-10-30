#include "symtable.h"


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
        perror("symtable.c init_sym_table: Chyba pri alokacii");
        exit(99);
    }
    table->current_frame = NULL;
    table->first_frame = NULL;
    table->frames_count = 0;
    return table;
}

SymTableItemFrame* init_sym_table_item_frame(size_t hash_table_size){
    SymTableItemFrame *frame = calloc(1,sizeof (SymTableItemFrame));
    if(frame == NULL){
        perror("symtable.c init_sym_table_item_frame: Chyba pri alokacii");
        exit(99);
    }

    TData** hash_table = calloc(hash_table_size,sizeof (TData*));
    if(hash_table == NULL){
        perror("symtable.c init_sym_table_item_frame: Chyba pri alokacii");
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
        perror("symtable.c init_tdata: Chyba pri alokacii");
        exit(99);
    }
    data->next = NULL;
    data->data_type = NULL;
    data->data = NULL;
    data->key = NULL;
    return data;
}

size_t sym_table_frames_count(SymTable *table){
    return table->frames_count;
}

SymTableItemFrame* sym_table_get_current_frame(SymTable *table){
    return table->current_frame;
}


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

void sym_table_add_frame(SymTable *table,SymTableItemFrame *frame){
    if(table == NULL)
        return;

    table->first_frame =frame;
}
TData* sym_table_item_frame_search(SymTableItemFrame *frame,char* key){
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

void sym_table_item_frame_insert(SymTableItemFrame *frame,TData* tData){
    if(frame == NULL || tData == NULL || tData->key == NULL)
        return;
    size_t index = get_index(tData->key);
    TData *first = frame->hash_table[index];
    TData *existing = sym_table_item_frame_search(frame,tData->key);

    if(existing == NULL){
        frame->hash_table[index] = tData;
        frame->hash_table[index]->next = first;;
    }
    else{
        existing = tData;
    }
}

TData* create_tdata(char* key,DataType* type,DataValue* value){
    TData *data = init_tdata();
    data->key = key;
    data->data_type = type;
    data->data = value;
    return data;
}