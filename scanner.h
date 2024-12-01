/**
 * @file scanner.h
 * @brief Header file for the lexical analyzer, which 
 * defines the structures, enumerations, and function prototypes.
 *
 * @author Peter Huňady (xhunadp00)
 *
 * @date 2024
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef IFJ_PROJEKT_SCANNER_H
#define IFJ_PROJEKT_SCANNER_H

/**
 * @enum tType
 * @brief Enumeration for token types.
 *
 * Defines all possible types of tokens that can be identified by the lexical analyzer.
 */
typedef enum {
    T_ID,
    T_IFJ,
    T_UNDEFINED,
    T_ERROR,
    T_EOF,
    T_COMMENT,
    T_INT,
    T_FLOAT,
    T_LBRACKET,         // (
    T_RBRACKET,         // )
    T_CLBRACKET,        // {
    T_CRBRACKET,        // }
    T_SLBRACKET,        // [
    T_SRBRACKET,        // ]
    T_AND,              // &
    T_PIPE,             // |
    T_GREATER,          // >
    T_GREATEREQUAL,     // >=
    T_LESS,             // <
    T_LESSEQUAL,        // <=
    T_EQUALSIGN,        // =
    T_EQUALS,           // ==
    T_NOT,              // !
    T_NOTEQUAL,         // !=
    T_PLUS,             // +
    T_MINUS,            // -
    T_ASTERISK,         // *
    T_SLASH,            // /
    T_DOT,              // .
    T_SEMICOLON,        // ;
    T_COLON,            // :
    T_COMMA,            // ,
    T_QUESTIONMARK,     // ?
    T_STRING,           // ""
    T_IMPORT,           // @import
    T_CONST,            // keyword const
    T_ELSE,             // keyword else
    T_FN,               // keyword fn
    T_IF,               // keyword if
    T_I32,              // keyword i32
    T_F64,              // keyword f64
    T_NULL,             // keyword null
    T_PUB,              // keyword pub
    T_RETURN,           // keyword fn
    T_U8,               // keyword u8
    T_VAR,              // keyword var
    T_VOID,             // keyword void
    T_WHILE,            // keyword while
    NEW_COMMAND,
    START,
    FN_CALL,
    FN_PARAM,
    FN_DATA,
    IF_DATA,
    WHILE_DATA,
    INVALID_TOKEN,
    IF_ELSE_BODY,
    STACK_END
} tType;


/**
 * @enum sState
 * @brief Enumeration for finite state machine (FSM) states.
 *
 * Represents the possible states of the lexical analyzer's FSM.
 */
typedef enum {
    S_START,
    S_ID,
    S_NULL,
    S_ERROR,
    S_LBRACKET,         // (
    S_RBRACKET,         // )
    S_CLBRACKET,        // {
    S_CRBRACKET,        // }
    S_SLBRACKET,        // [
    S_SRBRACKET,        // ]
    S_PIPE,             // |
    S_GREATER,          // >
    S_GREATEREQUAL,     // >=
    S_LESS,             // <
    S_LESSEQUAL,        // <=
    S_EQUALSIGN,        // =
    S_EQUALS,           // ==
    S_NOTEQUAL,         // !=
    S_NOT,              // !
    S_PLUS,             // +
    S_MINUS,            // -
    S_ASTERISK,         // *
    S_SLASH,            // /
    S_DOT,              // .
    S_SEMICOLON,        // ;
    S_COLON,            // :
    S_COMMA,            // ,
    S_QUESTIONMARK,     // ?
    S_STRING,           // ""
    S_STRING2,
    S_STRING3,
    S_STRING4,
    S_MULTISTRING,
    S_MULTISTRING2,
    S_MULTISTRING3,
    S_INT,
    S_FLOAT,
    S_FLOAT2,
    S_EXP,
    S_EXP2,
    S_EXP3,
    S_COMMENT,
} sState;

/**
 * @struct Token
 * @brief Structure for tokens.
 *
 * Represents a token with its type and associated data.
 */
typedef struct Token {
    char *data;
    tType type;
} *Token;

/**
 * @brief Checks if a token is a keyword and updates its type if true.
 *
 * @param token The token to check.
 * @return 1 if the token is a keyword, 0 otherwise.
 */
int check_keywords(Token token);

/**
 * @brief Converts a token to a string for debugging.
 *
 * @param token The token to convert.
 * @return A constant string representation of the token type.
 */
const char *token_to_string(Token token);

/**
 * @brief Processes input using the finite state machine (FSM).
 *
 * @param file The file from which characters are read.
 * @param token The token to populate based on input.
 * @return 0 on success, non-zero on error.
 */
int token_fsm(FILE *file, Token token);

/**
 * @brief Reads the next token from the source file.
 *
 * @param token The token structure to fill with data.
 * @return 0 on success, non-zero on error.
 */
int get_token(Token restrict token);

/**
 * @brief Sets the source file for lexical analysis.
 *
 * @param file Pointer to the file to analyze.
 */
void set_source_file(FILE *file);

#endif
