#include "semantic_analysis.h"
#include <math.h>
FList* frame_list;


bool bool_expression_convention[7][7] = {
        // INT    INT_CONV  INT_NULL FLOAT   FLOAT_CONV FLOAT_NULL INT_FLOAT_CONV
        { true,   true,     false,   false,  true,      false,     true  }, // INT
        { true,   true,     false,   true,   true,      false,     true  }, // INT_CONVERTABLE
        { false,  false,    false,   false,  false,     false,     false }, // INT_NULLABLE
        { false,  true,     false,   true,   true,      false,     true  }, // FLOAT
        { true,   true,     false,   true,   true,      false,     true  }, // FLOAT_CONVERTABLE
        { false,  false,    false,   false,  false,     false,     false }, // FLOAT_NULLABLE
        { true,   true,     false,   true,   true,      false,     true  }, // INT_FLOAT_CONVERTABLE
};

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


DataTypeVariable variable_change_convention[6][7] = {
{DATA_TYPE_INT,DATA_TYPE_INT_CONVERTABLE,ERR,ERR,DATA_TYPE_INT_CONVERTABLE,ERR,DATA_TYPE_INT_CONVERTABLE},
{DATA_TYPE_INT,DATA_TYPE_INT_CONVERTABLE,ERR,ERR,DATA_TYPE_INT_CONVERTABLE,ERR,DATA_TYPE_INT_CONVERTABLE},
{DATA_TYPE_INT_NULLABLE,DATA_TYPE_INT_NULLABLE,DATA_TYPE_INT_NULLABLE,ERR,DATA_TYPE_INT_NULLABLE,ERR,DATA_TYPE_INT_NULLABLE},
{ERR,DATA_TYPE_FLOAT_CONVERTABLE,ERR,DATA_TYPE_FLOAT,DATA_TYPE_FLOAT_CONVERTABLE,ERR,DATA_TYPE_FLOAT_CONVERTABLE},
{ERR,DATA_TYPE_FLOAT_CONVERTABLE,ERR,DATA_TYPE_FLOAT,DATA_TYPE_FLOAT_CONVERTABLE,ERR,DATA_TYPE_FLOAT_CONVERTABLE},
{ERR,DATA_TYPE_FLOAT_NULLABLE,ERR,DATA_TYPE_FLOAT,DATA_TYPE_FLOAT_NULLABLE,DATA_TYPE_FLOAT_NULLABLE,ERR}
};



void semantic_parse(NodePtr node){
    SymTable *sym_table = init_sym_table();
    sym_table_create_insert_global_frame(sym_table);
    sym_table_set_current_to_first(sym_table);

    first_pass(node,sym_table);
    second_pass(node,sym_table);
}

void second_pass(NodePtr node,SymTable* sym_table){
    check_main_fn_is_correct(sym_table);

    NodePtr new_command = node->left;
    if(new_command == NULL)
        return;


    TData * current_frame =sym_table->current_frame;
    NodePtr function_node = new_command->right;
    frame_list = frame_list_create();
    while (function_node != NULL){
        char* fn_name = function_node->data.string_val;
        TData* fn_frame = sym_table_search(current_frame,fn_name);
        fn_frame->number_of_inner_frames = 0;
        frame_list_add(frame_list,fn_frame);
        check_body(current_frame,fn_frame,fn_frame,function_node->right);
        if(!fn_frame->function->contains_return && fn_frame->function->return_type != RETURN_TYPE_VOID)
            exit(6);

        Fn_Params* params = fn_frame->function->params;
        while (params != NULL){
            if(params->is_used == false){
                exit(9);
            }
            params = params->next;
        }

        new_command = new_command->left;
        function_node = new_command->right;
        frame_delete_all(frame_list);
    }


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
            char str1[100] = "ifj.";
            strcat(str1, node->right->left->data.string_val);
            node->right->left->data.string_val = str1;
            check_fn_call(global_frame,function_frame,current_frame,node->right->left);
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
                    exit(7);
                }
                data_type_of_expression = get_not_null_data_type_from_nullable(data_type_of_expression);
                id_no_null = node->right->left->right->data.string_val;

                check_redefinition_of_variable(inner_frame,function_frame,id_no_null);

                TData *variable_Frame = sym_table_create_data(id_no_null, scope, VARIABLE_FRAME);
                TData_Variable *var = create_tdata_variable(data_type_of_expression, CONST);
                variable_Frame->variable = var;
                sym_table_insert(current_frame,variable_Frame);
            } else{
                // bool expression
                DataTypeVariable data_type_of_expression = check_expression(global_frame,function_frame,current_frame,node->right->left->left);
                if(data_type_of_expression != DATA_TYPE_BOOL){
                    exit(7);
                }

            }

            check_body(global_frame,inner_frame,function_frame,node->right->right);
            check_body(global_frame,current_frame,function_frame,node->left);
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
                    exit(7);
                }
                data_type_of_expression = get_not_null_data_type_from_nullable(data_type_of_expression);
                id_no_null = node->right->left->right->data.string_val;

                check_redefinition_of_variable(inner_frame,function_frame,id_no_null);

                TData *variable_Frame = sym_table_create_data(id_no_null, scope, VARIABLE_FRAME);
                TData_Variable *var = create_tdata_variable(data_type_of_expression, CONST);
                variable_Frame->variable = var;
                sym_table_insert(current_frame,variable_Frame);

            } else{
                // bool expression
                DataTypeVariable data_type_of_expression = check_expression(global_frame,function_frame,current_frame,node->right->left->left);
                if(data_type_of_expression != DATA_TYPE_BOOL){
                    exit(7);
                }

            }

            check_body(global_frame,inner_frame,function_frame,node->right->right);
            break;
        }
        case T_ELSE:{
            char scope[SCOPE_IDENTIFIER_SIZE];
            memcpy(scope,current_frame->scope,SCOPE_IDENTIFIER_SIZE);
            scope[strlen(scope)] = scope[strlen(scope)] + 2;
            TData* inner_frame = sym_table_search(current_frame, scope);
            check_body(global_frame,inner_frame,function_frame,node->right->right);
            return;
        }
        case T_RETURN:{
            DataTypeVariable return_data_type = DATA_TYPE_NONE;
            if(node->right->right != NULL){
                if(node->right->right->keyword == T_STRING){
                    exit(7);
                }
                return_data_type = check_expression(global_frame,function_frame,current_frame,node->right->right);
            }

            if(function_frame->function->return_type == RETURN_TYPE_VOID){
                if(return_data_type != DATA_TYPE_NONE)
                    exit(6); // returning value in void fn
                return;
            } else if(return_data_type == DATA_TYPE_NONE){
                exit(6); // return with no value
                return;
            }
            if(!is_returned_value_correct(function_frame->function->return_type,return_data_type)){
                exit(4);
            }
            function_frame->function->contains_return = true;

        }
         default:{
            return;
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
            LData* ldata = frame_list_get(frame_list);
            while (ldata != NULL){
                TData* data = sym_table_search(ldata->frame,node->data.string_val);
                if(data!= NULL){
                   data->is_used = true;
                   return data->variable->data_type;
                };
                frame_list_next(frame_list);
                ldata = frame_list_get(frame_list);
            }
            Fn_Params* param = function_frame->function->params;
            while (param != NULL){
                if(strcmp(param->name,node->data.string_val) == 0){
                    param->is_used = true;
                    return param->data_type;
                };
                param = param->next;
            }
            return DATA_TYPE_NONE;
        }
        case T_PLUS:
        case T_MINUS:
        case T_ASTERISK:
        case T_SLASH: {
            DataTypeVariable left_type = DATA_TYPE_NONE;
            DataTypeVariable right_type = DATA_TYPE_NONE;
            if (node->left != NULL) {
                left_type = check_expression(global_frame, function_frame, current_Frame, node->left);
            }
            if (node->right != NULL) {
                right_type = check_expression(global_frame, function_frame, current_Frame, node->right);
            }
            return compare_data_type_variables(left_type, right_type);

        }

       case T_EQUALS:
       case T_NOTEQUAL:
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
           if(!is_bool_expression_ok(left_type,right_type)){
               exit(7);
           } else
               return DATA_TYPE_BOOL;
       }


        case FN_CALL:{
            TData* fn = sym_table_search(global_frame,node->data.string_val);
            if(fn == NULL){
                exit(3);
            }
            //only non void can be called without assignment
            if(fn->function->return_type == RETURN_TYPE_VOID){
                exit(7);
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
                    exit(4);
                DataTypeVariable param_type = check_expression(global_frame,function_frame,current_Frame,param_node);
                compare_variable_expression(reversed_params[i]->data_type,param_type);
                param_node = param_node->left;
            }
            //more arguments provided
            if(param_node != NULL){
                exit(4);
            }
            return map_ReturnType_to_DataTypeVariable(fn->function->return_type);
        }
        default:
            return DATA_TYPE_NONE;
    }

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
        case DATA_TYPE_NULL: //TODO handle ?i32 = null
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
        exit(7);
    }
    if(left_type == DATA_TYPE_STRING_NULLABLE || right_type == DATA_TYPE_STRING_NULLABLE){
        exit(7);
    }

    return bool_expression_convention[left_type][right_type];
}

DataTypeVariable compare_variable_expression(DataTypeVariable left_type, DataTypeVariable right_type){

    DataTypeVariable new_type = variable_change_convention[left_type][right_type];
    if(new_type == ERR)
        exit(7);
    else
        return new_type;
}


DataTypeVariable compare_data_type_variables(DataTypeVariable left_type, DataTypeVariable right_type){
    // operation are forbidden in strings
    if(left_type == DATA_TYPE_STRING || right_type == DATA_TYPE_STRING){
        exit(7);
    }
    if(left_type == DATA_TYPE_NONE){
        return right_type;
    }
    if (right_type == DATA_TYPE_NONE) {
        return left_type;
    }
    DataTypeVariable new_type = expression_convention[left_type][right_type];
    if(new_type == ERR)
        exit(7);
    else
        return new_type;
}



void check_fn_call(TData* global_frame,TData* function_frame,TData* current_frame,NodePtr node){
    TData* fn = sym_table_search(global_frame,node->data.string_val);
    if(fn == NULL){
        exit(3);
    }
    //only  void can be called without assignment
    if(fn->function->return_type != RETURN_TYPE_VOID){
        exit(4);
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
            exit(4);
        DataTypeVariable param_type = check_expression(global_frame,function_frame,current_frame,param_node);
        compare_variable_expression(reversed_params[i]->data_type,param_type);
        param_node = param_node->left;
    }
    //more arguments provided
    if(param_node != NULL){
        exit(4);
    }
}


void check_declaration_stmt(TData* global_frame,TData* function_frame,TData* current_frame,NodePtr node) {
    Variable_Type variable_type = map_tType_to_Variable_Type(node->left->keyword);
    char *name = node->left->data.string_val;
    TData *variable = NULL;
    DataTypeVariable data_type_of_variable;
    if (variable_type == NONE) {
        variable = check_assigment_only_to_var_is_accepted(name);
    }
    if (variable == NULL) {
        frame_first(frame_list);
        while (frame_list->current != NULL) {
            TData *previous_frame = frame_list_get(frame_list)->frame;
            frame_list_next(frame_list);

            if (strlen(previous_frame->scope) > strlen(previous_frame->scope))
                continue;
            variable = sym_table_search(previous_frame, name);

            if (variable != NULL) {
                data_type_of_variable = variable->variable->data_type;
                break;
            }
        }
        Fn_Params *param = function_frame->function->params;
        while (param != NULL) {
            if (strcmp(param->name, name) == 0) {
                data_type_of_variable = param->data_type;
                break;
            }
            param = param->next;
        }
    } else
        data_type_of_variable = variable->variable->data_type;

    if(node->right->keyword == T_STRING && variable_type == NONE)
        exit(7);

    if(node->right->keyword == T_STRING && variable_type != NONE)
        exit(8);

    DataTypeVariable expression_data_type = check_expression(global_frame, function_frame,current_frame, node->right);

    if (is_data_type_nullable(data_type_of_variable) == false && expression_data_type == DATA_TYPE_NULL) {
        exit(8);
    }

    if(variable_type == NONE || data_type_of_variable ==  DATA_TYPE_NONE){
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

TData* check_assigment_only_to_var_is_accepted(char* name){
    TData* variable = NULL;
    frame_first(frame_list);
    while (frame_list->current != NULL){
        TData * previous_frame = frame_list_get(frame_list)->frame;
        frame_list_next(frame_list);

        if(strlen(previous_frame->scope) > strlen(previous_frame->scope))
            continue;
        variable = sym_table_search(previous_frame,name);
        // assignment only to var is accepted
        if( variable != NULL ){
            break;
        }
    }
    if(variable == NULL || variable->variable->variable_type != VAR)
        exit(5);

    return variable;
}
void check_main_fn_is_correct(SymTable* sym_table){
    TData* main_fn = sym_table_search(sym_table->current_frame,"main");
    if(main_fn == NULL)
        exit(3);

    if (main_fn->function->return_type != RETURN_TYPE_VOID)
        exit(4);

    if (main_fn->function->params != NULL)
        exit(4);

}

void first_pass(NodePtr node,SymTable* sym_table){
    NodePtr new_command = node->left;
    if(new_command == NULL)
        return;

    frame_list = frame_list_create();

    TData * current_frame =sym_table->current_frame;
    add_build_in_functions(current_frame);

    NodePtr function_node = new_command->right;

    char scope[SCOPE_IDENTIFIER_SIZE] = {'\001',0,};
    while (function_node != NULL){
        char* fn_name = function_node->data.string_val;

        if(sym_table_search(current_frame,fn_name) != NULL){
            exit(5);
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
    frame_free(frame_list);

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
                parse_body(function_frame,inner_frame,node->right->right);
                parse_body(function_frame,current_frame,node->left->right);
                break;
            }
            case T_ELSE:{
                char scope[SCOPE_IDENTIFIER_SIZE];
                memcpy(scope,current_frame->scope,SCOPE_IDENTIFIER_SIZE);
                scope[strlen(scope)] = scope[strlen(scope)]+ current_frame->number_of_inner_frames + 1;
                TData* inner_frame = sym_table_create_data(scope, scope, INNER_FRAME);

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


        //if(strlen(current_frame->scope) > strlen(in_frame->scope))
          //  continue;

        if(sym_table_search(current_frame,var_name) != NULL)
            exit(5);

    }
    //redefinition of function parameter
    Fn_Params* fn_param = function_frame->function->params;
    while (fn_param != NULL){
        if(strcmp(fn_param->name,var_name)== 0){
            exit(5);
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
    sym_table_insert(frame, variable_Frame);
}
void parse_function_params(TData_Fn* data_fn,NodePtr node ){
    while (node != NULL){
        DataTypeVariable data_type_variable = map_node_to_DataTypeVariable(node);
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
        case T_STRING:{
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
    sym_table_insert(global_frame,fn_Frame);
    fn_Frame =sym_table_create_data( "ifj.readi32",scope,FUNCTION_FRAME);
    return_type = RETURN_TYPE_INT_NULLABLE;
    fn_Frame->function = create_tdata_fn(return_type);
    fn_Frame->function->params = NULL;
    sym_table_insert(global_frame,fn_Frame);


    fn_Frame =sym_table_create_data( "ifj.readf64",scope,FUNCTION_FRAME);
    return_type = RETURN_TYPE_FLOAT_NULLABLE;
    fn_Frame->function = create_tdata_fn(return_type);
    fn_Frame->function->params = NULL;
    sym_table_insert(global_frame,fn_Frame);

    fn_Frame =sym_table_create_data( "ifj.i2f",scope,FUNCTION_FRAME);
    return_type = RETURN_TYPE_FLOAT;
    fn_Frame->function = create_tdata_fn(return_type);
    Fn_Params* params = malloc(sizeof(Fn_Params));
    *params = (Fn_Params){.next = NULL, .name= "term", .data_type = DATA_TYPE_INT, .is_used = false};
    fn_Frame->function->params = params;
    sym_table_insert(global_frame,fn_Frame);

    fn_Frame =sym_table_create_data( "ifj.f2i",scope,FUNCTION_FRAME);
    return_type = RETURN_TYPE_INT;
    fn_Frame->function = create_tdata_fn(return_type);
    params = malloc(sizeof(Fn_Params));
    *params = (Fn_Params){.next = NULL, .name= "term", .data_type = DATA_TYPE_FLOAT, .is_used = false};
    fn_Frame->function->params = params;
    sym_table_insert(global_frame,fn_Frame);

    fn_Frame =sym_table_create_data( "ifj.string",scope,FUNCTION_FRAME);
    return_type = RETURN_TYPE_STRING;
    fn_Frame->function = create_tdata_fn(return_type);
    params = malloc(sizeof(Fn_Params));
    *params = (Fn_Params){.next = NULL, .name= "term", .data_type = DATA_TYPE_STRING, .is_used = false};
    fn_Frame->function->params = params;
    sym_table_insert(global_frame,fn_Frame);


    fn_Frame =sym_table_create_data( "ifj.length",scope,FUNCTION_FRAME);
    return_type = RETURN_TYPE_INT;
    fn_Frame->function = create_tdata_fn(return_type);
    params = malloc(sizeof(Fn_Params));
    *params = (Fn_Params){.next = NULL, .name= "s", .data_type = DATA_TYPE_STRING, .is_used = false};
    fn_Frame->function->params = params;
    sym_table_insert(global_frame,fn_Frame);


    fn_Frame =sym_table_create_data( "ifj.concat",scope,FUNCTION_FRAME);
    return_type = RETURN_TYPE_STRING;
    fn_Frame->function = create_tdata_fn(return_type);
    params = malloc(sizeof(Fn_Params));
    *params = (Fn_Params){.next = NULL, .name= "s1", .data_type = DATA_TYPE_STRING, .is_used = false};
    Fn_Params* params2 = malloc(sizeof(Fn_Params));
    *params2 = (Fn_Params){.next = NULL, .name= "s2", .data_type = DATA_TYPE_STRING, .is_used = false};
    params2->next=params;
    fn_Frame->function->params = params2;
    sym_table_insert(global_frame,fn_Frame);


    fn_Frame =sym_table_create_data( "ifj.substring",scope,FUNCTION_FRAME);
    return_type = RETURN_TYPE_STRING_NULLABLE;
    fn_Frame->function = create_tdata_fn(return_type);
    params = malloc(sizeof(Fn_Params));
    params2 = malloc(sizeof(Fn_Params));
    Fn_Params* params3 = malloc(sizeof(Fn_Params));
    *params = (Fn_Params){.next = NULL, .name= "s", .data_type = DATA_TYPE_STRING, .is_used = false};
    *params2 = (Fn_Params){.next = NULL, .name= "i", .data_type = DATA_TYPE_INT, .is_used = false};
    *params3 = (Fn_Params){.next = NULL, .name= "j", .data_type = DATA_TYPE_INT, .is_used = false};
    params2->next=params;
    params3->next = params2;
    fn_Frame->function->params = params3;
    sym_table_insert(global_frame,fn_Frame);


    fn_Frame =sym_table_create_data( "ifj.strcmp",scope,FUNCTION_FRAME);
    return_type = RETURN_TYPE_INT;
    fn_Frame->function = create_tdata_fn(return_type);
    params = malloc(sizeof(Fn_Params));
    params2 = malloc(sizeof(Fn_Params));
    *params = (Fn_Params){.next = NULL, .name= "s1", .data_type = DATA_TYPE_STRING, .is_used = false};
    *params2 = (Fn_Params){.next = NULL, .name= "s2", .data_type = DATA_TYPE_STRING, .is_used = false};
    params2->next=params;
    fn_Frame->function->params = params2;
    sym_table_insert(global_frame,fn_Frame);

    fn_Frame =sym_table_create_data( "ifj.ord",scope,FUNCTION_FRAME);
    return_type = RETURN_TYPE_INT;
    fn_Frame->function = create_tdata_fn(return_type);
    params = malloc(sizeof(Fn_Params));
    params2 = malloc(sizeof(Fn_Params));
    *params = (Fn_Params){.next = NULL, .name= "s", .data_type = DATA_TYPE_STRING, .is_used = false};
    *params2 = (Fn_Params){.next = NULL, .name= "i", .data_type = DATA_TYPE_INT, .is_used = false};
    params2->next=params;
    fn_Frame->function->params = params2;
    sym_table_insert(global_frame,fn_Frame);

    fn_Frame =sym_table_create_data( "ifj.chr",scope,FUNCTION_FRAME);
    return_type = RETURN_TYPE_STRING;
    fn_Frame->function = create_tdata_fn(return_type);
    params = malloc(sizeof(Fn_Params));
    *params = (Fn_Params){.next = NULL, .name= "i", .data_type = DATA_TYPE_INT, .is_used = false};
    fn_Frame->function->params = params;
    sym_table_insert(global_frame,fn_Frame);
}

bool is_build_in_fn(char* name){
    const char* built_in_functions[] = {
            "ifj.readstr",
            "ifj.readi32",
            "ifj.readf64",
            "ifj.i2f",
            "ifj.f2i",
            "ifj.string",
            "ifj.length",
            "ifj.concat",
            "ifj.substring",
            "ifj.strcmp",
            "ifj.ord",
            "ifj.chr"
    };

    for (size_t i = 0; i < 12; ++i) {
        if (strcmp(name, built_in_functions[i]) == 0) {
            return true;
        }
    }

    return false;
}