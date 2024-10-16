#include "scanner.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

int main() {
    FILE* file = fopen("idk.zig", "r");

    getToken(file);

    return 0;
}
