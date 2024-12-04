/**
 * @file semantic_analysis.c
 * @brief Semantic Analysis for the IFJ project.
 *
 * @author Marko Poľanský (xpolan12)
 *
 * @date 2024
 */

#include "semantic_analysis.h"
#include <math.h>

FList* frame_list; // A list for managing frames during semantic analysis.
FList* variable_list;  // A list for checking of mutation and usage of variables during semantic analysis.
SymTable *sym_table; // The symbol table used for managing the symbol scope.
NodePtr root_node; // The root node of the abstract syntax tree (AST).


/**
 * bool_expression_convention[7][7]
 * This matrix defines whether two data types can be compared in a boolean context (e.g., relational operations).
 * The values are true or false, indicating whether the comparison between those types is valid in boolean expressions.
 */
bool bool_expression_convention[7][7] = {
        // INT    INT_CONV  INT_NULL FLOAT   FLOAT_CONV FLOAT_NULL INT_FLOAT_CONV
        { true,  true,  false, false, true,  false, true },    // DATA_TYPE_INT
        { true,  true,  false, true,  true,  false, true },    // DATA_TYPE_INT_CONVERTABLE
        { false, false, false, false, false, false, false },    // DATA_TYPE_INT_NULLABLE
        { false, true,  false, true,  true,  false, true },    // DATA_TYPE_FLOAT
        { true,  true,  false, true,  true,  false, true },        // DATA_TYPE_FLOAT_CONVERTABLE
        { false, false, false, false, false, false, false },        // DATA_TYPE_FLOAT_NULLABLE
        { true,  true,  false, true,  true,  false, true }        // DATA_TYPE_INT_FLOAT_CONVERTABLE
};

/**
 * expression_convention[7][7]
 * This matrix defines the result of applying type conversions between two types in an expression.
 * Each row represents a source data type, and each column represents a target data type.
 * The result can be another data type (if the conversion is valid) or -1 (if the conversion is invalid).
 */
DataTypeVariable expression_convention[7][7] = {

        {
                DATA_TYPE_INT, DATA_TYPE_INT,DATA_TYPE_INT_NULLABLE,-1,DATA_TYPE_INT,-1,DATA_TYPE_INT},
        {
                DATA_TYPE_INT,DATA_TYPE_INT_CONVERTABLE,DATA_TYPE_INT_NULLABLE,DATA_TYPE_FLOAT,DATA_TYPE_INT_FLOAT_CONVERTABLE,
                                                                                     DATA_TYPE_FLOAT_NULLABLE,DATA_TYPE_INT_CONVERTABLE},
        {
                DATA_TYPE_INT_NULLABLE,DATA_TYPE_INT_NULLABLE,DATA_TYPE_INT_NULLABLE,-1,DATA_TYPE_INT_NULLABLE,-1,DATA_TYPE_INT_NULLABLE},
        {
                -1,DATA_TYPE_FLOAT,-1,DATA_TYPE_FLOAT,DATA_TYPE_FLOAT,DATA_TYPE_FLOAT_NULLABLE,DATA_TYPE_FLOAT},
        {
                DATA_TYPE_INT,DATA_TYPE_INT_FLOAT_CONVERTABLE,DATA_TYPE_INT_NULLABLE,DATA_TYPE_FLOAT,DATA_TYPE_FLOAT_CONVERTABLE,
                DATA_TYPE_FLOAT_NULLABLE,DATA_TYPE_FLOAT_CONVERTABLE},
        {
                -1,DATA_TYPE_FLOAT_NULLABLE,-1,DATA_TYPE_FLOAT_NULLABLE,DATA_TYPE_FLOAT_NULLABLE,DATA_TYPE_FLOAT_NULLABLE,DATA_TYPE_FLOAT_NULLABLE},
        {
                DATA_TYPE_INT,DATA_TYPE_INT_CONVERTABLE,DATA_TYPE_INT_NULLABLE,DATA_TYPE_FLOAT,DATA_TYPE_FLOAT_CONVERTABLE,DATA_TYPE_FLOAT_NULLABLE,
                DATA_TYPE_INT_FLOAT_CONVERTABLE}
};

/**
 * variable_change_convention[10][10]
 * This matrix defines how data types can be converted between each other.
 * It checks whether a variable's type can be changed to another type (e.g., from an integer to a float, etc.).
 * The result is either a new valid data type or an error code (ERR).
 */
DataTypeVariable variable_change_convention[10][10] = {
        // DATA_TYPE_INT
        {DATA_TYPE_INT, DATA_TYPE_INT_CONVERTABLE, ERR, ERR, DATA_TYPE_INT_CONVERTABLE, ERR, DATA_TYPE_INT_CONVERTABLE, ERR, ERR, ERR},
        // DATA_TYPE_INT_CONVERTABLE
        {DATA_TYPE_INT, DATA_TYPE_INT_CONVERTABLE, ERR, ERR, DATA_TYPE_INT_CONVERTABLE, ERR, DATA_TYPE_INT_CONVERTABLE, ERR, ERR, ERR},
        // DATA_TYPE_INT_NULLABLE
        {DATA_TYPE_INT_NULLABLE, DATA_TYPE_INT_NULLABLE, DATA_TYPE_INT_NULLABLE, ERR, DATA_TYPE_INT_NULLABLE, ERR, DATA_TYPE_INT_NULLABLE, ERR, ERR, DATA_TYPE_INT_NULLABLE},
        // DATA_TYPE_FLOAT
        {ERR, DATA_TYPE_FLOAT_CONVERTABLE, ERR, DATA_TYPE_FLOAT, DATA_TYPE_FLOAT_CONVERTABLE, ERR, DATA_TYPE_FLOAT_CONVERTABLE, ERR, ERR, ERR},
        // DATA_TYPE_FLOAT_CONVERTABLE
        {ERR, DATA_TYPE_FLOAT_CONVERTABLE, ERR, DATA_TYPE_FLOAT, DATA_TYPE_FLOAT_CONVERTABLE, ERR, DATA_TYPE_FLOAT_CONVERTABLE, ERR, ERR, ERR},
        // DATA_TYPE_FLOAT_NULLABLE
        {ERR, DATA_TYPE_FLOAT_NULLABLE, ERR, DATA_TYPE_FLOAT_NULLABLE, DATA_TYPE_FLOAT_NULLABLE, DATA_TYPE_FLOAT_NULLABLE, ERR, ERR, ERR, DATA_TYPE_FLOAT_NULLABLE},
        // DATA_TYPE_INT_FLOAT_CONVERTABLE
        {DATA_TYPE_INT_FLOAT_CONVERTABLE, DATA_TYPE_INT_FLOAT_CONVERTABLE, ERR, DATA_TYPE_INT_FLOAT_CONVERTABLE, DATA_TYPE_INT_FLOAT_CONVERTABLE, ERR, DATA_TYPE_INT_FLOAT_CONVERTABLE, ERR, ERR, ERR},
        // DATA_TYPE_STRING
        {ERR, ERR, ERR, ERR, ERR, ERR, ERR, DATA_TYPE_STRING, ERR, ERR},
        // DATA_TYPE_STRING_NULLABLE
        {ERR, ERR, ERR, ERR, ERR, ERR, ERR, DATA_TYPE_STRING_NULLABLE, DATA_TYPE_STRING_NULLABLE, DATA_TYPE_STRING_NULLABLE},
        // DATA_TYPE_NULL
        {ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR}
};

void semantic_parse(NodePtr node){
    root_node = node;
    sym_table = init_sym_table();
    sym_table_create_insert_global_frame(sym_table);

    first_pass(node);
    second_pass(node);

    free_sym_table(sym_table);

}

void second_pass(NodePtr node){
    check_main_fn_is_correct();

    NodePtr new_command = node->left;
    if(new_command == NULL)
        return;


    TData * current_frame =sym_table->global_frame;
    NodePtr function_node = new_command->right;
    frame_list = frame_list_create();
    while (function_node != NULL){
        char* fn_name = function_node->data.string_val;
        TData* fn_frame = sym_table_search(current_frame,fn_name);
        fn_frame->number_of_inner_frames = 0;
        frame_list_add(frame_list,fn_frame);

        check_body(current_frame,fn_frame,fn_frame,function_node->right);

        if(!fn_frame->function->contains_return && fn_frame->function->return_type != RETURN_TYPE_VOID)
            semantic_error(6);

        Fn_Params* params = fn_frame->function->params;
        while (params != NULL){
            if(params->is_used == false){
                semantic_error(9);
            }
            params = params->next;
        }

        new_command = new_command->left;
        function_node = new_command->right;
        frame_delete_all(frame_list);
    }


    //not used variables
    frame_first(variable_list);
    while (variable_list->current != NULL) {
        TData* variable = frame_list_get(variable_list)->frame;
        if(variable->is_used == false){
            semantic_error(9);
        }
        if(variable->variable->variable_type == VAR && variable->is_mutated == false){
            semantic_error(9);
            fprintf(stderr,"Warning: Variable %s, is not mutated\n", variable->key);
        }
        frame_list_next(variable_list);
    }
    frame_free(variable_list);
    frame_free(frame_list);

}

void check_body(TData* global_frame,TData* current_frame,TData* function_frame,NodePtr node){
    if(node == NULL || node->right == NULL || current_frame == NULL || function_frame == NULL)
        return;

    switch (node->right->keyword) {
        case T_EQUALSIGN:
            check_declaration_stmt(global_frame,function_frame,current_frame,node->right);
            break;
        case FN_CALL:
            check_fn_call(global_frame,function_frame,current_frame,node->right);
            break;
        case T_IFJ:{
            char* new_str = malloc(strlen("ifj.") + strlen(node->right->left->data.string_val) + 1);
            strcpy(new_str, "ifj.");
            strcat(new_str, node->right->left->data.string_val);
            node->right->left->data.string_val = new_str;
            check_fn_call(global_frame,function_frame,current_frame,node->right);
            break;
        }

        case T_IF:{
            char scope[SCOPE_IDENTIFIER_SIZE];
            memcpy(scope,current_frame->scope,SCOPE_IDENTIFIER_SIZE);
            scope[strlen(scope)] = scope[strlen(scope)]+ current_frame->number_of_inner_frames + 1;
            TData* inner_frame = sym_table_search(current_frame, scope);
            current_frame->number_of_inner_frames = current_frame->number_of_inner_frames + 1;
            char* id_no_null;
            frame_list_add(frame_list,inner_frame);

            // expression with null
            if(node->right->left->right != NULL){
                DataTypeVariable data_type_of_expression = check_expression(global_frame,function_frame,current_frame,node->right->left->left);
                if(data_type_of_expression == DATA_TYPE_BOOL || !is_data_type_nullable(data_type_of_expression)){
                    semantic_error(7);
                }
                data_type_of_expression = get_not_null_data_type_from_nullable(data_type_of_expression);
                id_no_null = node->right->left->right->data.string_val;
                TData* variable = sym_table_search(inner_frame,id_no_null);
                variable->variable->data_type = data_type_of_expression;
            } else{
                // bool expression
                DataTypeVariable data_type_of_expression = check_expression(global_frame,function_frame,current_frame,node->right->left->left);
                if(data_type_of_expression != DATA_TYPE_BOOL){
                    semantic_error(7);
                }
            }

            check_body(global_frame,inner_frame,function_frame,node->right->right);
            break;
        }
        case T_WHILE:{

            char scope[SCOPE_IDENTIFIER_SIZE];
            memcpy(scope,current_frame->scope,SCOPE_IDENTIFIER_SIZE);
            scope[strlen(scope)] = scope[strlen(scope)]+ current_frame->number_of_inner_frames + 1;
            TData* inner_frame = sym_table_search(current_frame, scope);
            current_frame->number_of_inner_frames = current_frame->number_of_inner_frames + 1;
            char* id_no_null;
            frame_list_add(frame_list,inner_frame);

            // expression with null
            if(node->right->left->right != NULL){
                DataTypeVariable data_type_of_expression = check_expression(global_frame,function_frame,current_frame,node->right->left->left);
                if(data_type_of_expression == DATA_TYPE_BOOL || !is_data_type_nullable(data_type_of_expression)){
                    semantic_error(7);
                }
                data_type_of_expression = get_not_null_data_type_from_nullable(data_type_of_expression);
                id_no_null = node->right->left->right->data.string_val;
                TData* variable = sym_table_search(inner_frame,id_no_null);
                variable->variable->data_type = data_type_of_expression;

            } else{
                // bool expression
                DataTypeVariable data_type_of_expression = check_expression(global_frame,function_frame,current_frame,node->right->left->left);
                if(data_type_of_expression != DATA_TYPE_BOOL){
                    semantic_error(7);
                }

            }

            check_body(global_frame,inner_frame,function_frame,node->right->right);
            break;
        }
        case T_ELSE:{
            char scope[SCOPE_IDENTIFIER_SIZE];
            memcpy(scope,current_frame->scope,SCOPE_IDENTIFIER_SIZE);
            scope[strlen(scope)] = scope[strlen(scope)] + current_frame->number_of_inner_frames + 1;
            current_frame->number_of_inner_frames = current_frame->number_of_inner_frames + 1;
            TData* inner_frame = sym_table_search(current_frame, scope);
            frame_list_add(frame_list,inner_frame);
            check_body(global_frame,inner_frame,function_frame,node->right->right);
            break;
        }
        case T_RETURN:{
            DataTypeVariable return_data_type = DATA_TYPE_NONE;
            if(node->right->right != NULL){
                if(node->right->right->keyword == T_STRING){
                    semantic_error(7);
                }
                return_data_type = check_expression(global_frame,function_frame,current_frame,node->right->right);
            }

            if(function_frame->function->return_type == RETURN_TYPE_VOID){
                if(return_data_type != DATA_TYPE_NONE)
                    semantic_error(6); // returning value in void fn
                return;
            } else if(return_data_type == DATA_TYPE_NONE){
                semantic_error(6); // return with no value
            }
            if(!is_returned_value_correct(function_frame->function->return_type,return_data_type)){
                semantic_error(4);
            }
            function_frame->function->contains_return = true;

        }
        default:{
            break;
        }
    }
    check_body(global_frame,current_frame,function_frame,node->left);
}

DataTypeVariable check_expression(TData* global_frame,TData* function_frame,TData* current_Frame,NodePtr node){
    switch (node->keyword) {
        case T_INT:
            return DATA_TYPE_INT_CONVERTABLE;
        case T_FLOAT:
            if (node->data.float_val == floor(node->data.float_val))
                return DATA_TYPE_FLOAT_CONVERTABLE;
            else
                return DATA_TYPE_FLOAT;
        case T_STRING:
            return DATA_TYPE_STRING;
        case T_NULL:
            return DATA_TYPE_NULL;
        case T_UNDEFINED:
            return DATA_TYPE_NONE;


        case FN_PARAM:
            return check_expression(global_frame,function_frame,current_Frame,node->right);

        case T_ID: {
            frame_first(frame_list);
            while (frame_list->current != NULL) {
                TData *previous_frame = frame_list_get(frame_list)->frame;
                frame_list_next(frame_list);

                if (strncmp(current_Frame->scope,previous_frame->scope,  strlen(previous_frame->scope)) != 0 ||
                    strlen(previous_frame->scope) > strlen(current_Frame->scope))
                    continue;

                TData* variable = sym_table_search(previous_frame, node->data.string_val);

                if (variable != NULL) {
                    variable->is_used = true;
                    return variable->variable->data_type;
                }
            }
            Fn_Params* param = function_frame->function->params;
            while (param != NULL){
                if(strcmp(param->name,node->data.string_val) == 0){
                    param->is_used = true;
                    return param->data_type;
                };
                param = param->next;
            }
            semantic_error(3);
            break;
        }
        case T_PLUS:
        case T_MINUS:
        case T_ASTERISK:{
            DataTypeVariable left_type = DATA_TYPE_NONE;
            DataTypeVariable right_type = DATA_TYPE_NONE;
            if (node->left != NULL) {
                left_type = check_expression(global_frame, function_frame, current_Frame, node->left);
            }
            if (node->right != NULL) {
                right_type = check_expression(global_frame, function_frame, current_Frame, node->right);
            }
            DataTypeVariable data_type_var = compare_data_type_variables(left_type, right_type);
            if(is_data_type_nullable(data_type_var)){
                semantic_error(7);
                break;
            } else
                return data_type_var;

        }
        case T_SLASH: {
            DataTypeVariable left_type = DATA_TYPE_NONE;
            DataTypeVariable right_type = DATA_TYPE_NONE;
            if (node->left != NULL) {
                left_type = check_expression(global_frame, function_frame, current_Frame, node->left);
            }
            if (node->right != NULL) {
                right_type = check_expression(global_frame, function_frame, current_Frame, node->right);
            }
            DataTypeVariable data_type_var = compare_data_type_variables(left_type, right_type);
            if(is_data_type_nullable(data_type_var)){
                semantic_error(7);
                break;
            } else
                return data_type_var;

        }

        case T_EQUALS:
        case T_NOTEQUAL:{
            DataTypeVariable left_type = DATA_TYPE_NONE;
            DataTypeVariable right_type= DATA_TYPE_NONE;
            if(node->left != NULL){
                left_type = check_expression(global_frame,function_frame,current_Frame,node->left);
            }
            if(node->right != NULL){
                right_type =  check_expression(global_frame,function_frame,current_Frame,node->right);
            }
            if(left_type == DATA_TYPE_NULL && right_type == DATA_TYPE_NULL){
                return DATA_TYPE_BOOL;
            }

            if((left_type == DATA_TYPE_NULL && is_data_type_nullable(right_type)) ||
                    (right_type == DATA_TYPE_NULL && is_data_type_nullable(left_type)) ){
                return DATA_TYPE_BOOL;
            }

            if(!is_bool_expression_ok(left_type,right_type)){
                semantic_error(7);
                break;
            } else
                return DATA_TYPE_BOOL;
        }

        case T_GREATER:
        case T_GREATEREQUAL:
        case T_LESS:
        case T_LESSEQUAL:{
            DataTypeVariable left_type = DATA_TYPE_NONE;
            DataTypeVariable right_type= DATA_TYPE_NONE;
            if(node->left != NULL){
                left_type = check_expression(global_frame,function_frame,current_Frame,node->left);
            }
            if(node->right != NULL){
                right_type =  check_expression(global_frame,function_frame,current_Frame,node->right);
            }
            if(is_data_type_nullable(left_type) || is_data_type_nullable(right_type)){
                semantic_error(7);
                break;
            }
            if(!is_bool_expression_ok(left_type,right_type)){
                semantic_error(7);
                break;
            } else
                return DATA_TYPE_BOOL;
        }


        case FN_CALL:{
            TData* fn = sym_table_search(global_frame,node->data.string_val);
            if(fn == NULL){
                semantic_error(3);
                break;
            }
            //only non void can be called without assignment
            if(fn->function->return_type == RETURN_TYPE_VOID){
                semantic_error(7);
                break;
            }
            Fn_Params* param = fn->function->params;
            Fn_Params* reversed_params[fn->function->params_count];
            for (int i = fn->function->params_count-1; i >=0 ; --i) {
                reversed_params[i] = param;
                param = param->next;
            }
            NodePtr param_node = node->left;
            for (int i = 0; i < fn->function->params_count; i++) {
                //fewer arguments than provided
                if(param_node == NULL){
                    semantic_error(4);
                    break;
                }

                DataTypeVariable param_type = check_expression(global_frame,function_frame,current_Frame,param_node);
                compare_variable_expression(reversed_params[i]->data_type,param_type);
                param_node = param_node->left;
            }
            //more arguments provided
            if(param_node != NULL){
                semantic_error(4);
                break;
            }
            return map_ReturnType_to_DataTypeVariable(fn->function->return_type);
        }
        case T_IFJ:{
            char ifj_fn_name[100] = "ifj.";
            strcat(ifj_fn_name,node->left->data.string_val);
            TData* fn = sym_table_search(global_frame,ifj_fn_name);
            if(fn == NULL){
                semantic_error(3);
                break;
            }
            //only non void can be called without assignment
            if(fn->function->return_type == RETURN_TYPE_VOID){
                semantic_error(7);
                break;
            }
            Fn_Params* param = fn->function->params;
            Fn_Params* reversed_params[fn->function->params_count];
            for (int i = fn->function->params_count-1; i >=0 ; --i) {
                reversed_params[i] = param;
                param = param->next;
            }
            NodePtr param_node = node->right;
            for (int i = 0; i < fn->function->params_count; i++) {
                //fewer arguments than provided
                if(param_node == NULL){
                    semantic_error(4);
                    break;
                }

                DataTypeVariable param_type = check_expression(global_frame,function_frame,current_Frame,param_node);
                compare_variable_expression(reversed_params[i]->data_type,param_type);
                param_node = param_node->left;
            }
            //more arguments provided
            if(param_node != NULL){
                semantic_error(4);
                break;
            }
            return map_ReturnType_to_DataTypeVariable(fn->function->return_type);
        }

        default:
            return DATA_TYPE_NONE;
    }
    return DATA_TYPE_NONE;
}

DataTypeVariable get_not_null_data_type_from_nullable(DataTypeVariable data_type_of_bool_expression){
    switch (data_type_of_bool_expression) {
        case DATA_TYPE_INT:
            return DATA_TYPE_INT;
        case DATA_TYPE_INT_CONVERTABLE:
            return DATA_TYPE_INT_CONVERTABLE;
        case DATA_TYPE_INT_NULLABLE:
            return DATA_TYPE_INT;
        case DATA_TYPE_FLOAT:
            return DATA_TYPE_FLOAT;
        case DATA_TYPE_FLOAT_CONVERTABLE:
            return DATA_TYPE_FLOAT_CONVERTABLE;
        case DATA_TYPE_FLOAT_NULLABLE:
            return DATA_TYPE_FLOAT;
        case DATA_TYPE_INT_FLOAT_CONVERTABLE:
            return DATA_TYPE_INT_FLOAT_CONVERTABLE;
        case DATA_TYPE_NONE:
            break;
        case DATA_TYPE_STRING:
            return DATA_TYPE_STRING;
        case DATA_TYPE_STRING_NULLABLE:
            return DATA_TYPE_STRING;
        case DATA_TYPE_BOOL:
            break;
        case DATA_TYPE_NULL:
            break;
        case ERR:
            break;
    }
    return ERR;
}

bool is_returned_value_correct(ReturnTypes fn_expected, DataTypeVariable returned){
    switch (fn_expected) {
        case RETURN_TYPE_INT:
            return returned == DATA_TYPE_INT ||
                   returned == DATA_TYPE_INT_CONVERTABLE ||
                   returned == DATA_TYPE_INT_FLOAT_CONVERTABLE ||
                   returned == DATA_TYPE_FLOAT_CONVERTABLE;

        case RETURN_TYPE_FLOAT:
            return returned == DATA_TYPE_FLOAT ||
                   returned == DATA_TYPE_FLOAT_CONVERTABLE ||
                   returned == DATA_TYPE_INT_CONVERTABLE ||
                   returned == DATA_TYPE_INT_FLOAT_CONVERTABLE;

        case RETURN_TYPE_STRING:
            return returned == DATA_TYPE_STRING;

        case RETURN_TYPE_STRING_NULLABLE:
            return returned == DATA_TYPE_STRING ||
                   returned == DATA_TYPE_STRING_NULLABLE;

        case RETURN_TYPE_INT_NULLABLE:
            return returned == DATA_TYPE_NULL ||
                   returned == DATA_TYPE_INT ||
                   returned == DATA_TYPE_INT_CONVERTABLE ||
                   returned == DATA_TYPE_INT_FLOAT_CONVERTABLE ||
                   returned == DATA_TYPE_FLOAT_CONVERTABLE ||
                   returned == DATA_TYPE_INT_NULLABLE;

        case RETURN_TYPE_FLOAT_NULLABLE:
            return returned == DATA_TYPE_NULL ||
                   returned == DATA_TYPE_FLOAT ||
                   returned == DATA_TYPE_FLOAT_CONVERTABLE ||
                   returned == DATA_TYPE_INT_CONVERTABLE ||
                   returned == DATA_TYPE_INT_FLOAT_CONVERTABLE ||
                   returned == DATA_TYPE_FLOAT_NULLABLE;

        default:
            return false;
    }
}

bool is_bool_expression_ok(DataTypeVariable left_type, DataTypeVariable right_type){

    if(left_type == DATA_TYPE_STRING || right_type == DATA_TYPE_STRING){
        semantic_error(7);
    }
    if(left_type == DATA_TYPE_STRING_NULLABLE || right_type == DATA_TYPE_STRING_NULLABLE){
        semantic_error(7);
    }

    return bool_expression_convention[left_type][right_type];
}

DataTypeVariable compare_variable_expression(DataTypeVariable left_type, DataTypeVariable right_type){
    DataTypeVariable new_type = variable_change_convention[left_type][right_type];
    if(new_type == ERR){
        semantic_error(7);
        return DATA_TYPE_NONE;
    }
    else
        return new_type;
}


DataTypeVariable compare_data_type_variables(DataTypeVariable left_type, DataTypeVariable right_type){
    // operation are forbidden in strings
    if(left_type == DATA_TYPE_STRING || right_type == DATA_TYPE_STRING){
        semantic_error(7);
    }
    if(left_type == DATA_TYPE_STRING_NULLABLE || right_type == DATA_TYPE_STRING_NULLABLE){
        semantic_error(7);
    }
    if(left_type == DATA_TYPE_NONE){
        return right_type;
    }
    if (right_type == DATA_TYPE_NONE) {
        return left_type;
    }
    DataTypeVariable new_type = expression_convention[left_type][right_type];
    if(new_type == ERR){
        semantic_error(7);
        return DATA_TYPE_NONE;
    }

    else
        return new_type;
}



void check_fn_call(TData* global_frame,TData* function_frame,TData* current_frame,NodePtr node){
    TData* fn = NULL;

    if( node->left != NULL && node->left->data.string_val != NULL){
         fn = sym_table_search(global_frame,node->left->data.string_val);
        if(strcmp(node->left->data.string_val,"ifj.write")==0){
            NodePtr param_node = node->right;
            if(param_node == NULL)
                semantic_error(4);

            DataTypeVariable param_type = check_expression(global_frame,function_frame,current_frame,param_node->right);
            param_node = param_node->left;

            if(param_type == DATA_TYPE_NONE || param_type == DATA_TYPE_BOOL){
                semantic_error(7);
            }

            if(param_node != NULL){
                semantic_error(4);
            }
            return;
        }
    }else if (node->data.string_val != NULL){
         fn = sym_table_search(global_frame,node->data.string_val);
    }

    if(fn == NULL){
        semantic_error(3);
    }
    //only  void can be called without assignment
    if(fn->function->return_type != RETURN_TYPE_VOID){
        semantic_error(4);
    }



    Fn_Params* param = fn->function->params;
    Fn_Params* reversed_params[fn->function->params_count];
    for (int i = fn->function->params_count-1; i >=0 ; --i) {
        reversed_params[i] = param;
        param = param->next;
    }
    NodePtr param_node = node->left;
    for (int i = 0; i < fn->function->params_count; i++) {
        //fewer arguments than provided
        if(param_node == NULL)
            semantic_error(4);
        DataTypeVariable param_type = check_expression(global_frame,function_frame,current_frame,param_node);
        compare_variable_expression(reversed_params[i]->data_type,param_type);
        param_node = param_node->left;
    }
    //more arguments provided
    if(param_node != NULL){
        semantic_error(4);
    }
}


void check_declaration_stmt(TData* global_frame,TData* function_frame,TData* current_frame,NodePtr node) {
    Variable_Type variable_type = map_tType_to_Variable_Type(node->left->keyword);
    char *name = node->left->data.string_val;

    if(strcmp(node->left->data.string_val,"_") == 0 ){
        check_expression(global_frame, function_frame,current_frame, node->right);
        return;
    }
    TData *variable = NULL;
    DataTypeVariable data_type_of_variable;
    if (variable_type == NONE) {
        variable = check_assigment_only_to_var_is_accepted(name,current_frame,function_frame);
    }
    if (variable == NULL) {
        frame_first(frame_list);
        while (frame_list->current != NULL) {
            TData *previous_frame = frame_list_get(frame_list)->frame;
            frame_list_next(frame_list);

            if (strncmp(current_frame->scope,previous_frame->scope,  strlen(previous_frame->scope)) != 0 ||
                strlen(previous_frame->scope) > strlen(current_frame->scope))
                continue;

            variable = sym_table_search(previous_frame, name);

            if (variable != NULL) {
                data_type_of_variable = variable->variable->data_type;
                break;
            }
        }
    } else{
        variable->is_used = true;
        data_type_of_variable = variable->variable->data_type;
        if(variable->variable->variable_type == VAR){
            variable->is_mutated = true;
        }
    }


    if(node->right->keyword == T_STRING && variable_type == NONE)
        semantic_error(7);

    if(node->right->keyword == T_STRING && variable_type != NONE)
        semantic_error(8);


    DataTypeVariable expression_data_type = check_expression(global_frame, function_frame,current_frame, node->right);

    if (is_data_type_nullable(data_type_of_variable) == false && expression_data_type == DATA_TYPE_NULL) {
        semantic_error(8); //TODO kedy nastava
    }

    if(data_type_of_variable == DATA_TYPE_NONE){ // var a = 3
        if (expression_data_type == DATA_TYPE_INT_FLOAT_CONVERTABLE)
            variable->variable->data_type = DATA_TYPE_FLOAT_CONVERTABLE;
        else
            variable->variable->data_type = expression_data_type;
    } else{
        variable->variable->data_type =   compare_variable_expression(data_type_of_variable,expression_data_type);
    }

}



bool is_data_type_nullable(DataTypeVariable data_type){
    switch (data_type) {
        case DATA_TYPE_INT_NULLABLE:
        case DATA_TYPE_FLOAT_NULLABLE:
        case DATA_TYPE_STRING_NULLABLE:
            return true;
        default:
            return false;
    }

}

TData* check_assigment_only_to_var_is_accepted(char* name, TData* in_frame, TData* function_frame){
    TData* variable = NULL;
    frame_first(frame_list);
    while (frame_list->current != NULL){
        TData * previous_frame = frame_list_get(frame_list)->frame;
        frame_list_next(frame_list);

        if (strncmp(in_frame->scope,previous_frame->scope,  strlen(previous_frame->scope)) != 0 ||
            strlen(previous_frame->scope) > strlen(in_frame->scope))
            continue;

        variable = sym_table_search(previous_frame,name);
        // assignment only to var is accepted
        if( variable != NULL ){
            break;
        }
    }
    if(variable == NULL){
        Fn_Params *param = function_frame->function->params;
        while (param != NULL) {
            if (strcmp(param->name, name) == 0) {
                semantic_error(5);
            }
            param = param->next;
        }
        semantic_error(3);
    } else if(variable->variable->variable_type != VAR){
        semantic_error(5);
    }

    return variable;
}
void check_main_fn_is_correct(){
    TData* main_fn = sym_table_search(sym_table->global_frame,"main");
    if(main_fn == NULL)
        semantic_error(3);

    if (main_fn->function->return_type != RETURN_TYPE_VOID)
        semantic_error(4);

    if (main_fn->function->params != NULL)
        semantic_error(4);

}

void first_pass(NodePtr node){
    NodePtr new_command = node->left;
    if(new_command == NULL)
        return;

    frame_list = frame_list_create();
    variable_list = frame_list_create();

    TData * current_frame =sym_table->global_frame;

    add_build_in_functions(current_frame);

    NodePtr function_node = new_command->right;

    char scope[SCOPE_IDENTIFIER_SIZE] = {'\001',0,};

    while (function_node != NULL){
        char* fn_name = function_node->data.string_val;

        if(sym_table_search(current_frame,fn_name) != NULL){
            semantic_error(5);
        };
        TData* fn_Frame =sym_table_create_data(fn_name,scope,FUNCTION_FRAME);
        sym_table_insert(current_frame,fn_Frame);

        ReturnTypes return_type = parse_function_return_value(function_node->left->right);
        fn_Frame->function = create_tdata_fn(return_type);

        parse_function_params(fn_Frame->function,function_node->left->left);
        frame_list_add(frame_list,fn_Frame);
        parse_body(fn_Frame,fn_Frame,function_node->right);
        new_command = new_command->left;
        function_node = new_command->right;
        scope[0] = scope[0] + 1;

        frame_first(frame_list);
        while (frame_list->current!= NULL){
            frame_list->current->frame->number_of_inner_frames = 0;
            frame_list_next(frame_list);
        }
        frame_delete_all(frame_list);
    }


}
void parse_body(TData* function_frame,TData* current_frame,NodePtr node){
    if(node == NULL || node->right == NULL){
        return;
    }
    switch (node->right->keyword) {
        case T_EQUALSIGN:
            parse_declaration_stmt(current_frame,function_frame,node->right);
            break;

        case T_WHILE:{
            char scope[SCOPE_IDENTIFIER_SIZE];
            memcpy(scope,current_frame->scope,SCOPE_IDENTIFIER_SIZE);
            scope[strlen(scope)] = scope[strlen(scope)]+ current_frame->number_of_inner_frames + 1;
            TData* inner_frame = sym_table_create_data(scope, scope, INNER_FRAME);
            frame_list_add(frame_list,inner_frame);
            sym_table_insert(current_frame,inner_frame);
            char* id_no_null;

            // expression with null
            if(node->right->left->right != NULL){
                id_no_null = node->right->left->right->data.string_val;
                check_redefinition_of_variable(inner_frame,function_frame,id_no_null);
                TData *variable_Frame = sym_table_create_data(id_no_null, scope, VARIABLE_FRAME);
                TData_Variable *var = create_tdata_variable(DATA_TYPE_NONE, CONST);
                variable_Frame->variable = var;
                sym_table_insert(inner_frame,variable_Frame);
                frame_list_add(variable_list,variable_Frame);
            }

            parse_body(function_frame,inner_frame,node->right->right);
            break;
        }
        case T_IF:{
            char scope[SCOPE_IDENTIFIER_SIZE];
            memcpy(scope,current_frame->scope,SCOPE_IDENTIFIER_SIZE);
            scope[strlen(scope)] = scope[strlen(scope)]+ current_frame->number_of_inner_frames + 1;
            TData* inner_frame = sym_table_create_data(scope, scope, INNER_FRAME);
            frame_list_add(frame_list,inner_frame);
            sym_table_insert(current_frame,inner_frame);
            char* id_no_null;

            // expression with null
            if(node->right->left->right != NULL){
                id_no_null = node->right->left->right->data.string_val;
                check_redefinition_of_variable(inner_frame,function_frame,id_no_null);
                TData *variable_Frame = sym_table_create_data(id_no_null, scope, VARIABLE_FRAME);
                TData_Variable *var = create_tdata_variable(DATA_TYPE_NONE, CONST);
                variable_Frame->variable = var;
                sym_table_insert(inner_frame,variable_Frame);
                frame_list_add(variable_list,variable_Frame);
            }
            parse_body(function_frame,inner_frame,node->right->right);
            parse_body(function_frame,current_frame,node->left->right);
            break;
        }
        case T_ELSE:{
            char scope[SCOPE_IDENTIFIER_SIZE];
            memcpy(scope,current_frame->scope,SCOPE_IDENTIFIER_SIZE);
            scope[strlen(scope)] = scope[strlen(scope)]+ current_frame->number_of_inner_frames + 1;
            TData* inner_frame = sym_table_create_data(scope, scope, INNER_FRAME);

            sym_table_insert(current_frame,inner_frame);
            frame_list_add(frame_list,inner_frame);

            parse_body(function_frame,inner_frame,node->right->right);
            break;
        }
        default:
            break;

    }
    parse_body(function_frame,current_frame,node->left);
}



void check_redefinition_of_variable(TData* in_frame,TData* function_frame, char* var_name){
    if(in_frame == NULL || frame_list == NULL)
        return;

    frame_first(frame_list);
    while (frame_list->current != NULL){
        TData * current_frame = frame_list_get(frame_list)->frame;
        frame_list_next(frame_list);

        if (strncmp(in_frame->scope,current_frame->scope,  strlen(current_frame->scope)) != 0 ||
            strlen(current_frame->scope) > strlen(in_frame->scope))
            continue;

        if(sym_table_search(current_frame,var_name) != NULL)
            semantic_error(5);

    }
    //redefinition of function parameter
    Fn_Params* fn_param = function_frame->function->params;
    while (fn_param != NULL){
        if(strcmp(fn_param->name,var_name)== 0){
            semantic_error(5);
        }
        fn_param = fn_param->next;
    }
}
void parse_declaration_stmt(TData* frame,TData* function_frame,NodePtr node) {
    Variable_Type variable_type = map_tType_to_Variable_Type(node->left->keyword);
    // in first pass handling only explicitly declared variables
    if (variable_type == NONE)
        return;

    DataTypeVariable data_type_variable;
    if(node->left->left != NULL){
        data_type_variable = map_node_to_DataTypeVariable(node->left->left);
    } else
        data_type_variable = DATA_TYPE_NONE;

    char *name = node->left->data.string_val;


    //redefinition of variable
    check_redefinition_of_variable(frame,function_frame,name);


    TData *variable_Frame = sym_table_create_data(name, frame->scope, VARIABLE_FRAME);
    TData_Variable *var = create_tdata_variable(data_type_variable, variable_type);
    variable_Frame->variable = var;
    frame_list_add(variable_list,variable_Frame);
    sym_table_insert(frame, variable_Frame);
}
void parse_function_params(TData_Fn* data_fn,NodePtr node ){
    while (node != NULL){
        DataTypeVariable data_type_variable = map_node_to_DataTypeVariable(node);
        Fn_Params * param = data_fn->params;
        while (param != NULL){
            if(strcmp(param->name,node->data.string_val) == 0){
                semantic_error(5);
            }
            param = param->next;
        }
        add_fn_param(data_fn,node->data.string_val,data_type_variable);
        node = node->right;
    }
}
ReturnTypes parse_function_return_value(NodePtr node){
    return map_node_to_ReturnTypes(node);
}

ReturnTypes map_node_to_ReturnTypes(NodePtr node){
    bool is_nullable = false;
    if(node->left != NULL)
        is_nullable = node->left->keyword == T_QUESTIONMARK;
    switch (node->keyword) {
        case T_I32:{
            return is_nullable? RETURN_TYPE_INT_NULLABLE : RETURN_TYPE_INT;
        }
        case T_F64:{
            return is_nullable? RETURN_TYPE_FLOAT_NULLABLE : RETURN_TYPE_FLOAT;
        }
        case T_U8:{
            return is_nullable? RETURN_TYPE_STRING_NULLABLE : RETURN_TYPE_STRING;
        }
        case T_VOID:{
            return RETURN_TYPE_VOID;
        }
        default:{
            return RETURN_TYPE_UNEXPECTED;
        }
    }
}

DataTypeVariable map_ReturnType_to_DataTypeVariable(ReturnTypes type){
    switch (type) {
        case RETURN_TYPE_INT:
            return DATA_TYPE_INT;
        case RETURN_TYPE_FLOAT:
            return DATA_TYPE_FLOAT;
        case RETURN_TYPE_STRING:
            return DATA_TYPE_STRING;
        case RETURN_TYPE_INT_NULLABLE:
            return DATA_TYPE_INT_NULLABLE;
        case RETURN_TYPE_FLOAT_NULLABLE:
            return DATA_TYPE_FLOAT_NULLABLE;
        case RETURN_TYPE_STRING_NULLABLE:
            return DATA_TYPE_STRING_NULLABLE;
        case RETURN_TYPE_VOID:
        case RETURN_TYPE_UNEXPECTED:
            return DATA_TYPE_NONE;
    }
    return DATA_TYPE_NONE;
}

DataTypeVariable map_node_to_DataTypeVariable(NodePtr node){
    bool is_nullable = false;
    if(node->left != NULL)
        is_nullable = node->left->keyword == T_QUESTIONMARK;
    switch (node->keyword) {
        case T_I32:{
            return is_nullable ? DATA_TYPE_INT_NULLABLE : DATA_TYPE_INT;
        }
        case T_F64:{
            return is_nullable ? DATA_TYPE_FLOAT_NULLABLE : DATA_TYPE_FLOAT;
        }
        case T_U8:{
            return is_nullable ? DATA_TYPE_STRING_NULLABLE: DATA_TYPE_STRING;
        }
        default:{
            return DATA_TYPE_NONE;
        }
    }
}

Variable_Type map_tType_to_Variable_Type(tType type){
    switch (type) {
        case T_CONST:{
            return CONST;
        }
        case T_VAR:{
            return VAR;
        }
        default:{
            return NONE;
        }
    }
}


void add_build_in_functions(TData* global_frame){
    char scope[SCOPE_IDENTIFIER_SIZE];
    memset(scope, 0, sizeof(global_frame->scope));
    global_frame->scope[0] = 1;

    TData* fn_Frame =sym_table_create_data( "ifj.readstr",scope,FUNCTION_FRAME);
    ReturnTypes return_type = RETURN_TYPE_STRING_NULLABLE;
    fn_Frame->function = create_tdata_fn(return_type);
    fn_Frame->function->params = NULL;
    fn_Frame->function->params_count = 0;
    sym_table_insert(global_frame,fn_Frame);

    fn_Frame =sym_table_create_data( "ifj.readi32",scope,FUNCTION_FRAME);
    return_type = RETURN_TYPE_INT_NULLABLE;
    fn_Frame->function = create_tdata_fn(return_type);
    fn_Frame->function->params = NULL;
    fn_Frame->function->params_count = 0;
    sym_table_insert(global_frame,fn_Frame);



    fn_Frame =sym_table_create_data( "ifj.readf64",scope,FUNCTION_FRAME);
    return_type = RETURN_TYPE_FLOAT_NULLABLE;
    fn_Frame->function = create_tdata_fn(return_type);
    fn_Frame->function->params = NULL;
    fn_Frame->function->params_count = 0;
    sym_table_insert(global_frame,fn_Frame);

    fn_Frame =sym_table_create_data( "ifj.i2f",scope,FUNCTION_FRAME);
    return_type = RETURN_TYPE_FLOAT;
    fn_Frame->function = create_tdata_fn(return_type);
    add_fn_param(fn_Frame->function,"term",DATA_TYPE_INT);
    sym_table_insert(global_frame,fn_Frame);

    fn_Frame =sym_table_create_data( "ifj.f2i",scope,FUNCTION_FRAME);
    return_type = RETURN_TYPE_INT;
    fn_Frame->function = create_tdata_fn(return_type);
    add_fn_param(fn_Frame->function,"term",DATA_TYPE_FLOAT);
    sym_table_insert(global_frame,fn_Frame);

    fn_Frame =sym_table_create_data( "ifj.string",scope,FUNCTION_FRAME);
    return_type = RETURN_TYPE_STRING;
    fn_Frame->function = create_tdata_fn(return_type);
    add_fn_param(fn_Frame->function,"term",DATA_TYPE_STRING);
    sym_table_insert(global_frame,fn_Frame);


    fn_Frame =sym_table_create_data( "ifj.length",scope,FUNCTION_FRAME);
    return_type = RETURN_TYPE_INT;
    fn_Frame->function = create_tdata_fn(return_type);
    add_fn_param(fn_Frame->function,"s",DATA_TYPE_STRING);
    sym_table_insert(global_frame,fn_Frame);


    fn_Frame =sym_table_create_data( "ifj.concat",scope,FUNCTION_FRAME);
    return_type = RETURN_TYPE_STRING;
    fn_Frame->function = create_tdata_fn(return_type);
    add_fn_param(fn_Frame->function,"s1",DATA_TYPE_STRING);
    add_fn_param(fn_Frame->function,"s2",DATA_TYPE_STRING);
    sym_table_insert(global_frame,fn_Frame);


    fn_Frame =sym_table_create_data( "ifj.substring",scope,FUNCTION_FRAME);
    return_type = RETURN_TYPE_STRING_NULLABLE;
    fn_Frame->function = create_tdata_fn(return_type);
    add_fn_param(fn_Frame->function,"s",DATA_TYPE_STRING);
    add_fn_param(fn_Frame->function,"i",DATA_TYPE_INT);
    add_fn_param(fn_Frame->function,"j",DATA_TYPE_INT);
    sym_table_insert(global_frame,fn_Frame);


    fn_Frame =sym_table_create_data( "ifj.strcmp",scope,FUNCTION_FRAME);
    return_type = RETURN_TYPE_INT;
    fn_Frame->function = create_tdata_fn(return_type);
    add_fn_param(fn_Frame->function,"s1",DATA_TYPE_STRING);
    add_fn_param(fn_Frame->function,"s2",DATA_TYPE_STRING);
    sym_table_insert(global_frame,fn_Frame);

    fn_Frame =sym_table_create_data( "ifj.ord",scope,FUNCTION_FRAME);
    return_type = RETURN_TYPE_INT;
    fn_Frame->function = create_tdata_fn(return_type);
    add_fn_param(fn_Frame->function,"s",DATA_TYPE_STRING);
    add_fn_param(fn_Frame->function,"i",DATA_TYPE_INT);
    sym_table_insert(global_frame,fn_Frame);

    fn_Frame =sym_table_create_data( "ifj.chr",scope,FUNCTION_FRAME);
    return_type = RETURN_TYPE_STRING;
    fn_Frame->function = create_tdata_fn(return_type);
    add_fn_param(fn_Frame->function,"i",DATA_TYPE_INT);
    sym_table_insert(global_frame,fn_Frame);

    fn_Frame =sym_table_create_data( "ifj.write",scope,FUNCTION_FRAME);
    return_type = RETURN_TYPE_VOID;
    fn_Frame->function = create_tdata_fn(return_type);
    add_fn_param(fn_Frame->function,"term",DATA_TYPE_NONE);
    sym_table_insert(global_frame,fn_Frame);
}




void semantic_error(int error){
    free_sym_table(sym_table);
    free_tree(root_node);
    exit(error);
}