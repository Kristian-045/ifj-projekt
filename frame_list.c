#include "frame_list.h"
#include <stdio.h>

FList* frame_list_create(){
    FList* list = calloc(1,sizeof (FList));
    if(list == NULL){
        fprintf(stderr, "frame_list.c frame_list_create: Chyba pri alokacii");
        exit(99);
    }
    list->current = NULL;
    list->first = NULL;
    return list;
}
void frame_list_add(FList* list, TData* data){
    if(list == NULL || data == NULL){
        return;
    }

    LData* ldata = calloc(1,sizeof (LData));
    if(ldata == NULL){
        fprintf(stderr, "frame_list.c frame_list_add: Chyba pri alokacii");
        exit(99);
    }
   ldata->frame = data;
   ldata->next = list->first;
   list->first = ldata;
}
void frame_list_next(FList* list){
    if (list == NULL)
        return;
    list->current = list->current->next;
}
void frame_delete(FList* list){
    if (list == NULL)
        return;

}
void frame_delete_all(FList* list){
   if (list == NULL)
       return;
   LData* next = list->first;
   LData* to_delete;
    while (next != NULL){
        to_delete = next;
        next = next->next;
        free(to_delete);
    }
    list->first = NULL;
    list->current = NULL;
}
void frame_free(FList* list){
    frame_delete_all(list);
    free(list);
}
void frame_first(FList* list){
    list->current = list->first;
}

LData* frame_list_get(FList* list){
    return list->current;
}