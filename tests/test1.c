#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <direct.h>
#define MAX_TESTS 100

// ANSI escape codes for colors
#define COLOR_GREEN "\033[0;32m"
#define COLOR_RED "\033[0;31m"
#define COLOR_RESET "\033[0m"

typedef struct {
    const char *test_case;
    int expected_exit_code;
} Test;


void rewrite_test_zig(const char *filename, const char *test_case) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Failed to open file for writing");
        exit(EXIT_FAILURE);
    }

    // Write the single test case into test.zig
    fprintf(file, "%s\n", test_case);
    fclose(file);
}

int run_test_parser(const char *output_file) {
    // Redirect the output of the test_parser program to a file

    char command[256];
    snprintf(command, sizeof(command), "../ifj_projekt test.zig > %s", output_file); // Adjust the path as needed
    int exit_code = system(command);
    return WEXITSTATUS(exit_code);
}

void cleanup(const char *filename) {
    // Remove the test.zig file
    if (remove(filename) != 0) {
        fprintf(stderr, "Failed to delete test.zig");
    }
}

void save_test_output(const char *output_file, int test_num) {
    // Append the test output to a results file
    FILE *results_file = fopen("test_results.txt", "a");
    if (results_file == NULL) {
        fprintf(stderr, "Failed to open results file for appending");
        exit(EXIT_FAILURE);
    }

    // Write a header for the test case
    fprintf(results_file, "----------- Test %d -----------\n", test_num);

    // Read and write the output from the output_file to the results file
    FILE *output = fopen(output_file, "r");
    if (output == NULL) {
        fprintf(stderr, "Failed to open output file for reading");
        fclose(results_file);
        exit(EXIT_FAILURE);
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), output) != NULL) {
        fprintf(results_file, "%s", buffer);
    }

    fclose(output);
    fclose(results_file);
}

int main() {
    // Clear the test results file at the beginning
    FILE *results_file = fopen("test_results.txt", "w");
    if (results_file == NULL) {
        fprintf(stderr, "Failed to open results file for writing");
        exit(EXIT_FAILURE);
    }
    fclose(results_file);
    const char *filename = "test.zig";
    const char *output_file = "output.txt"; // File to capture the output


    char *prolog1 = "const ifj = @import(\"ifj24.zig\");";
    char *prolog2 = "var ifj = @import(\"ifj24.zig\");";
    char *prolog3 = "cont ifj @import(\"ifj24.zig\");";
    char *prolog4 = "cont ifj = import(\"ifj24.zig\");";
    char *prolog5 = "cont ifj = import(\"ifj24.zig\")";
    char *prolog6 = "cont ifj = import(\"ifj24.zig\"";
    char *prolog7 = "cont ifj = import(\"ifj24.zig\";";
    char *prolog8 = "= @import(\"ifj24.zig\");";
    char *prolog9 = "@import(\"ifj24.zig\");";
    char *prolog10 = "const ifj = @import\"ifj24.zig\");";

    char *function1 = "const ifj = @import(\"ifj24.zig\");\n"
                      "pub fn decrement() f64 {\n"
                      "}";
    char *function2 = "const ifj = @import(\"ifj24.zig\");\n"
                      "pub fn decrement(a:[]u8, b: ?i32, c: i32,) ?f64 {\n"
                      "}";
    char *function3 = "const ifj = @import(\"ifj24.zig\");\n"
                      "pub fn decrement(ah:?[]u8, n: ?i32, m: ?f64,) void {\n"
                      "}";
    char *function4 = "const ifj = @import(\"ifj24.zig\");\n"
                      "pub fn decrement(ah:[]u8, n: i32, m: f64) void {\n"
                      "}";
    char *function5 = "const ifj = @import(\"ifj24.zig\");\n"
                      "pub fn decrement(,) void {\n"
                      "}";
    char *function6 = "const ifj = @import(\"ifj24.zig\");\n"
                      "pub fn decrement(a,) void {\n"
                      "}";
    char *function7 = "const ifj = @import(\"ifj24.zig\");\n"
                      "pub fn decrement(a:?,) void {\n"
                      "}";
    char *function8 = "const ifj = @import(\"ifj24.zig\");\n"
                      "pub fn decrement(a:?i32,) void {\n"
                      "}";
    char *function9 = "const ifj = @import(\"ifj24.zig\");\n"
                      "pub fn decrement(a:?i32) void {\n"
                      "}";
    char *function10 = "const ifj = @import(\"ifj24.zig\");\n"
                       "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?void {\n"
                       "}";
    char *function11 = "const ifj = @import(\"ifj24.zig\");\n"
                       "pub fn decrement(a:[]u8, b: ?i32, c: i32,) ?f64 \n"
                       "}";
    char *function12 = "const ifj = @import(\"ifj24.zig\");\n"
                       "pub fn decrement(a:[]u8, b: ?i32, c: i32,) ?f64 {\n"
                       "";

    char *variableDec1 = "const ifj = @import(\"ifj24.zig\");\n"
                         "pub fn decrement(a:[]u8, b: ?i32, c: i32,) ?f64 {\n"
                         "     var a:?i32 = 5;\n"
                         "     var a:i32 = 5;\n"
                         "     var a:?f64 = 5;\n"
                         "     var a:f64 = 5;\n"
                         "     var a:?[]u8 = 5;\n"
                         "     var a:[]u8 = 5;\n"
                         "}";
    char *variableDec2 = "const ifj = @import(\"ifj24.zig\");\n"
                         "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                         "     const a:?i32 = 5;\n"
                         "     const a:i32 = 5;\n"
                         "     const a:?f64 = 5;\n"
                         "     const a:f64 = 5;\n"
                         "     const a:?[]u8 = 5;\n"
                         "     const a:[]u8 = 5;\n"
                         "}";
    char *variableDec3 = "const ifj = @import(\"ifj24.zig\");\n"
                         "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                         "     var a = 5;\n"
                         "}";
    char *variableDec4 = "const ifj = @import(\"ifj24.zig\");\n"
                         "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                         "     const a = 5;\n"
                         "}";
    char *variableDec5 = "const ifj = @import(\"ifj24.zig\");\n"
                         "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                         "     const a:? = 5;\n"
                         "}";
    char *variableDec6 = "const ifj = @import(\"ifj24.zig\");\n"
                         "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                         "     const a: = 5;\n"
                         "}";
    char *variableDec7 = "const ifj = @import(\"ifj24.zig\");\n"
                         "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                         "     var a: = ;\n"
                         "}";
    char *variableDec8 = "const ifj = @import(\"ifj24.zig\");\n"
                         "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                         "     const a = ;\n"
                         "}";
    char *assigment1 = "const ifj = @import(\"ifj24.zig\");\n"
                       "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                       "        a = 6;\n"
                       "        a = 6+5;\n"
                       "        _ = 6;\n"
                       "        a = \"hello\";\n"
                       "}";
    char *assigment2 = "const ifj = @import(\"ifj24.zig\");\n"
                       "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                       "        a = ;\n"
                       "}";

    char *functionCall1 = "const ifj = @import(\"ifj24.zig\");\n"
                          "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                          "        test();\n"
                          "        test(1);\n"
                          "        test(1,);\n"
                          "        test(1,a+b,d*d-e);\n"
                          "        test(1,a+b,d*d-e,);\n"
                          "}";
    char *functionCall2 = "const ifj = @import(\"ifj24.zig\");\n"
                          "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                          "        test(,);\n"
                          "}";
    char *functionCall3 = "const ifj = @import(\"ifj24.zig\");\n"
                          "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                          "        test(a,,);\n"
                          "}";
    char *functionCall4 = "const ifj = @import(\"ifj24.zig\");\n"
                          "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                          "        test(a,,b);\n"
                          "}";
    char *functionCall5 = "const ifj = @import(\"ifj24.zig\");\n"
                          "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                          "        test(a,,b,);\n"
                          "}";
    char *functionCall6 = "const ifj = @import(\"ifj24.zig\");\n"
                          "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                          "        test(a,b,)\n"
                          "}";

    char *ifjCall1 = "const ifj = @import(\"ifj24.zig\");\n"
                     "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                     "        ifj.test();\n"
                     "        ifj.test(1);\n"
                     "        ifj.test(1,);\n"
                     "        ifj.test(1,a+b,d*d-e);\n"
                     "        ifj.test(1,a+b,d*d-e,);\n"
                     "}";
    char *ifjCall2 = "const ifj = @import(\"ifj24.zig\");\n"
                     "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                     "        ifj.test(,);\n"
                     "}";
    char *ifjCall3 = "const ifj = @import(\"ifj24.zig\");\n"
                     "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                     "        ifj.test(a,,);\n"
                     "}";
    char *ifjCall4 = "const ifj = @import(\"ifj24.zig\");\n"
                     "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                     "        ifj.test(a,,b);\n"
                     "}";
    char *ifjCall5 = "const ifj = @import(\"ifj24.zig\");\n"
                     "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                     "        ifj.test(a,,b,);\n"
                     "}";
    char *ifjCall6 = "const ifj = @import(\"ifj24.zig\");\n"
                     "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                     "        ifj.test(a,b,)\n"
                     "}";

    char *return1 = "const ifj = @import(\"ifj24.zig\");\n"
                    "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                    "        return hello;\n"
                    "        return 5+5;\n"
                    "        return ;\n"
                    "}";

    char *if1 = "const ifj = @import(\"ifj24.zig\");\n"
                "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
//                "        if(5+5){}\n"
                "        if(5+5){}else{}\n"
//                "        if(5+5)|variable|{}\n"
                "        if(5+5)|variable|{}else{}\n"
                "}";
    char *if2 = "const ifj = @import(\"ifj24.zig\");\n"
                "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
//                "        if(5+5){"
//                "           var a = 5;"
//                "           return a;"
//                "}\n"
                "        if(5+5){"
                "           var a = 5;"
                "           return a;"
                "}else{"
                "           var a = 5;"
                "           return a;"
                "}\n"
//                "        if(5+5)|variable|{"
//                "           var a = 5;"
//                "           return a;"
//                "}\n"
                "        if(5+5)|variable|{"
                "           var a = 5;"
                "           return a;"
                "}else{"
                "           var a = 5;"
                "           return a;"
                "}\n"
                "}";

    char *if3 = "const ifj = @import(\"ifj24.zig\");\n"
                "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                "        if(5+5)variable|{}\n"
                "}";
    char *if4 = "const ifj = @import(\"ifj24.zig\");\n"
                "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                "        if(5+5)|variable{}\n"
                "}";
    char *if5 = "const ifj = @import(\"ifj24.zig\");\n"
                "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                "        if(5+5)variable{}\n"
                "}";
    char *if6 = "const ifj = @import(\"ifj24.zig\");\n"
                "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                "        if(){}\n"
                "}";
    char *if7 = "const ifj = @import(\"ifj24.zig\");\n"
                "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                "        if()}\n"
                "}";
    char *if8 = "const ifj = @import(\"ifj24.zig\");\n"
                "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                "        if(){\n"
                "}";

    char *if9 = "const ifj = @import(\"ifj24.zig\");\n"
                "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                "        ifa+a){}\n"
                "}";
    char *if10 = "const ifj = @import(\"ifj24.zig\");\n"
                 "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                 "        if(a+a)){}\n"
                 "}";
    char *if11 = "const ifj = @import(\"ifj24.zig\");\n"
                 "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                 "        if(a+a)|notNull|{}else\n"
                 "}";
    char *if12 = "const ifj = @import(\"ifj24.zig\");\n"
                 "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                 "        if(a+a)|notNull|{}else{\n"
                 "}";
    char *if13 = "const ifj = @import(\"ifj24.zig\");\n"
                 "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                 "        if(a+a)|notNull|{}else}\n"
                 "}";
    char *if14 = "const ifj = @import(\"ifj24.zig\");\n"
                 "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                 "        if(a+a)|notNull|{}else(){}\n"
                 "}";

    char *while1 = "const ifj = @import(\"ifj24.zig\");\n"
                   "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                   "        while(5+5){}\n"
                   "        while(5+5)|notnull|{}\n"
                   "}";
    char *while2 = "const ifj = @import(\"ifj24.zig\");\n"
                   "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                   "        while(){}\n"
                   "}";
    char *while3 = "const ifj = @import(\"ifj24.zig\");\n"
                   "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                   "        while(5+5)|{}\n"
                   "}";
    char *while4 = "const ifj = @import(\"ifj24.zig\");\n"
                   "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                   "        while(5+5)||{}\n"
                   "}";
    char *while5 = "const ifj = @import(\"ifj24.zig\");\n"
                   "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                   "        while(5+5)notnull{}\n"
                   "}";
    char *while6 = "const ifj = @import(\"ifj24.zig\");\n"
                   "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                   "        while(5+5)}\n"
                   "}";
    char *while7 = "const ifj = @import(\"ifj24.zig\");\n"
                   "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                   "        while(5+5){\n"
                   "}";
    char *while8 = "const ifj = @import(\"ifj24.zig\");\n"
                   "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                   "        while(5+5)\n"
                   "}";
    char *fullCode1 = "const ifj = @import(\"ifj24.zig\");\n"
                      "\n"
                      "pub fn main() void {\n"
                      "ifj.write(\"Zadejte cislo pro vypocet faktorialu\\n\");\n"
                      "const a = ifj.readi32();\n"
                      "if (a) |val| {\n"
                      "if (val < 0) {\n"
                      "ifj.write(\"Faktorial \"); ifj.write(val);\n"
                      "ifj.write(\" nelze spocitat\\n\");\n"
                      "} else {\n"
                      "var d: f64 = ifj.i2f(val);\n"
                      "var vysl: f64 = 1.0;\n"
                      "while (d > 0) {\n"
                      "vysl = vysl * d;\n"
                      "d = d - 1.0;\n"
                      "}\n"
                      "ifj.write(\"Vysledek: \"); ifj.write(vysl); ifj.write(\" = \");\n"
                      "const vysl_i32 = ifj.f2i(vysl);\n"
                      "ifj.write(vysl_i32); ifj.write(\"\\n\");\n"
                      "}\n"
                      "} else { \n"
                      "ifj.write(\"Faktorial pro null nelze spocitat\\n\");\n"
                      "}\n"
                      "}";

    char *fullCode2 = "const ifj = @import(\"ifj24.zig\");\n"
                      "pub fn main() void {\n"
                      "ifj.write(\"Zadejte cislo pro vypocet faktorialu: \");\n"
                      "const inp = ifj.readi32();\n"
                      "if (inp) |INP| {\n"
                      "if (INP < 0) {\n"
                      "ifj.write(\"Faktorial nelze spocitat!\\n\");\n"
                      "} else {\n"
                      "const vysl = factorial(INP);\n"
                      "ifj.write(\"Vysledek: \"); ifj.write(vysl);\n"
                      "}\n"
                      "} else {\n"
                      "ifj.write(\"Chyba pri nacitani celeho cisla!\\n\");\n"
                      "}\n"
                      "}\n"
                      "pub fn decrement(n: i32, m: i32) i32 {\n"
                      "return n - m;\n"
                      "}\n"
                      "pub fn factorial(n: i32) i32 {\n"
                      "var result: i32 = 0 - 1;\n"
                      "if (n < 2) {\n"
                      "result = 1;\n"
                      "} else {\n"
                      "const decremented_n = decrement(n, 1);\n"
                      "const temp_result = factorial(decremented_n);\n"
                      "result = n * temp_result;\n"
                      "}\n"
                      "return result;\n"
                      "}";
    char *fullCode3 = "const ifj = @import(\"ifj24.zig\");\n"
                      "pub fn main() void {\n"
                      "const str1 = ifj.string(\"Toto je text v programu jazyka IFJ24\");\n"
                      "var str2 = ifj.string(\", ktery jeste trochu obohatime\");\n"
                      "str2 = ifj.concat(str1, str2);\n"
                      "ifj. write(str1); ifj . write(\"\\n\");\n"
                      "ifj .write(str2); ifj.write(\"\\n\");\n"
                      "ifj.write(\"Zadejte serazenou posloupnost malych pismen a-h:\\n\");\n"
                      "var newInput = ifj.readstr();\n"
                      "var all: []u8 = ifj.string(\"\");\n"
                      "while (newInput) |inpOK| {\n"
                      "const abcdefgh = ifj.string(\"abcdefgh\");\n"
                      "const strcmpResult = ifj.strcmp(inpOK, abcdefgh);\n"
                      "if (strcmpResult == 0) {\n"
                      "ifj.write(\"Spravne zadano!\\n\");\n"
                      "ifj.write(all);\n"
                      "newInput = null;\n"
                      "} else {\n"
                      "ifj.write(\"Spatne zadana posloupnost, zkuste znovu:\\n\");\n"
                      "all = ifj.concat(all, inpOK); \n"
                      "newInput = ifj.readstr();\n"
                      "}\n"
                      "}\n"
                      "}";

    char *expression1 = "const ifj = @import(\"ifj24.zig\");\n"
                       "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                       "        a = 5;\n"
                       "        a = id;\n"
                       "        a = ifj.write(5+3);\n"
                       "        a = ifj.write(a+b);\n"
                       "        a = ifj.write(3)+2;\n"
                       "        a = ifj.write(a)+b;\n"
                       "        a = 2+ifj.write(1,);\n"
                       "        a = 2+ifj.write(a,);\n"
                       "        a = 2+(ifj.write(1,)+1)*2;\n"
                       "        a = d+(ifj.write(a,)+b)*c;\n"
                       "        a = 2+(5+1)*5;\n"
                       "        a = a+(b+v)*d;\n"
                       "        a = b == 2;\n"
                       "        a = fun(a) == 3;\n"
                       "        a = fun(a) == a;\n"
                       "        a = fun(a) == ifj.daco(5);\n"
                       "        a = fun(a) == null;\n"
                       "        a = fun(a) < null;\n"
                       "}";
    char *expression2 = "const ifj = @import(\"ifj24.zig\");\n"
                        "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                        "        a = 5\n"
                        "}";
    char *expression3 = "const ifj = @import(\"ifj24.zig\");\n"
                        "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                        "        a = -5;\n"
                        "}";
    char *expression4 = "const ifj = @import(\"ifj24.zig\");\n"
                        "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                        "        a = funA(4+3;\n"
                        "}";
    char *expression5 = "const ifj = @import(\"ifj24.zig\");\n"
                        "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                        "        a = (4+3;\n"
                        "}";
    char *expression6 = "const ifj = @import(\"ifj24.zig\");\n"
                        "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                        "        a = 4+;\n"
                        "}";
    char *expression7 = "const ifj = @import(\"ifj24.zig\");\n"
                        "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                        "        a = 4*;\n"
                        "}";
    char *expression8 = "const ifj = @import(\"ifj24.zig\");\n"
                        "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                        "        a = funA(;\n"
                        "}";
    char *expression9 = "const ifj = @import(\"ifj24.zig\");\n"
                        "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                        "        a = -5+3;\n"
                        "}";
    char *expression10 = "const ifj = @import(\"ifj24.zig\");\n"
                        "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                        "        a = 5+(-2);\n"
                        "}";

    char *expression11 = "const ifj = @import(\"ifj24.zig\");\n"
                         "pub fn decrement(ah:[]u8, n: ?i32, m: i32,) ?f64 {\n"
                         "        a = 5+(2));\n"
                         "}";

    // Define your test cases and their expected exit codes
    Test tests[MAX_TESTS] = {
            //TESTING PROLOG
            {prolog1,       0},//1
            {prolog2,       2},//2
            {prolog3,       2},//3
            {prolog4,       2},//4
            {prolog5,       2},//5
            {prolog6,       2},//6
            {prolog7,       2},//7
            {prolog8,       2},//8
            {prolog9,       2},//9
            {prolog10,      2},//10

            // TESTING FUNCTION DECLARATION
            {function1,     0},
            {function2,     0},
            {function3,     0},
            {function4,     0},
            {function5,     2},
            {function6,     2},
            {function7,     2},
            {function8,     0},
            {function9,     0},
            {function10,    2},
            {function11,    2},
            {function12,    2},//22

            // TESTING VARIABLE DECLARATION
            {variableDec1,  0},
            {variableDec2,  0},
            {variableDec3,  0},
            {variableDec4,  0},
            {variableDec5,  2},
            {variableDec6,  2},
            {variableDec7,  2},
            {variableDec8,  2},//30

            // TESTING ASSIGMENT
            {assigment1,    0},
            {assigment2,    2},//32,

            // TESTING FUNCTION CALL
            {functionCall1, 0},
            {functionCall2, 2},
            {functionCall3, 2},
            {functionCall4, 2},
            {functionCall5, 2},
            {functionCall6, 2},//38

            // TESTING IFJ CALL
            {ifjCall1,      0},
            {ifjCall2,      2},
            {ifjCall3,      2},
            {ifjCall4,      2},
            {ifjCall5,      2},
            {ifjCall6,      2},//44


            // Testing return call;
            {return1,       0},//45

            // TESTING IF
            {if1,           0},//46
            {if2,           0},//47
            {if3,           2},
            {if4,           2},
            {if5,           2},
            {if6,           2},
            {if7,           2},
            {if8,           2},//53
            {if9,           2},
            {if10,          2},
            {if11,          2},
            {if12,          2},//57
            {if13,          2},//58
            {if14,          2},//59

            // TESTING WHILE
            {while1,        0},
            {while2,        2},
            {while3,        2},
            {while4,        2},
            {while5,        2},
            {while6,        2},
            {while7,        2},
            {while8,        2},//67

            //TESTING FULL CODE
            {fullCode1,     0},
            {fullCode2,     0},
            {fullCode3,     0},//70

            //TESTING EXPRESSION
            {expression1,     0},
            {expression2,     2},
            {expression3,     2},
            {expression4,     2},
            {expression5,     2},
            {expression6,     2},
            {expression7,     2},
            {expression8,     2},
            {expression9,     2},
            {expression10,     2},//80
            {expression11,     2},

            // Add more tests as needed
    };
    int num_tests = 81; // Update this to reflect the number of tests defined
    int passed_tests = 0;
    // Step through each test case
    for (int i = 0; i < num_tests; i++) {
        // Step 1: Rewrite the test file with the current test case
        rewrite_test_zig(filename, tests[i].test_case);

        // Step 2: Run the parser and redirect output


        // Step 2: Run the parser and redirect output
        int exit_code = run_test_parser(output_file);

        // Save the output to the results file
        save_test_output(output_file, i + 1);

        // Compare the actual exit code with the expected exit code
        if (exit_code != tests[i].expected_exit_code) {
            printf(COLOR_RED "Test %d failed.\n" COLOR_RESET, i + 1); // Print in red
        } else {
            printf(COLOR_GREEN "Test %d passed.\n" COLOR_RESET, i + 1); // Print in green
            passed_tests++;
        }
    }

    // Step 3: Cleanup
//    cleanup(filename);
    remove(output_file);
    printf("Result %d/%d\n", passed_tests, num_tests);
    return 0; // You could also return a different exit code based on the tests' results
}