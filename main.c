#include "scanner.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

int main() {

    FILE *file = fopen("idk.zig", "r");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }

    setSourceFile(file);
    parser();
    return 0;
}
