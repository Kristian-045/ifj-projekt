#include "scanner.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

//scanner.c

FILE *source_file;

int getToken(Token restrict token) {
    if (token == NULL) return -1; // Error handling

    // Reset token
    token->type = T_UNDEFINED;
    token->data = NULL;

    // Read the next token from the source file
    if (tokenFSM(source_file, token)) {
        printf("Error in token FSM\n");
        return -1; // Return error
    }

    // Print token type and data
    printf("Token: %s ", tokenToString(token));
    if (token->data) printf("-> %s\n", token->data);
    else printf("\n");

    return token->type;
}


void setSourceFile(FILE *file)
{
    source_file = file;
}

int tokenFSM(FILE* file, Token token) {
    sState state = S_START;
    sState newState = S_NULL;
    token->type = T_UNDEFINED;
    token->data = NULL;

    char character = '\0';
    unsigned stringLength = 100, stringPosition = 0;
    char *string = malloc(sizeof(char)*stringLength);


    if (!string) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    

    while (true) {
        character = getc(file);        
        
        if (stringPosition + 1 >= stringLength){
            stringLength *= 2;
            string = realloc(string, sizeof(char)*stringLength);
            if (!string) {
                free(string);
                fprintf(stderr, "Memory reallocation failed\n");
                exit(1);  
            }
        }

        if (!isspace(character) || state == S_STRING || state == S_ID || state == S_INT || state == S_FLOAT || state == S_COMMENT){
            string[stringPosition] = character;
            string[stringPosition + 1] = '\0';
            stringPosition++;  
        }

        newState = S_NULL;

        switch (state) {
            case S_START:
                if (character == '(') newState = S_LBRACKET;
                else if (character == ')') newState = S_RBRACKET;
                else if (character == '{') newState = S_CLBRACKET;
                else if (character == '}') newState = S_CRBRACKET;
                else if (character == '[') newState = S_SLBRACKET;
                else if (character == ']') newState = S_SRBRACKET;
                else if (character == '|') newState = S_PIPE; 
                else if (character == '>') newState = S_GREATER;
                else if (character == '<') newState = S_LESS;
                else if (character == '=') newState = S_EQUALSIGN;
                else if (character == '!') newState = S_NOT;
                else if (character == '+') newState = S_PLUS;
                else if (character == '-') newState = S_MINUS;
                else if (character == '*') newState = S_ASTERISK;
                else if (character == '/') newState = S_SLASH;
                else if (character == '"') newState = S_STRING;
                else if (character == '.') newState = S_DOT;
                else if (character == ';') newState = S_SEMICOLON;
                else if (character == ':') newState = S_COLON;
                else if (character == ',') newState = S_COMMA;
                else if (character == '?') newState = S_QUESTIONMARK;
                else if (isdigit(character)) newState = S_INT;
                else if (isspace(character)) continue;
                else if (isalpha(character) || character == '_' || character == '@') newState = S_ID;
                else newState = S_ERROR;            
                break;
            case S_LBRACKET:
                token->type = T_LBRACKET;
                break; 
            case S_RBRACKET:
                token->type = T_RBRACKET;
                break;  
            case S_CLBRACKET:
                token->type = T_CLBRACKET;
                break;
            case S_CRBRACKET:
                token->type = T_CRBRACKET;
                break;
            case S_SLBRACKET:
                token->type = T_SLBRACKET;
                break;
            case S_SRBRACKET:
                token->type = T_SRBRACKET;
                break;
            case S_SEMICOLON:
                token->type = T_SEMICOLON;
                break;
            case S_DOT:
                token->type = T_DOT;
                break;
            case S_COLON:
                token->type = T_COLON;
                break;
            case S_COMMA:
                token->type = T_COMMA;
                break;
            case S_QUESTIONMARK:
                token->type = T_QUESTIONMARK;
                break;
            case S_PIPE:
                token->type = T_PIPE;
                break;
            case S_PLUS:
                token->type = T_PLUS;
                break;
            case S_MINUS:
                token->type = T_MINUS;
                break;
            case S_ASTERISK:
                token->type = T_ASTERISK;
                break;
            case S_SLASH:
                if(character == '/') newState = S_COMMENT;
                else token->type = T_SLASH;
                break;
            case S_COMMENT:
                if (character == '\n' || character == EOF) token->type = T_COMMENT;
                else newState = S_COMMENT;
                break;
            case S_EQUALSIGN:
                if(character == '=') newState = S_EQUALS;
                else token->type = T_EQUALSIGN;
                break;
            case S_EQUALS:
                token->type = T_EQUALS;
                break;
            case S_NOT:
                if(character == '=') newState = S_NOTEQUAL;
                else token->type = T_NOT;
                break;
            case S_NOTEQUAL:
                token->type = T_NOTEQUAL;
                break;
            case S_GREATER:
                if(character == '=') newState = S_GREATEREQUAL;
                else token->type = T_GREATER;
                break;
            case S_GREATEREQUAL:
                token->type = T_GREATEREQUAL;
                break;
            case S_LESS:
                if(character == '=') newState = S_LESSEQUAL;
                else token->type = T_LESS;
                break;
            case S_LESSEQUAL:
                token->type = T_LESSEQUAL;
                break;
            case S_INT:
                if (isdigit(character)) newState = S_INT;
                else if (character == '.') newState = S_FLOAT;
                else token->type = T_INT;
                break;
            case S_FLOAT:
                if(isdigit(character)) newState = S_FLOAT;
                else token->type = T_FLOAT;    
                break;
            case S_STRING:
                if (character == '"' && stringPosition > 1) token->type = T_STRING; 
                else if(character == '\n') newState = S_ERROR;
                else newState = S_STRING;
                break;
            case S_ID:
                if ((isalpha(character) || isdigit(character) || character == '_')) newState = S_ID;
                else token->type = T_ID;
                break;
            default:
                break;
        }
        
        if (token->type != T_UNDEFINED || newState == S_NULL) {
            if(token->type != T_IMPORT && token->type != T_STRING ) ungetc(character, file);
            break;
        }


        if (newState == S_ERROR && character != EOF){
            token->type = T_ERROR;
            printf("Error\n"); 
            break;
        }

        if (character == EOF) {
            token->type = T_EOF;
            break;
        }
        
        state = newState;
    }


    switch(token->type){
        case T_INT:
        case T_FLOAT:
        case T_ID:
        case T_COMMENT:
            string[stringPosition - 1] = '\0';
            if(strcmp(string, "@import") == 0) token->type = T_IMPORT;
            token->data = malloc(strlen(string) + 1);
            if (token->data != NULL) {
                strcpy(token->data, string);  // Copy the string
            }

            if(token->data && checkKeywords(token)){
                
            }
            break;
        case T_STRING:
            string[stringPosition] = '\0';


            token->data = malloc(strlen(string) + 1);
            if (token->data != NULL) {
                strcpy(token->data, string);  // Copy the string
            }
            break;
        default:
            break;
    }
    
    free(string);
    return 0;
}

int checkKeywords(Token token){
    if (strcmp(token->data, "const") == 0) token->type = T_CONST;
    else if (strcmp(token->data, "else") == 0) token->type = T_ELSE;
    else if(strcmp(token->data, "fn") == 0) token->type = T_FN;
    else if(strcmp(token->data, "if") == 0) token->type = T_IF;
    else if(strcmp(token->data, "i32") == 0) token->type = T_I32;
    else if(strcmp(token->data, "f64") == 0) token->type = T_F64;
    else if(strcmp(token->data, "null") == 0) token->type = T_NULL;
    else if(strcmp(token->data, "pub") == 0) token->type = T_PUB;
    else if(strcmp(token->data, "return") == 0) token->type = T_RETURN;
    else if(strcmp(token->data, "u8") == 0) token->type = T_U8;
    else if(strcmp(token->data, "var") == 0) token->type = T_VAR;
    else if(strcmp(token->data, "void") == 0) token->type = T_VOID;
    else if(strcmp(token->data, "while") == 0) token->type = T_WHILE;
    else return 0;

    return 1;
}


const char* tokenToString(Token token) {
    switch(token->type) {
        case T_ID:
            return "T_ID";
        case T_LBRACKET:
            return "T_LBRACKET";
        case T_RBRACKET:
            return "T_RBRACKET";
        case T_CLBRACKET:
            return "T_CLBRACKET";
        case T_CRBRACKET:
            return "T_CRBRACKET";    
        case T_SLBRACKET:
            return "T_SLBRACKET";
        case T_SRBRACKET:
            return "T_SRBRACKET";
        case T_AND:
            return "T_AND";
        case T_PIPE:
            return "T_PIPE";
        case T_GREATER:
            return "T_GREATER";
        case T_GREATEREQUAL:
            return "T_GREATEREQUAL";
        case T_LESS:
            return "T_LESS";
        case T_LESSEQUAL:
            return "T_LESSEQUAL";
        case T_EQUALSIGN:
            return "T_EQUALSIGN";
        case T_EQUALS:
            return "T_EQUALS";
        case T_NOTEQUAL:
            return "T_NOTEQUAL";
        case T_NOT:
            return "T_NOT";
        case T_PLUS:
            return "T_PLUS";
        case T_MINUS:
            return "T_MINUS";
        case T_ASTERISK:
            return "T_ASTERISK";
        case T_SLASH:
            return "T_SLASH";
        case T_COMMENT:
            return "T_COMMENT";
        case T_CONST:
            return "T_CONST";
        case T_ELSE:
            return "T_ELSE";
        case T_FN:
            return "T_FN";
        case T_IF:
            return "T_IF";
        case T_I32:
            return "T_I32";
        case T_F64:
            return "T_F64";
        case T_NULL:
            return "T_NULL";
        case T_PUB:
            return "T_PUB";
        case T_U8:
            return "T_U8";
        case T_RETURN:
            return "T_RETURN";
        case T_VAR:
            return "T_VAR";
        case T_VOID:
            return "T_VOID";
        case T_WHILE:
            return "T_WHILE";
        case T_IMPORT:
            return "T_IMPORT";
        case T_STRING:
            return "T_STRING";
        case T_SEMICOLON:
            return "T_SEMICOLON";
        case T_DOT:
            return "T_DOT";
        case T_COLON:
            return "T_COLON";
        case T_COMMA:
            return "T_COMMA";
        case T_QUESTIONMARK:
            return "T_QUESTIONMARK";
        case T_INT:
            return "T_INT";
        case T_FLOAT:
            return "T_FLOAT";
        case T_EOF:
            return "T_EOF";
        default:
            return "undefined";
    }
}