#include "scanner.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

int parser(){
    Token token = malloc(sizeof(struct Token));
    token->type = T_UNDEFINED;
    token->data = NULL;

    getToken(token);
    getToken(token);
    getToken(token);

    free(token);
}