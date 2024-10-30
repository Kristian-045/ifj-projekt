#include <stdio.h>
#include "../symtable.h"

int test_insert_one_frame__exists(){
    SymTable *symTable = init_sym_table();
    SymTableItemFrame *frame = init_sym_table_item_frame(HASH_TABLE_SIZE);

    DataType type = INT;
    DataValue value;
    value.int_val = 512;
    sym_table_item_frame_insert(frame, create_tdata("asd",&type,&value)); //index 388
    if(frame->hash_table[388]->data->int_val == value.int_val &&
       *frame->hash_table[388]->data_type == type &&
       strcmp(frame->hash_table[388]->key,"asd") == 0)
        return 1;
    return 0;
}

int main(){
    if(test_insert_one_frame__exists() != 1)
        fprintf(stderr,"error not expected");


    return 0;
}

