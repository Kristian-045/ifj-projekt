/**
 * @file frame_list.h
 * @brief Frame List for the IFJ project.
 *
 * @author Marko Poľanský (xpolan12)
 *
 * @date 2024
 */

#include <stdlib.h>
#include "symtable.h"
#ifndef IFJ_PROJEKT_FRAME_STACK_H
#define IFJ_PROJEKT_FRAME_STACK_H

#define STACK_SIZE

typedef struct ldata{
    struct ldata* next;
    TData* frame;
}LData;

typedef struct flist{
    LData* first;
    LData* current;
}FList;

FList* frame_list_create();
void frame_list_add(FList* list, TData* data);
LData* frame_list_get(FList* list);
void frame_list_next(FList* list);
void frame_delete_all(FList* list);
void frame_free(FList* list);
void frame_first(FList* list);

#endif //IFJ_PROJEKT_FRAME_STACK_H
