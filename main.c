#include "scanner.h"
#include "parser.h"
#include <stdio.h>
#include "semantic_analysis.h"
#include "codegeneration.h"
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

int main() {

    FILE *file = stdin;

    if (file == NULL) {
        fprintf(stderr, "Failed to access stdin");
        return 1;
    }

    set_source_file(file);
    NodePtr node = parser();
    //semantic_parse(node);
    CodeGenerator *cg = cg_init();  // code generator
    cg_generate_header();
    generate_block(cg, node);
    
    freeTree(node);

    return 0;
}
