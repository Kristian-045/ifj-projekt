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

/**
 * @brief Computes the hash index for a given string.
 *
 * @param str The string to hash.
 * @return A size_t value representing the hash index modulo the hash table size.
 */
size_t get_index(const char *str) {
    uint32_t h=0;
    const unsigned char *p;
    for(p=(const unsigned char*)str; *p!='\0'; p++)
        h = 65599*h + *p;
    return h % HASH_TABLE_SIZE;
}
/**
 * @brief Computes the step size for probing in case of hash collisions.
 *
 * @param index The current index value.
 * @return A size_t step size based on the hash table size.
 */
size_t get_step(size_t index){
    return 1 + (index % (HASH_TABLE_SIZE - 1));
}

/**
 * @brief Initializes a new symbol table.
 *
 * Allocates memory for the symbol table and sets its global frame to NULL.
 * If memory allocation fails, the program exits with an error code.
 *
 * @return A pointer to the newly initialized symbol table.
 */
SymTable* init_sym_table(){
    SymTable *table = calloc(1,sizeof (SymTable));
    if(table == NULL){
        fprintf(stderr, "symtable.c init_sym_table: Memory allocation failed!\n");
        exit(99);
    }
    table->global_frame = NULL;

    return table;
}


/**
 * @brief Creates a new symbol table entry (frame).
 *
 * Allocates and initializes a new frame with the given key, scope, and frame type.
 *
 * @param key The key for the frame.
 * @param scope_level The scope level associated with the frame.
 * @param frame_type The type of the frame (e.g., GLOBAL_FRAME, FUNCTION_FRAME).
 * @return A pointer to the newly created frame.
 */
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


     return new_frame;
 }
/**
 * @brief Creates a new function-specific data structure.
 *
 * @param return_type The return type of the function.
 * @return A pointer to the newly created function data structure.
 */
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

/**
* @brief Creates a new variable-specific data structure.
*
* @param data_type The data type of the variable.
* @param variable_type The type of the variable (e.g., VAR or CONST).
* @return A pointer to the newly created variable data structure.
*/
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

/**
* @brief Creates a new function parameter structure.
*
* @param name The name of the parameter.
* @param data_type The data type of the parameter.
* @return A pointer to the newly created parameter structure.
*/
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

/**
 * @brief Adds a parameter to a function's parameter list.
 *
 * @param function The function to which the parameter is added.
 * @param name The name of the parameter.
 * @param data_type The data type of the parameter.
 * @return true if the parameter was added successfully, false otherwise.
 */
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

/**
 * @brief Frees the memory used by a parameter list.
 *
 * @param params The parameter list to free.
 */
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

/**
* @brief Frees the memory used by a symbol table frame.
*
* @param tdata The frame to free.
*/
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

/**
* @brief Frees the entire symbol table.
*
* @param symtable The symbol table to free.
*/
 void free_sym_table(SymTable* symtable){
     free_tdata(symtable->global_frame);
     free(symtable->global_frame->hash_table);
     free(symtable->global_frame);
     free(symtable);
 }

/**
* @brief Searches for a key in the hash table of a given frame.
*
* The function uses double hashing for collision resolution. It computes an index using the key and iterates through
* the hash table using a step size until it finds a matching key or determines the key is not present.
*
* @param frame The frame in which to search for the key.
* @param key The key to search for.
* @return A pointer to the TData structure associated with the key if found, NULL otherwise.
*/
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

/**
* @brief Inserts a new frame into the hash table of an existing frame.
*
* The function uses double hashing to find an empty slot for the new frame.
* If the frame type of the new entry is INNER_FRAME, the count of inner frames is incremented.
*
* @param frame The parent frame where the new frame will be inserted.
* @param new_frame The new frame to be inserted.
*/
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

/**
* @brief Creates and inserts the global frame into the symbol table.
*
* This function initializes the global frame of the symbol table, allocating memory for it and its hash table.
* The global frame acts as the root frame for all subsequent entries in the symbol table.
*
* @param table The symbol table to which the global frame will be added.
*/
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

