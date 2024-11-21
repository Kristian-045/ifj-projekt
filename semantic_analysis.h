#include "scanner.h"
#include "parser.h"
#include "symtable.h"
#include "frame_list.h"

#ifndef IFJ_PROJEKT_SEMANTIC_ANALYSIS_H
#define IFJ_PROJEKT_SEMANTIC_ANALYSIS_H




void first_pass(NodePtr node,SymTable* sym_table);
void second_pass(NodePtr node,SymTable* sym_table);
void semantic_parse(NodePtr node);
void parse_function_params(TData_Fn* data_fn,NodePtr node);
ReturnTypes parse_function_return_value(NodePtr node);
void parse_body(TData* function_frame,TData* current_frame,NodePtr node);
DataTypeVariable map_node_to_DataTypeVariable(NodePtr node);
ReturnTypes map_node_to_ReturnTypes(NodePtr node);
Variable_Type map_tType_to_Variable_Type(tType type);
DataTypeVariable map_ReturnType_to_DataTypeVariable(ReturnTypes type);

bool is_data_type_nullable(DataTypeVariable data_type);
DataTypeVariable compare_variable_expression(DataTypeVariable left_type, DataTypeVariable right_type);
DataTypeVariable compare_data_type_variables(DataTypeVariable left_type, DataTypeVariable right_type);
bool is_bool_expression_ok(DataTypeVariable left_type, DataTypeVariable right_type);



void parse_declaration_stmt(TData* frame,TData* function_frame,NodePtr node);
void parse_expression(TData* frame,NodePtr node);
void parse_left(TData* frame,NodePtr node);

DataTypeVariable get_not_null_data_type_from_nullable(DataTypeVariable data_type_of_bool_expression);

bool is_returned_value_correct(ReturnTypes expected_from_fn, DataTypeVariable returned);

DataTypeVariable check_expression(TData* global_frame,TData* function_frame,TData* current_frame,NodePtr node);

void check_assigment_to_non_declared(TData* in_frame, char* var_name); // TODO delete
void check_redefinition_of_const(TData* in_frame, char* var_name); //TODO delete
void check_assigment_to_function_params(TData* function_frame,NodePtr node); // TODO delete

TData* check_assigment_only_to_var_is_accepted(TData* in_frame,char* name);

void check_fn_params(TData* global_frame,TData* function_frame,TData* current_frame,NodePtr node);
void check_fn_call(TData* global_frame,TData* function_frame,TData* current_frame,NodePtr node);
void check_declaration_stmt(TData* global_frame,TData* function_frame,TData* current_frame,NodePtr node);
void check_body(TData* global_frame,TData* current_frame,TData* function_frame,NodePtr node);
void check_redefinition_of_variable(TData* current_frame,TData* frame, char* var_name);
void check_redefinition_of_variable_inner(TData* in_frame,TData* function_frame, char* var_name);


void check_main_fn_is_correct(SymTable* sym_table);
void add_build_in_functions(TData* global_frame);
bool is_build_in_fn(char* name);

#endif //IFJ_PROJEKT_SEMANTIC_ANALYSIS_H
