/*string.h - code generator*/
/*november 2024*/

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#ifndef STRING_H
#define STRING_H
#define STRING_LENGHT 30

typedef struct tstring {
    char *data;
    size_t lenght;
    size_t max_lenght;
} tString;

tString* string_init();
void string_free(tString *s);
void string_clear(tString* s);
tString* string_append(tString *s, char c);
void string_concat(tString *s, char *c );
tString* string_push(tString* s, char *string);
tString* string_push_int(tString *s, int i);

#endif