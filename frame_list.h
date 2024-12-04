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

// LData: A structure representing a node in the linked list, containing a frame and a pointer to the next node
typedef struct ldata{
    struct ldata* next;
    TData* frame;
}LData;

// FList: A structure representing the list itself
typedef struct flist{
    LData* first;
    LData* current;
}FList;

/**
 * @brief Creates and initializes an empty frame list.
 *
 * Allocates memory for the frame list structure and initializes both `first` and `current` to NULL, indicating
 * that the list is initially empty.
 *
 * @return A pointer to the newly created frame list.
 */
FList* frame_list_create();
/**
 * @brief Adds a new frame to the list.
 *
 * This function creates a new node (`LData`) containing the provided `TData` frame and adds it to the list.
 * The node is added at the end of the list, and the `current` pointer may be updated accordingly.
 *
 * @param list The frame list to which the new frame will be added.
 * @param data A pointer to the frame (`TData`) to add to the list.
 */
void frame_list_add(FList* list, TData* data);

/**
 * @brief Retrieves the current frame from the list without removing it.
 *
 * This function allows access to the current frame in the list. It returns the node that contains the frame.
 *
 * @param list The frame list to retrieve the current frame from.
 * @return A pointer to the current frame (`LData`), or NULL if the list is empty.
 */
LData* frame_list_get(FList* list);

/**
 * @brief Moves the list's current pointer to the next frame.
 *
 * This function updates the `current` pointer to the next frame in the list. This is useful for traversing the list.
 *
 * @param list The frame list to move to the next frame in the list.
 */
void frame_list_next(FList* list);

/**
 * @brief Deletes all frames in the list and frees their memory.
 *
 * This function walks through the list, frees all frames, and deletes all nodes in the list.
 * It ensures that all memory associated with the list is properly freed.
 *
 * @param list The frame list to delete. The list structure itself will be freed, but not the frames themselves.
 */

void frame_delete_all(FList* list);

/**
 * @brief Frees the frame list structure with contained frames.
 *
 * This function only frees the memory allocated for the frame list (`FList`), not the frames (`TData`) it contains.
 * If the frames themselves need to be freed, those should be handled separately.
 *
 * @param list The frame list to free.
 */
void frame_free(FList* list);

/**
 * @brief Resets the current pointer to the first frame in the list.
 *
 * This function sets the `current` pointer back to the first frame in the list, effectively "resetting" the list.
 *
 * @param list The frame list to reset.
 */
void frame_first(FList* list);

#endif //IFJ_PROJEKT_FRAME_STACK_H
