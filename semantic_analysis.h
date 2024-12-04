/**
 * @file semantic_analysis.h
 * @brief Semantic Analysis for the IFJ project.
 *
 * @author Marko Poľanský (xpolan12)
 *
 * @date 2024
 */

#include "scanner.h"
#include "parser.h"
#include "symtable.h"
#include "frame_list.h"

#ifndef IFJ_PROJEKT_SEMANTIC_ANALYSIS_H
#define IFJ_PROJEKT_SEMANTIC_ANALYSIS_H



/**
 * @brief Performs the first pass of semantic analysis on the abstract syntax tree (AST).
 *
 * This function traverses the AST and performs the first stage of semantic checks, such as validating
 * function and variable declarations, as well as handling scope and type-related issues.
 *
 * @param node The root node of the abstract syntax tree to be analyzed.
 */
void first_pass(NodePtr node);
/**
 * @brief Performs the second pass of semantic analysis on the AST.
 *
 * This second stage of semantic analysis further checks the consistency of the program, such as ensuring
 * proper function calls, variable assignments, and type checks.
 *
 * @param node The root node of the abstract syntax tree to be analyzed.
 */
void second_pass(NodePtr node);

/**
 * @brief Orchestrates the semantic analysis for the entire AST.
 *
 * This function drives the semantic analysis by invoking both the first and second passes and handling
 * any semantic errors encountered during the process.
 *
 * @param node The root node of the abstract syntax tree to be analyzed.
 */
void semantic_parse(NodePtr node);



/**
 * @brief Parses the parameters of a function and adds them to the function's data structure.
 *
 * This function processes the function parameters from the AST and updates the function's metadata,
 * such as adding the parameters to the function's symbol table.
 *
 * @param data_fn The function data structure to store the parsed parameters.
 * @param node The node representing the function parameters in the AST.
 */
void parse_function_params(TData_Fn* data_fn,NodePtr node);


/**
 * @brief Parses the return type of a function from the AST.
 *
 * This function extracts the return type of a function from the AST and returns it as an enumerated value
 * of the ReturnTypes type.
 *
 * @param node The node representing the return value of the function in the AST.
 * @return The return type of the function.
 */
ReturnTypes parse_function_return_value(NodePtr node);

/**
 * @brief Parses the body of a function, including variable declarations and statements.
 *
 * This function processes the body of the function, adding all declarations of function and variable to
 * the symtable
 *
 * @param function_frame The function's frame, representing its local symbol table.
 * @param current_frame The current symbol table to check within.
 * @param node The node representing the function body in the AST.
 */
void parse_body(TData* function_frame,TData* current_frame,NodePtr node);

/**
 * @brief Maps a node from the AST to a DataTypeVariable.
 *
 * This function converts an AST node into the appropriate data type variable, representing the type of the
 * expression or variable in the node.
 *
 * @param node The node to map to a data type.
 * @return The corresponding data type.
 */
DataTypeVariable map_node_to_DataTypeVariable(NodePtr node);

/**
 * @brief Maps a node from the AST to a ReturnTypes value.
 *
 * This function converts an AST node into a ReturnTypes enumeration, representing the return type of a function.
 *
 * @param node The node to map to a return type.
 * @return The corresponding return type.
 */
ReturnTypes map_node_to_ReturnTypes(NodePtr node);
/**
 * @brief Maps a tType to a Variable_Type.
 *
 * This function converts a tType (parser type) into the appropriate variable type.
 *
 * @param type The tType to map to a variable type.
 * @return The corresponding variable type.
 */
Variable_Type map_tType_to_Variable_Type(tType type);
/**
 * @brief Maps a ReturnTypes value to a DataTypeVariable.
 *
 * This function converts a return type to a corresponding data type variable, ensuring type consistency.
 *
 * @param type The return type to map to a data type.
 * @return The corresponding data type.
 */
DataTypeVariable map_ReturnType_to_DataTypeVariable(ReturnTypes type);


/**
 * @brief Checks if a given data type is nullable.
 *
 * This function checks if the specified data type allows nullable values, such as for optional types or references.
 *
 * @param data_type The data type to check.
 * @return true if the data type is nullable, false otherwise.
 */
bool is_data_type_nullable(DataTypeVariable data_type);

/**
 * @brief Compares two data types for compatibility in expressions.
 *
 * This function compares the left and right data types of an expression and returns a data type that is
 * compatible with both types. If types are not compatible it will return ERR
 *
 * @param left_type The data type of the left operand.
 * @param right_type The data type of the right operand.
 * @return A compatible data type.
 */
DataTypeVariable compare_variable_expression(DataTypeVariable left_type, DataTypeVariable right_type);


/**
 * @brief Compares two data type variables for compatibility.
 *
 * This function checks if two data type variables are compatible, such as comparing whether two types can
 * be used together in expressions or assignments.
 *
 * @param left_type The data type of the left operand.
 * @param right_type The data type of the right operand.
 * @return A compatible data type.
 */
DataTypeVariable compare_data_type_variables(DataTypeVariable left_type, DataTypeVariable right_type);
/**
 * @brief Checks if a boolean expression is valid with respect to its operands' types.
 *
 * This function ensures that the left and right operands of a boolean expression are of compatible types
 * for logical operations.
 *
 * @param left_type The data type of the left operand.
 * @param right_type The data type of the right operand.
 * @return true if the boolean expression is valid, false otherwise.
 */
bool is_bool_expression_ok(DataTypeVariable left_type, DataTypeVariable right_type);


/**
 * @brief Parses a variable declaration statement.
 *
 * This function processes the declaration of a variable in the AST, ensuring that the variable is
 * properly defined and that its type is correct.
 *
 * @param frame The current frame to store the variable.
 * @param function_frame The frame representing the function context.
 * @param node The node representing the variable declaration in the AST.
 */
void parse_declaration_stmt(TData* frame,TData* function_frame,NodePtr node);

/**
 * @brief Gets a non-nullable data type from a nullable expression.
 *
 * This function handles nullable types in boolean expressions, ensuring that the nullable type is converted
 * to a non-nullable one for further analysis.
 *
 * @param data_type_of_bool_expression The nullable data type.
 * @return A non-nullable data type.
 */
DataTypeVariable get_not_null_data_type_from_nullable(DataTypeVariable data_type_of_bool_expression);

/**
 * @brief Checks if the returned value of a function matches the expected return type.
 *
 * This function compares the expected return type of a function with the actual type returned by the function.
 *
 * @param expected_from_fn The expected return type.
 * @param returned The returned data type.
 * @return true if the types match, false otherwise.
 */
bool is_returned_value_correct(ReturnTypes expected_from_fn, DataTypeVariable returned);

/**
 * @brief Checks the expression for semantic validity.
 *
 * This function checks the semantic correctness of an expression by verifying the types of its operands
 * and ensuring that the operation is valid for the given types.
 *
 * @param global_frame The global symbol table to check against.
 * @param function_frame The function symbol table to check against.
 * @param current_frame The current frame to check against.
 * @param node The node representing the expression to check.
 * @return The data type of the expression after validation.
 */
DataTypeVariable check_expression(TData* global_frame,TData* function_frame,TData* current_frame,NodePtr node);


/**
 * @brief Checks if only variables are allowed on the left-hand side of an assignment.
 *
 * This function ensures that assignments only target variables (not constants).
 *
 * @param name The name of the variable being assigned to.
 * @param in_frame The frame where the variable should be searched for.
 * @param current_frame The current frame for the semantic check.
 * @return A pointer to the variable data if it is valid, NULL otherwise.
 */
TData* check_assigment_only_to_var_is_accepted(char* name, TData* in_frame, TData* current_frame);

/**
 * @brief Checks the semantic correctness of a function call.
 *
 * This function verifies that the function being called exists, that the parameters match the function's
 * definition, and that the return type is valid.
 *
 * @param global_frame The global symbol table to check against.
 * @param function_frame The function symbol table to check against.
 * @param current_frame The current frame to check against.
 * @param node The node representing the function call.
 */
void check_fn_call(TData* global_frame,TData* function_frame,TData* current_frame,NodePtr node);
/**
 * @brief Checks the semantic correctness of a declaration statement.
 *
 * This function checks that variables are properly declared, including type checks and scope validation.
 *
 * @param global_frame The global symbol table to check against.
 * @param function_frame The function symbol table to check against.
 * @param current_frame The current frame to check against.
 * @param node The node representing the declaration statement.
 */
void check_declaration_stmt(TData* global_frame,TData* function_frame,TData* current_frame,NodePtr node);

/**
 * @brief Checks the semantic correctness of the body of a function.
 *
 * This function verifies that the body of a function is semantically valid, including checking the types of
 * expressions, the correctness of variable declarations, and ensuring there are no invalid statements or
 * operations within the function's body.
 *
 * @param global_frame The global symbol table for context.
 * @param current_frame The current function or scope's symbol table.
 * @param function_frame The symbol table representing the function's local scope.
 * @param node The node representing the body of the function in the AST.
 */
void check_body(TData* global_frame,TData* current_frame,TData* function_frame,NodePtr node);

/**
 * @brief Checks if a variable is redefined within the same scope.
 *
 * This function ensures that variables are not redefined within the same and parent scopes, which would cause a conflict
 * in variable names. It checks for a variable's existence in the given frame and validates that no other
 * variable with the same name exists.
 *
 * @param current_frame The current symbol table where the variable should be checked for redefinition.
 * @param frame The symbol table of the scope being checked.
 * @param var_name The name of the variable to check for redefinition.
 */
void check_redefinition_of_variable(TData* current_frame,TData* frame, char* var_name);

/**
 * @brief Handles semantic errors, reporting the error and freeing resources.
 *
 * This function handles any semantic errors encountered during analysis by printing an error message,
 * freeing any allocated memory that needs to be cleaned up, and then terminating or reporting the error.
 *
 * @param error The error code representing the specific semantic error.
 */
void semantic_error(int error);

/**
 * @brief Checks if the main function is correctly defined.
 *
 * This function ensures that the main function, which serves as the entry point of the program, is defined
 * properly with the correct signature, return type, and presence within the code.
 */
void check_main_fn_is_correct();


/**
 * @brief Adds built-in functions to the global symbol table.
 *
 * This function adds predefined functions to the global symbol table
 * to make them available during semantic analysis and throughout the program.
 *
 * @param global_frame The global symbol table to insert the built-in functions into.
 */
void add_build_in_functions(TData* global_frame);


#endif //IFJ_PROJEKT_SEMANTIC_ANALYSIS_H
