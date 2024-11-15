#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

//scanner.h

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

    //for parser
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
    S_INT,
    S_FLOAT,
    S_FLOAT2,
    S_EXP,
    S_EXP2,
    S_EXP3,
    S_COMMENT,
} sState;


typedef struct Token {
    char *data;
    tType type;
} *Token;

int checkKeywords(Token token);

const char *tokenToString(Token token);

int tokenFSM(FILE *file, Token token);

int getToken(Token restrict token);

void setSourceFile(FILE *file);

