#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "scanner.h"
#include "parser.h"

#define MAX_HEIGHT 1000
#define EMPTY_CELL "   "

// Function to get the formatted string representation of a node's value
void getFormattedValue(NodePtr node, char *result) {
    if (node == NULL) {
        strcpy(result, EMPTY_CELL);
        return;
    }

    switch (node->data_type) {
        case INT:
            sprintf(result, "%03d", node->data.int_val % 1000);
            break;
        case FLOAT: {
            char temp[20];
            sprintf(temp, "%.3f", node->data.float_val);
            strncpy(result, temp, 3);
            result[3] = '\0';
            break;
        }
        case STRING:
            if (node->data.string_val) {
                strncpy(result, node->data.string_val, 3);
                result[3] = '\0';
                // Pad with spaces if string is shorter than 3 chars
                while (strlen(result) < 3) {
                    strcat(result, " ");
                }
            } else {
                strcpy(result, "   ");
            }
            break;
        case ONLY_KEYWORD:
            sprintf(result, "T%02d", (int)node->keyword);
            break;
    }
}

// Function to get the height of the tree
int getHeight(NodePtr root) {
    if (root == NULL) return 0;
    int leftHeight = getHeight(root->left);
    int rightHeight = getHeight(root->right);
    return (leftHeight > rightHeight ? leftHeight : rightHeight) + 1;
}

// Function to print the tree horizontally
void printBinaryTree(NodePtr root) {
    if (root == NULL) return;

    int height = getHeight(root);
    int width = (1 << height) - 1;

    // Create a 2D array to store the tree representation
    char ***grid = (char ***)malloc(height * sizeof(char **));
    for (int i = 0; i < height; i++) {
        grid[i] = (char **)malloc(width * sizeof(char *));
        for (int j = 0; j < width; j++) {
            grid[i][j] = (char *)malloc(4 * sizeof(char));
            strcpy(grid[i][j], EMPTY_CELL);
        }
    }

    // Queue for level-order traversal
    NodePtr *queue = (NodePtr *)malloc(1000 * sizeof(NodePtr));
    int *col_positions = (int *)malloc(1000 * sizeof(int));
    int *levels = (int *)malloc(1000 * sizeof(int));
    int front = 0, rear = 0;

    // Add root to queue
    queue[rear] = root;
    col_positions[rear] = width / 2;
    levels[rear] = 0;
    rear++;

    // Process nodes
    while (front < rear) {
        NodePtr current = queue[front];
        int col = col_positions[front];
        int level = levels[front];
        front++;

        if (current) {
            char value[4];
            getFormattedValue(current, value);
            strcpy(grid[level][col], value);

            if (current->left) {
                queue[rear] = current->left;
                col_positions[rear] = col - (1 << (height - level - 2));
                levels[rear] = level + 1;
                rear++;
            }
            if (current->right) {
                queue[rear] = current->right;
                col_positions[rear] = col + (1 << (height - level - 2));
                levels[rear] = level + 1;
                rear++;
            }
        }
    }

    // Print the grid
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            printf("%s ", grid[i][j]);
        }
        printf("\n");
    }

    // Free allocated memory
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            free(grid[i][j]);
        }
        free(grid[i]);
    }
    free(grid);
    free(queue);
    free(col_positions);
    free(levels);
}

int test() {
    // tests
    NodePtr root = malloc(sizeof(struct Node));
    root->data_type = INT;
    root->data.int_val = 123;

    root->left = malloc(sizeof(struct Node));
    root->left->data_type = STRING;
    root->left->data.string_val = "ABCf";

    root->right = malloc(sizeof(struct Node));
    root->right->data_type = ONLY_KEYWORD;
    root->right->keyword = T_EQUALSIGN;

    root->left->left = malloc(sizeof(struct Node));
    root->left->left->data_type = ONLY_KEYWORD;
    root->left->left->keyword = T_FN;

    root->left->left->left = malloc(sizeof(struct Node));
    root->left->left->left->data_type = ONLY_KEYWORD;
    root->left->left->left->keyword = T_FN;

    printBinaryTree(root);
    return 0;
}