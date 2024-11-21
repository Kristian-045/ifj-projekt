#include "scanner.h"
#include "parser.h"
#include <stdio.h>
#include "semantic_analysis.h"
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

    const char *filename = "./idk.zig";
    // Check if a filename was passed as an argument
    if (argc > 1) {
        filename = argv[1]; // Use the provided filename
    }
//    printf("filename: %s\n",filename);

    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Failed to open file");
        return 1;
    }

    setSourceFile(file);
    NodePtr node = parser();
    //semantic_parse(node);
    freeTree(node);


    return 0;
}