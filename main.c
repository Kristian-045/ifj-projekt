#include "scanner.h"
#include "parser.h"
#include <stdio.h>
#include "semantic_analysis.h"
#include "codegenerator.h"
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

/**
 * @file main.c
 * @brief Main file for the IFJ project.
 *
 * @authors
 * - Kristián Kaleta (xkaletk00)
 * - Marko Poľanský (xpolan12)
 * - Silvia Šlachtovská (xslachs00)
 * - Peter Hu_hu_nady (xhunadp00)
 *
 * @date 2024
 */
int main() {
    // Open the standard input for reading
    FILE *file = stdin;

    // Check if the input file (stdin) was successfully opened
    if (file == NULL) {
        fprintf(stderr, "Failed to access stdin");
        return 99;
    }

    // Start parsing the input, constructing the abstract syntax tree (AST)
    set_source_file(file);

    NodePtr node = parser();
    semantic_parse(node);
    CodeGenerator *cg = cg_init();  // code generator
    cg_generate_header();
    generate_block(cg, node);
    gen_built_in_fun();
    
    free_tree(node);

    return 0;
}
