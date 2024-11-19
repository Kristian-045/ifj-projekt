/*string.c - code generator*/
/*november 2024*/

#include <stdlib.h>
#include <stdbool.h>
#include "string.h"

tString* string_init(tString *s) {
    //tString *s = malloc(sizeof(tString));
    if (s == NULL) {
        exit(EXIT_FAILURE);
    }
    s->data = malloc(STRING_LENGHT * sizeof(char));

    if(s->data == NULL) {
        free(s);
        exit(EXIT_FAILURE); //to do
    }

    s->data[0] = '\0';
    s->lenght = 0;
    s->max_lenght = STRING_LENGHT;
    return s;
}

void string_free(tString *s) {
    if ( s == NULL) {
        //to do
        return;
    }
    free(s->data);
    s->data = NULL;
    free(s);
}

void string_clear(tString* s) {
    if(s == NULL || s->data == NULL) {
        return;
    }
    s->lenght = 0;
    s->data[0] = '\0';
}

tString* string_append(tString *s, char c) {
    if ( s == NULL || s->data == NULL) {
        return NULL;
    }

    if (s->lenght + 1 >= s->max_lenght) {
        size_t new_lenght = (s->max_lenght * 2);
        char *tmp = realloc(s->data, sizeof(char) * new_lenght);
        if (tmp == NULL) {
            return NULL;  //to do
        }
        s->data = tmp;
        s->max_lenght = new_lenght;
    }
     s->data[s->lenght] = c;
     s->lenght++;
     s->data[s->lenght] = '\0';

     return s;
}


void string_concat(tString *s, char *c ) {
    if (s == NULL || c == NULL) {
        return;
    }
    int o_lenght = strlen(c);

    int new_lenght = s->lenght + o_lenght;

    if ( new_lenght >= s->max_lenght) {
        char *tmp = realloc(s->data, 2 * s->max_lenght);
        if (tmp == NULL) {
            exit(EXIT_FAILURE);  //to do
        }
        s->data = tmp;
        s->max_lenght *= 2;
    }
    strncpy(s->data + s->lenght, c, o_lenght);
    s->lenght = new_lenght;
    s->data[s->lenght] = '\0';
}

tString* string_push(tString* s, char *string) {
    if (s == NULL || string == NULL) {
        return s;
    }
    char c;
    int i = 0;
    while ((c = string[i])) {
        string_append(s, c);
        i++;
    }
    return s;
}

tString* string_push_int(tString *s, int i) {
    if (s == NULL) {
        return NULL;
    }
    char buf[25];
    sprintf(buf, "%d", i);
    return string_push(s, buf);
}

