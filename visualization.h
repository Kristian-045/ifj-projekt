#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "scanner.h"
#include "parser.h"

#define MAX_HEIGHT 1000
#define EMPTY_CELL "   "

// Function declarations
void getFormattedValue(NodePtr node, char *result);
int getHeight(NodePtr root);
void printBinaryTree(NodePtr root);
