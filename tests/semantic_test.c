#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#define MAX_TESTS 150

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
    snprintf(command, sizeof(command), " ../ifj_projekt < test.zig > %s", output_file); // Adjust the path as needed
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

    char *main_missing = "const ifj = @import(\"ifj24.zig\");\n"
                         "pub fn decrement() void  {\n"
                         "}";

    char *main_is_not_void = "const ifj = @import(\"ifj24.zig\");\n"
                         "pub fn main() i32  {\n"
                         "}";

    char *main_contains_params = "const ifj = @import(\"ifj24.zig\");\n"
                             "pub fn main(a :i32) i32  {\n"
                             "}";
    char *redefinition_var_in_same_block = "const ifj = @import(\"ifj24.zig\");\n"
                                       "pub fn main() void {\n"
                                       "    var count: i32 = 5;\n"
                                       "    var count: i32 = 10;"
                                 "}";
    char *redefinition_const_in_same_block = "const ifj = @import(\"ifj24.zig\");\n"
                                           "pub fn main() void {\n"
                                           "    const count: i32 = 5;\n"
                                           "    const count: i32 = 10;"
                                           "}";
    char *redefinition_const_no_variable_type_in_same_block = "const ifj = @import(\"ifj24.zig\");\n"
                                             "pub fn main() void {\n"
                                             "    const count: i32 = 5;\n"
                                             "    count = 10;"
                                             "}";
    char *redefinition_of_fn = "const ifj = @import(\"ifj24.zig\");\n"
                                           "pub fn main() void {\n"
                                           "}"
                                           "pub fn soucet(a: i32, b: i32) i32 {\n"
                                        "    return a + b;\n"
                                             "}\n"
                                       "pub fn soucet(a: f64, b: f64) f64 {\n"
                                       "    return a + b;\n"
                                       "}";
    char *redefinition_of_main = "const ifj = @import(\"ifj24.zig\");\n"
                               "pub fn main() void {\n"
                               "}"
                                 "pub fn main() void {\n"
                                 "}";
    char *redefinition_of_fn_params = "const ifj = @import(\"ifj24.zig\");\n"
                                 "pub fn main() void {\n"
                                 "}"
                                 "pub fn add(a :i32) void {\n"
                                 "  a = 3;"
                                 "}";
    char *redefinition_of_2th_fn_params = "const ifj = @import(\"ifj24.zig\");\n"
                                      "pub fn main() void {\n"
                                      "}"
                                      "pub fn add(a :i32, b :i32) void {\n"
                                      "b = 3;"
                                      "}";
    char *redefinition_of_fn_params2 = "const ifj = @import(\"ifj24.zig\");\n"
                                          "pub fn main() void {\n"
                                          "}"
                                          "pub fn add(a :i32, b :i32) void {\n"
                                          "var b = 3;"
                                          "}";
    char *redefinition_with_var_of_fn_param = "const ifj = @import(\"ifj24.zig\");\n"
                                          "pub fn main() void {\n"
                                          "}"
                                          "pub fn add(a :i32) void {\n"
                                          "var a = 3;"
                                          "}";
    char *redefinition_with_const_of_fn_param = "const ifj = @import(\"ifj24.zig\");\n"
                                              "pub fn main() void {\n"
                                              "}"
                                              "pub fn add(a :i32) void {\n"
                                              "const a = 3;"
                                              "}";
    char *no_return_value_when_expected = "const ifj = @import(\"ifj24.zig\");\n"
                                                "pub fn main() void {\n"
                                                "}"
                                                "pub fn add(a :i32) i32 {\n"
                                                "const s = 3;"
                                                "}";
    char *return_with_wrong_type = "const ifj = @import(\"ifj24.zig\");\n"
                                          "pub fn main() void {\n"
                                          "}"
                                          "pub fn add(a :i32) i32 {\n"
                                          "const s = 3.3;\n"
                                          "return s;"
                                          "}";

    char *returns_none = "const ifj = @import(\"ifj24.zig\");\n"
                                   "pub fn main() void {\n"
                                   "}"
                                   "pub fn add(a :i32) i32 {\n"
                                   "const s = 3.3;\n"
                                   "return ;"
                                   "}";
    char *returns_value_when_no_value_expected = "const ifj = @import(\"ifj24.zig\");\n"
                         "pub fn main() void {\n"
                         "}"
                         "pub fn add(a :i32) void {\n"
                         "const s = 3.3;\n"
                         "return s;"
                         "}";

    char *argument_non_used = "const ifj = @import(\"ifj24.zig\");\n"
                                                 "pub fn main() void {\n"
                                                 "}"
                                                 "pub fn add(a :i32) void {\n"
                                                 "const s = 3.3;\n"
                                                 "}";
    char *expression1 = "const ifj = @import(\"ifj24.zig\");\n"
                              "pub fn main() void {\n"
                              "var a = \"ASD\"; \n"
                              "}";
    char *expression2 = "const ifj = @import(\"ifj24.zig\");\n"
                        "pub fn main() void {\n"
                        "var a = 3;"
                        "var b = 3.3;"
                        "a = a + b;"
                        "}";
    char *expression3 = "const ifj = @import(\"ifj24.zig\");\n"
                        "pub fn main() void {\n"
                        "}"
                        "pub fn add(a :i32) void {\n"
                        "const s = 3.3 + a;\n"
                        "}";

    char *expression4 = "const ifj = @import(\"ifj24.zig\");\n"
                        "pub fn main() void {\n"
                        "var x: i32 = 10;\n"
                        "x = 3.14;\n"
                        "}";

    char *expression5 = "const ifj = @import(\"ifj24.zig\");\n"
                        "pub fn main() void {\n"
                        "}"
                        "pub fn add(a :i32) void {\n"
                        "const s :f64 =  a;\n"
                        "}";
    char *while_expression = "const ifj = @import(\"ifj24.zig\");\n"
                        "pub fn main() void {\n"
                        "while(1.1){"
                        "}"
                        "}";
    char *while_expression2= "const ifj = @import(\"ifj24.zig\");\n"
                             "pub fn main() void {\n"
                             "while(1){"
                             "}"
                             "}";
    char *while_expression3= "const ifj = @import(\"ifj24.zig\");\n"
                             "pub fn main() void {\n"
                             "}"
                             "pub fn add(a :i32) void {\n"
                             "while(3.3 > a){"
                             "}"
                             "}";
    char *while_expression4= "const ifj = @import(\"ifj24.zig\");\n"
                             "pub fn main() void {\n"
                             "}"
                             "pub fn add(a :i32) void {\n"
                             "var c = 3.3;"
                             "while(c > a){"
                             "}"
                             "}";
    char *while_expression5= "const ifj = @import(\"ifj24.zig\");\n"
                             "pub fn main() void {\n"
                             "}"
                             "pub fn add(a :i32) void {\n"
                             "var c: ?f64= 3.3;"
                             "while(c > 3.3){"
                             "}"
                             "}";
    char *while_expression6= "const ifj = @import(\"ifj24.zig\");\n"
                             "pub fn main() void {\n"
                             "}"
                             "pub fn add(a :i32) void {\n"
                             "var c: ?i32= 3;"
                             "while(c > 3){"
                             "}"
                             "}";
    char *while_expression7= "const ifj = @import(\"ifj24.zig\");\n"
                             "pub fn main() void {\n"
                             "}"
                             "pub fn add(a :i32) void {\n"
                             "var c: ?i32= 3;"
                             "var d: ?i32= 3;"
                             "while(c > d){"
                             "}"
                             "}";
    char *while_expression8= "const ifj = @import(\"ifj24.zig\");\n"
                             "pub fn main() void {\n"
                             "const c: ?i32= 3;"
                             "const d: i32= 3;"
                             "while(c == d){"
                             "}"
                             "}";
    char *while_expression9= "const ifj = @import(\"ifj24.zig\");\n"
                             "pub fn main() void {\n"
                             "var c: ?i32= 3;"
                             "var d: ?i32= 3;"
                             "while(c == d){"
                             "}"
                             "}";

    char *while_expression10= "const ifj = @import(\"ifj24.zig\");\n"
                             "pub fn main() void {\n"
                              "const y: i32 = 11;"
                              "const w: i32 = 3 * 4 + 23 + y;"
                              "while (w == 20){}"
                              "while (y + 23 * 3 == 31.0 + 2.0) {}"
                              "}";
    char *while_expression11= "const ifj = @import(\"ifj24.zig\");\n"
                              "pub fn main() void {\n"
                              "}"
                              "pub fn asd(x :i32) void {\n"
                              "while(x == 9.2) {}"
                              "}";
    char *redefinition_in_if_block= "const ifj = @import(\"ifj24.zig\");\n"
                              "pub fn main() void {\n"
                              "const a = 5;"
                              "if(a > 5){a = 4;}else{}"
                              "}";

   char *redefinition_in_else_block= "const ifj = @import(\"ifj24.zig\");\n"
                                     "pub fn main() void{\n"
                                     "    const a = 8;\n"
                                     "    if(a < 5){}\n"
                                     "    else{\n"
                                     "        a = 4;\n"
                                     "    }\n"
                                     "}\n";


    char *redefinition_in_3_nested_block= "const ifj = @import(\"ifj24.zig\");\n"
                                          "    pub fn main() void{\n"
                                          "    }\n"
                                          "    pub fn add() void{\n"
                                          "        const a = 8;\n"
                                          "        if(1 == 0){\n"
                                          "            if(1 == 0){\n"
                                          "                while (1 > 2){\n"
                                          "                    a = 8;\n"
                                          "                }\n"
                                          "            }else{\n"
                                          "            }\n"
                                          "        }else{\n"
                                          "        }\n"
                                          "    }";

    char *redefinition_in_1_3_nested_block= "const ifj = @import(\"ifj24.zig\");\n"
                                          "    pub fn main() void{\n"
                                          "    }\n"
                                          "    pub fn add() void{\n"
                                          "        if(1 == 0){\n"
                                          "         const a = 8;\n"
                                          "            if(1 == 0){\n"
                                          "                while (1 > 2){\n"
                                          "                    a = 8;\n"
                                          "                }\n"
                                          "            }else{\n"
                                          "            }\n"
                                          "        }else{\n"
                                          "        }\n"
                                          "    }";
    char *accepted_redefinition_in_3_1_nested_block= "const ifj = @import(\"ifj24.zig\");\n"
                                            "    pub fn main() void{\n"
                                            "    }\n"
                                            "    pub fn add() void{\n"
                                            "        if(1 == 0){\n"
                                            "            if(1 == 0){\n"
                                            "                while (1 > 2){\n"
                                            "                   const a = 8;\n"
                                            "                   _ = a;\n"
                                            "                   }\n"
                                            "            }else{\n"
                                            "            }\n"
                                            "        }else{\n"
                                            "     const a = 8;\n"
                                            "           _ = a;\n"
                                            "        }\n"
                                            "    }";
    char *accepted_redefinition_nested_block="const ifj = @import(\"ifj24.zig\");\n"
                                             "pub fn main() void{\n"
                                             "}\n"
                                             "pub fn add() void{\n"
                                             "    if(1 == 0){ \n"
                                             "        if(1 == 0){ \n"
                                             "        while (1 > 2){ \n"
                                             "            const a = 8;\n"
                                             "              _ = a;\n"
                                             "    }\n"
                                             "    }else{ //212\n"
                                             "        const a = 8;\n"
                                             "          _ = a;\n"
                                             "    }\n"
                                             "    }else{ //22\n"
                                             "       const a = 8;\n"
                                             "          _ = a;\n"
                                             "    }\n"
                                             "}";
    char *multiple_redefinition_nested_block="const ifj = @import(\"ifj24.zig\");\n"
                                             "pub fn main() void{\n"
                                             "}\n"
                                             "pub fn add() void{\n"
                                             "    if(1 == 0){ \n"
                                             " const a = 8;\n"
                                             "        if(1 == 0){ \n"
                                             "        while (1 > 2){ \n"
                                             "            const a = 8;\n"
                                             "    }\n"
                                             "    }else{ //212\n"
                                             "        const a = 8;\n"
                                             "    }\n"
                                             "    }else{ //22\n"
                                             "       const a = 8;\n"
                                             "    }\n"
                                             "}";

    char *not_defined_in_nested="const ifj = @import(\"ifj24.zig\");\n"
                                             "pub fn main() void{\n"
                                             "}\n"
                                             "pub fn add() void{\n"
                                             "    if(1 == 0){ \n"
                                             "        if(1 == 0){ \n"
                                             "        while (1 > 2){ \n"
                                             "            const a = 8;\n"
                                             "    }\n"
                                             "    }else{ //212\n"
                                             "    }\n"
                                             "    }else{ //22\n"
                                             "       a = 8;\n"
                                             "    }\n"
                                             "}";

    char *redefinition_of_param_in_nested="const ifj = @import(\"ifj24.zig\");\n"
                                "pub fn main() void{\n"
                                "}\n"
                                "pub fn add(a :i32) void{\n"
                                "    if(1 == 0){ \n"
                                "        if(1 == 0){ \n"
                                "        while (1 > 2){ \n"
                                "            const a = 8;\n"
                                "    }\n"
                                "    }else{ //212\n"
                                "    }\n"
                                "    }else{ //22\n"
                                "    }\n"
                                "}";
    char *redefinition_of_param_in_nested2="const ifj = @import(\"ifj24.zig\");\n"
                                          "pub fn main() void{\n"
                                          "}\n"
                                          "pub fn add(a :i32) void{\n"
                                          "    if(1 == 0){ \n"
                                          "        if(1 == 0){ \n"
                                          "        while (1 > 2){ \n"
                                          "             a = 8;\n"
                                          "    }\n"
                                          "    }else{ //212\n"
                                          "    }\n"
                                          "    }else{ //22\n"
                                          "    }\n"


                     "}";

    char *if_with_null_expected_bool_given="const ifj = @import(\"ifj24.zig\");\n"
                                           "pub fn main() void{\n"
                                           "     var a = 5;\n"
                                           "     if(a > 3)|A|{\n"
                                           "          \n"
                                           "     }else{}"
                                           "}\n";
    char *if_with_not_null_expected_bool_with_null_given="const ifj = @import(\"ifj24.zig\");\n"
                                           "pub fn main() void{\n"
                                           "     var a: ?i32 = 5;\n"
                                           "     if(a > 3){\n"
                                           "          \n"
                                           "     }else{}"
                                           "}\n";

    char *if_with_nullable_var_redefined="const ifj = @import(\"ifj24.zig\");\n"
                                        "pub fn main() void{\n"
                                        "         var a: ?i32 = 5;\n"
                                        "        if(a)|A|{\n"
                                        "         const A = 3;\n"
                                        "       }else{\n"
                                        " }}\n";

    char *if_with_nullable_var_redefined_nested="const ifj = @import(\"ifj24.zig\");\n"
                                                "pub fn main() void{\n"
                                                "         var a: ?i32 = 5;\n"
                                                "        if(a)|A|{\n"
                                                "        if(a)|C|{\n"
                                                "            A = 21;\n"
                                                "        }else{}\n"
                                                "       }else{\n"
                                                " }}\n";

    char *if_with_nullable_var_redefined_nested2="const ifj = @import(\"ifj24.zig\");\n"
                                                 "pub fn main() void{\n"
                                                 "         const a: ?i32 = 5;\n"
                                                 "        if(a)|A|{\n"
                                                 "            const C = 123;\n"
                                                 "        if(a)|C|{\n"
                                                 "        }else{}\n"
                                                 "       }else{\n"
                                                 " }}";
    char *accept_if_with_nullable_var_redefined_in_else="const ifj = @import(\"ifj24.zig\");\n"
                                                 "pub fn main() void{\n"
                                                 "         const a: ?i32 = 5;\n"
                                                 "        if(a)|A|{\n"
                                                 "             _ = A;\n"
                                                 "       }else{\n"
                                                 "       const A = 32;\n"
                                                 "           _ = A;\n"
                                                 " }}";
    char *if_with_nullable_var_redefined_nested3="const ifj = @import(\"ifj24.zig\");\n"
                                                 "pub fn main() void{\n"
                                                 "}\n"
                                                 "pub fn ss(a: i32, b: ?i32) void {\n"
                                                 "    if(b)|asd|{\n"
                                                 "        if(b)|asdd|{\n"
                                                 "        const asd = 4;\n"
                                                 "    }else{}\n"
                                                 "    }else{}\n"
                                                 "}";

    char *while_with_null_expected_bool_given="const ifj = @import(\"ifj24.zig\");\n"
                                           "pub fn main() void{\n"
                                           "     var a = 5;\n"
                                           "     while(a > 3)|A|{\n"
                                           "          \n"
                                           "     }"
                                           "}\n";
    char *while_with_not_null_expected_bool_with_null_given="const ifj = @import(\"ifj24.zig\");\n"
                                                         "pub fn main() void{\n"
                                                         "     var a: ?i32 = 5;\n"
                                                         "     while(a > 3){\n"
                                                         "          \n"
                                                         "     }"
                                                         "}\n";

    char *while_with_nullable_var_redefined="const ifj = @import(\"ifj24.zig\");\n"
                                         "pub fn main() void{\n"
                                         "         var a: ?i32 = 5;\n"
                                         "        while(a)|A|{\n"
                                         "         const A = 3;\n"
                                         "       }}\n";

    char *while_with_nullable_var_redefined_nested="const ifj = @import(\"ifj24.zig\");\n"
                                                "pub fn main() void{\n"
                                                "         var a: ?i32 = 5;\n"
                                                "        while(a)|A|{\n"
                                                "        while(a)|C|{\n"
                                                "            A = 21;\n"
                                                "        }\n"
                                                "       }}\n";

    char *while_with_nullable_var_redefined_nested2="const ifj = @import(\"ifj24.zig\");\n"
                                                 "pub fn main() void{\n"
                                                 "         var a: ?i32 = 5;\n"
                                                 "        while(a)|A|{\n"
                                                 "            var C = 123;\n"
                                                 "        while(a)|C|{\n"
                                                 "        }\n"
                                                 "       }}";

    char *while_with_nullable_var_redefined_nested3="const ifj = @import(\"ifj24.zig\");\n"
                                                 "pub fn main() void{\n"
                                                 "}\n"
                                                 "pub fn ss(a: i32, b: ?i32) void {\n"
                                                 "    while(b)|asd|{\n"
                                                 "        while(b)|asdd|{\n"
                                                 "        const asd = 4;\n"
                                                 "    }}"
                                                 "}";
    char *accept_expression= "const ifj = @import(\"ifj24.zig\");\n"
                              "pub fn main() void{\n"
                             "const X =  ifj.readi32();"
                             " const y: i32 = 11;"
                             "if(X)|x|{if (x == y) {} else {}}"
                             "else{}"
                              "}\n";
    char *accept_expression1= "const ifj = @import(\"ifj24.zig\");\n"
                                                    "pub fn main() void{\n"
                                                    "if (11 == 10)   {} else {}"
                                                    "}\n";
    char *accept_expression2= "const ifj = @import(\"ifj24.zig\");\n"
                       "pub fn main() void{\n"
                       "const X =  ifj.readi32();"
                       "if(X)|x|{"
                       " if (x == 10)    {} else {}"
                       "}else{"
                       "}"
                       "}\n";
    char *accept_expression3= "const ifj = @import(\"ifj24.zig\");\n"
                              "pub fn main() void{\n"
                              "    const X =  ifj.readi32();\n"
                              "    const y =  11;\n"
                              "    if(X) |x|{\n"
                              "            if (x + 1 == y) {} else {}\n"
                              "    }else{}\n"
                              "}\n";
    char *accept_expression4= "const ifj = @import(\"ifj24.zig\");\n"
                              "pub fn main() void{\n"
                              "    const X =  ifj.readi32();\n"
                              "    const y =  11;\n"
                              "    if(X) |x|{\n"
                              "      if (x + y == 9) {} else {}\n"
                              "    }else{}\n"
                              "}\n";
    char *accept_expression5= "const ifj = @import(\"ifj24.zig\");\n"
                              "pub fn main() void{\n"
                              "    const X =  ifj.readi32();\n"
                              "    const y =  11;\n"
                              "    if(X) |x|{\n"
                              "        if (2 * 19 - (10 + y) == 36 - x) {} else {}\n"
                              "    }else{}\n"
                              "}\n";
    char *accept_expression6= "const ifj = @import(\"ifj24.zig\");\n"
                              "pub fn main() void{\n"
                              "    const W =  ifj.readf64();\n"
                              "    if(W) |w|{\n"
                              "       if (10 == 10.0)    {_ = w;} else {}\n"
                              "    }else{}\n"
                              "}\n";

    char *accept_expression7= "const ifj = @import(\"ifj24.zig\");\n"
                              "pub fn main() void{\n"
                              "    const W =  ifj.readf64();\n"
                              "    if(W) |w|{\n"
                              "          if (10 == w){} else {}\n"
                              "    }else{}\n"
                              "}\n";
    char *accept_expression8= "const ifj = @import(\"ifj24.zig\");\n"
                              "pub fn main() void{\n"
                              "    const W =  ifj.readf64();\n"
                              "     const z: f64 = 10.0; "
                              "    if(W) |w|{\n"
                              "      if (10 == z * 2.3) {_ = w;} else {}\n"
                              "    }else{}\n"
                              "}\n";
    char *accept_expression9= "const ifj = @import(\"ifj24.zig\");\n"
                              "pub fn main() void{\n"
                              "    const W =  ifj.readf64();\n"
                              "    const y =  11;\n"
                              "    if(W) |w|{\n"
                              "          if (y == 11.0)     { _ = w;} else {}\n"
                              "    }else{}\n"
                              "}\n";
    char *accept_expression10= "const ifj = @import(\"ifj24.zig\");\n"
                              "pub fn main() void{\n"
                              "    const W =  ifj.readf64();\n"
                              "    const y =  11;\n"
                              "    if(W) |w|{\n"
                              "      if (y == w + 2.9)  { _ = w;} else {}\n"
                              "    }else{}\n"
                              "}\n";

    char *accept_expression11= "const ifj = @import(\"ifj24.zig\");\n"
                               "pub fn main() void{\n"
                               "    const W =  ifj.readf64();\n"
                               "    if(W) |w|{\n"
                               "          if (w == 10)   {} else {}"
                               "    }else{}\n"
                               "}\n";
    char *accept_expression12= "const ifj = @import(\"ifj24.zig\");\n"
                               "pub fn main() void{\n"
                               "    const W =  ifj.readf64();\n"
                               "    if(W) |w|{\n"
                               "           if (w + 2.3 == 10)   {} else {}"
                               "    }else{}\n"
                               "}\n";

    char *accept_expression13=  "const ifj = @import(\"ifj24.zig\");\n"
                                "pub fn main() void{\n"
                                "    const X =  ifj.readi32();\n"

                                "    if(X) |x|{\n"
                                "      if (9.0 == x) {} else {}\n"
                                "    }else{}\n"
                                "}\n";
    char *accept_expression14=  "const ifj = @import(\"ifj24.zig\");\n"
                                "pub fn main() void{\n"
                                "    const X =  ifj.readi32();\n"
                                "     const z: f64 = 10.0; "
                                "    if(X) |x|{\n"
                                "    if (x == z)   {} else {}\n"
                                "    }else{}\n"
                                "}\n";

    char *accept_expression15=  "const ifj = @import(\"ifj24.zig\");\n"
                                "pub fn main() void{\n"
                                "     const X =  ifj.readi32();\n"
                                "     const z: f64 = 10.0; "
                                "    if(X) |x|{\n"
                                "    if (z == x * 2 + 1)   {} else {}\n"
                                "    }else{}\n"
                                "}\n";

    char *check_expression6=  "const ifj = @import(\"ifj24.zig\");\n"
                                "pub fn main() void{\n"
                                "    var x =  ifj.readi32();\n"
                                "     const z: f64 = 10.2; "
                                "    if (z == x * 2 + 1)   {} else {}\n"
                                "}\n";

    char *check_expression_with_null=  "const ifj = @import(\"ifj24.zig\");\n"
                              "pub fn main() void{\n"
                              "    var x =  ifj.readi32();\n"
                              "    if (x > null)|X| {} else {}\n"
                              "}\n";

    char *check_expression_with_null2=  "const ifj = @import(\"ifj24.zig\");\n"
                                       "pub fn main() void{\n"
                                       "    var x =  ifj.readi32();\n"
                                       "    if (x == null)|X| {} else {}\n"
                                       "}\n";
    char *accept_expression_with_null=  "const ifj = @import(\"ifj24.zig\");\n"
                                        "pub fn main() void{\n"
                                        "    const x: ?i32 =  null;\n"
                                        "_ = x;"
                                        "}\n";

    char *accept_expression_with_null2=  "const ifj = @import(\"ifj24.zig\");\n"
                                        "pub fn main() void{\n"
                                        "const test: ?i32 = null;\n"
                                         "const test2: ?i32 = test;"
                                         "_ = test;"
                                         "_ = test2;"
                                        "}\n";

    char *check_string_assignment_init=  "const ifj = @import(\"ifj24.zig\");\n"
                                         "pub fn main() void{\n"
                                         "var test :[]u8 = \"ASDAD\";\n"
                                         "}\n";
    char *check_string_assignment=  "const ifj = @import(\"ifj24.zig\");\n"
                                    "pub fn main() void{\n"
                                    "var test: ?[]u8 = null;"
                                    "test = \"ASDAD\";\n"
                                    "}\n";
    char *check_string_in_expression=  "const ifj = @import(\"ifj24.zig\");\n"
                                    "pub fn main() void{\n"
                                    "var test: ?[]u8 = \"ASD\" + 3\n;"
                                     "}\n";
    char *check_string_in_expression2=  "const ifj = @import(\"ifj24.zig\");\n"
                                       "pub fn main() void{\n"
                                       "var test: ?[]u8 = \"ASD\" * 3\n;"
                                       "}\n";


    char *accept_funexp = "const ifj = @import(\"ifj24.zig\");\n"
                          "pub fn multiply(a: i32, b: i32) i32 {\n"
                          "    return a * b;\n"
                          "}\n"
                          "pub fn add(a: i32, b: i32) i32 {\n"
                          "    return a + b;\n"
                          "}\n"
                          "\n"
                          "pub fn main() void {\n"
                          "    const result = add(multiply(2, 3), multiply(4, 5));\n"
                          "    _ = result; "
                          "}\n";

    char *accept_funexp2 = "const ifj = @import(\"ifj24.zig\");\n"
                           "pub fn square(x: i32) i32 {\n"
                           "    return x * x;\n"
                           "}\n"
                           "pub fn cube(x: i32) i32 {\n"
                           "    return x * x * x;\n"
                           "}\n"
                           "pub fn compute( x: i32) i32 {\n"
                           "    return square(x);\n"
                           "}\n"
                           "pub fn compute2( x: i32) i32 {\n"
                           "    return cube(x);\n"
                           "}\n"
                           "pub fn main() void {\n"
                           "    const squareResult = compute(3);\n"
                           "    const cubeResult = compute2(3);\n"
                           "    _ = squareResult; \n"
                           "    _ = cubeResult; \n"
                           "}";

    char *accept_ifj24_example1 = "const ifj = @import(\"ifj24.zig\");\n"
                                      "pub fn main() void {\n"
                                      "    ifj.write(\"Zadejte cislo pro vypocet faktorialu\\n\");\n"
                                      "    const a = ifj.readi32();\n"
                                      "    if (a) |val| {\n"
                                      "        if (val < 0) {\n"
                                      "            ifj.write(\"Faktorial \"); ifj.write(val);\n"
                                      "            ifj.write(\" nelze spocitat\\n\");\n"
                                      "        } else {\n"
                                      "            var d: f64 = ifj.i2f(val);\n"
                                      "            var vysl: f64 = 1.0;\n"
                                      "            while (d > 0) {\n"
                                      "                vysl = vysl * d;\n"
                                      "                d = d - 1.0;\n"
                                      "            }\n"
                                      "            ifj.write(\"Vysledek: \"); ifj.write(vysl); ifj.write(\" = \");\n"
                                      "            const vysl_i32 = ifj.f2i(vysl);\n"
                                      "            ifj.write(vysl_i32); ifj.write(\"\\n\");\n"
                                      "        }\n"
                                      "    } else { \n"
                                      "        ifj.write(\"Faktorial pro null nelze spocitat\\n\");\n"
                                      "    }\n"
                                      "}";

    char *accept_ifj24_example2 = "const ifj = @import(\"ifj24.zig\");\n"
                                  "pub fn main() void {\n"
                                  "    ifj.write(\"Zadejte cislo pro vypocet faktorialu: \");\n"
                                  "    const inp = ifj.readi32();\n"
                                  "    if (inp) |INP| {\n"
                                  "        if (INP < 0) {\n"
                                  "            ifj.write(\"Faktorial nelze spocitat!\\n\");\n"
                                  "        } else {\n"
                                  "            const vysl = factorial(INP);\n"
                                  "            ifj.write(\"Vysledek: \"); ifj.write(vysl);\n"
                                  "        }\n"
                                  "    } else {\n"
                                  "        ifj.write(\"Chyba pri nacitani celeho cisla!\\n\");\n"
                                  "    }\n"
                                  "}\n"
                                  "pub fn decrement(n: i32, m: i32) i32 {\n"
                                  "    return n - m;\n"
                                  "}\n"
                                  "pub fn factorial(n: i32) i32 {\n"
                                  "    var result: i32 = 0 - 1;\n"
                                  "    if (n < 2) {\n"
                                  "        result = 1;\n"
                                  "    } else {\n"
                                  "        const decremented_n = decrement(n, 1);\n"
                                  "        const temp_result = factorial(decremented_n);\n"
                                  "        result = n * temp_result;\n"
                                  "    }\n"
                                  "    return result;\n"
                                  "}";

    char *accept_ifj24_example3 = "const ifj = @import(\"ifj24.zig\");\n"
                                  "pub fn main() void {\n"
                                  "    const str1 = ifj.string(\"Toto je text v programu jazyka IFJ24\");\n"
                                  "    var str2 = ifj.string(\", ktery jeste trochu obohatime\");\n"
                                  "    str2 = ifj.concat(str1, str2);\n"
                                  "    ifj. write(str1); ifj . write(\"\\n\");\n"
                                  "    ifj .write(str2); ifj.write(\"\\n\");\n"
                                  "    ifj.write(\"Zadejte serazenou posloupnost malych pismen a-h:\\n\");\n"
                                  "    var newInput = ifj.readstr();\n"
                                  "    var all: []u8 = ifj.string(\"\");\n"
                                  "    while (newInput) |inpOK| {\n"
                                  "        const abcdefgh = ifj.string(\"abcdefgh\");\n"
                                  "        const strcmpResult = ifj.strcmp(inpOK, abcdefgh);\n"
                                  "        if (strcmpResult == 0) {\n"
                                  "            ifj.write(\"Spravne zadano!\\n\");\n"
                                  "            ifj.write(all);\n"
                                  "newInput = null; \n"
                                  "} else {\n"
                                  "            ifj.write(\"Spatne zadana posloupnost, zkuste znovu:\\n\");\n"
                                  "            all = ifj.concat(all, inpOK); \n"
                                  "            newInput = ifj.readstr();\n"
                                  "        }\n"
                                  "    }\n"
                                  "}";
    char *check_int_to_string_variable=  "const ifj = @import(\"ifj24.zig\");\n"
                                        "pub fn main() void{\n"
                                        "var test: []u8 = 3;"
                                        "}\n";
    char *check_int_nullable_to_string_variable=  "const ifj = @import(\"ifj24.zig\");\n"
                                                    "pub fn main() void{\n"
                                                    "var c: ?i32 = null;"
                                                    "var test: []u8 = c ;"
                                                    "}\n";
    char *check_float_to_string_variable=  "const ifj = @import(\"ifj24.zig\");\n"
                                         "pub fn main() void{\n"
                                         "var test: []u8 = 3.3;"
                                         "}\n";
    char *check_float_nullable_to_string_variable=  "const ifj = @import(\"ifj24.zig\");\n"
                                           "pub fn main() void{\n"
                                           "var c: ?f64 = null;"
                                           "var test: []u8 = c ;"
                                           "}\n";
    char *check_int_to_string_nullable_variable=  "const ifj = @import(\"ifj24.zig\");\n"
                                         "pub fn main() void{\n"
                                         "var test: ?[]u8 = 3;"
                                         "}\n";
    char *check_int_nullable_to_string_nullable_variable=  "const ifj = @import(\"ifj24.zig\");\n"
                                                  "pub fn main() void{\n"
                                                  "var c: ?i32 = null;"
                                                  "var test: ?[]u8 = c ;"
                                                  "}\n";
    char *check_float_to_string_nullable_variable=  "const ifj = @import(\"ifj24.zig\");\n"
                                           "pub fn main() void{\n"
                                           "var test: ?[]u8 = 3.3;"
                                           "}\n";
    char *check_float_nullable_to_string_nullable_variable=  "const ifj = @import(\"ifj24.zig\");\n"
                                                    "pub fn main() void{\n"
                                                    "var c: ?f64 = null;"
                                                    "var test: ?[]u8 = c ;"
                                                    "}\n";
    char *check_expression_2_unknown_value_variables=  "const ifj = @import(\"ifj24.zig\");\n"
                                                             "pub fn main() void{\n"
                                                             "var c = ifj.readf64();"
                                                             "var d = ifj.readf64();"
                                                             "if(c == d){}else{}"
                                                             "}\n";
    char *check_expression_2_unknown_value_variables2=  "const ifj = @import(\"ifj24.zig\");\n"
                                                       "pub fn main() void{\n"
                                                       "var c =  ifj.readi32();"
                                                       "var d =  ifj.readi32();"
                                                       "if(c == d){}else{}"
                                                       "}\n";
    char *check_expression_unknown_1_value=  "const ifj = @import(\"ifj24.zig\");\n"
                                                        "pub fn main() void{\n"
                                                        "const c =  ifj.readi32();"
                                                        "const d =  2;"
                                                        "if(c == d){}else{}"
                                                        "}\n";
    char *accept_multiple_expression_unknown_1_value_=  "const ifj = @import(\"ifj24.zig\");\n"
                                              "pub fn main() void{\n"
                                              "const c =  ifj.readi32();"
                                              "const d =  2;"
                                              "if(c)|C|{ if((C + 2.0)==(d + 2.0)){}else{} }"
                                              "else{}"
                                              "}\n";
    char *check_multiple_expression_unknown_2_value_=  "const ifj = @import(\"ifj24.zig\");\n"
                                                        "pub fn main() void{\n"
                                                        "var c =  ifj.readi32();"
                                                        "const d =  2;"
                                                        "if((c + c)==(d + c)){}else{}"
                                                        "}\n";
    char *check_not_used_variable=  "const ifj = @import(\"ifj24.zig\");\n"
                                                       "pub fn main() void{\n"
                                                       "var c =  ifj.readi32();"
                                                       "}\n";
    char *check_not_used_variable_in_while=  "const ifj = @import(\"ifj24.zig\");\n"
                                    "pub fn main() void{\n"
                                    "var c =  ifj.readi32();"
                                    "while(c)|C|{}"
                                    "}\n";
    char *check_not_used_variable_in_if=  "const ifj = @import(\"ifj24.zig\");\n"
                                             "pub fn main() void{\n"
                                             "var c =  ifj.readi32();"
                                             "if(c)|C|{}else{}"
                                             "}\n";
    char *redefinition_of_fn_param=  "const ifj = @import(\"ifj24.zig\");\n"
                                    "    pub fn func(x : i32, x : i32) i32{\n"
                                    "        return x;\n"
                                    "    }\n"
                                    "    pub fn main() void{\n"
                                    "        _ = func(5, 5);\n"
                                    "    }";

    char *redefinition_of_fn_param2=  "const ifj = @import(\"ifj24.zig\");\n"
                                     "    pub fn func(x : i32, y : i32, c : i32, x : f64) i32{\n"
                                     "        return x;\n"
                                     "    }\n"
                                     "    pub fn main() void{\n"
                                     "        _ = func(5, 5);\n"
                                     "    }";
    char *accept_expression16=  "const ifj = @import(\"ifj24.zig\");\n"
                                "\n"
                                "pub fn main() void{\n"
                                "    var   b : ?f64 = ifj.readf64();\n"
                                "    const a : i32 = 5;\n"
                                "    if(b)|B|{\n"
                                "        const ab : f64 = a + B;\n"
                                "        ifj.write(ab);\n"
                                "    }\n"
                                "    else{\n"
                                "        b = 5.2;\n"
                                "    }\n"
                                "}";

    char *check_null_arithmetic=  "const ifj = @import(\"ifj24.zig\");\n"
                                "\n"
                                "pub fn main() void{\n"
                                " const b : ?f64 = ifj.readf64();\n"
                                " const a = 1 + b;"
                                "}";
    char *check_null_arithmetic2=  "const ifj = @import(\"ifj24.zig\");\n"
                                  "\n"
                                  "pub fn main() void{\n"
                                  " const   b : ?i32 = 3;"
                                  " const a = 1 + b;"
                                  "}";
    char *accept_arithmetic=  "const ifj = @import(\"ifj24.zig\");\n"
                                   "\n"
                                   "pub fn main() void{\n"
                                   " const b : i32 = 3;"
                                   " const a = 1 + b;"
                                    "_ = a;"
                                   "}";
    char *accept_arithmetic2=  "const ifj = @import(\"ifj24.zig\");\n"
                              "\n"
                              "pub fn main() void{\n"
                              " const b : f64 = 3.3;"
                              " const a = 1 + b;"
                              "_ = a;"
                              "}";
    char *check_arithmetic_with_null=  "const ifj = @import(\"ifj24.zig\");\n"
                               "\n"
                               "pub fn main() void{\n"
                               " const b : f64 = 3.3;"
                              " const c : f64 = 3.3;"
                               " while(b == c){}"
                               "}";

    char *check_if_with_not_nullable=  "const ifj = @import(\"ifj24.zig\");\n"
                                        "\n"
                                        "pub fn main() void{\n"
                                        " const b : f64 = 3.3;"
                                        "if(b)|B|{}else{}"
                                        "}";
    char *accept_if_nullable_string_convert_to_non_nullable=  "const ifj = @import(\"ifj24.zig\");\n"
                                       "\n"
                                       "pub fn main() void{\n"
                                       " const b: ?[]u8 = null;"
                                       "if(b)|B|{_ = B;}else{}"
                                       "}";

    char *accept_compare_with_null=  "const ifj = @import(\"ifj24.zig\");\n"
                                                           "\n"
                                                           "pub fn main() void{\n"
                                                           "const a: ?i32 = null;"
                                                           "if(a == null){}else{}"
                                                           "}";
    char *accept_compare_with_null2=  "const ifj = @import(\"ifj24.zig\");\n"
                                     "\n"
                                     "pub fn main() void{\n"
                                     "const a: ?f64 = null;"
                                     "if(a == null){}else{}"
                                     "}";
    char *accept_compare_with_null3=  "const ifj = @import(\"ifj24.zig\");\n"
                                      "\n"
                                      "pub fn main() void{\n"
                                      "const a: ?[]u8 = null;"
                                      "if(a == null){}else{}"
                                      "}";

    char *check_compare_with_null=  "const ifj = @import(\"ifj24.zig\");\n"
                                      "\n"
                                      "pub fn main() void{\n"
                                    "const a: i32 = 4;"
                                      "if(a == null){}else{}"
                                      "}";
    char *check_compare_with_null2=  "const ifj = @import(\"ifj24.zig\");\n"
                                    "\n"
                                    "pub fn main() void{\n"
                                    "const a: f64 = 4;"
                                    "if(a == null){}else{}"
                                    "}";
    char *check_compare_with_null3=  "const ifj = @import(\"ifj24.zig\");\n"
                                     "\n"
                                     "pub fn main() void{\n"
                                     "const a = ifj.string(\"ASDS\");"
                                     "if(a == null){}else{}"
                                     "}";
    char *accept_compare_null_null=  "const ifj = @import(\"ifj24.zig\");\n"
                                      "\n"
                                      "pub fn main() void{\n"
                                      "if(null == null){}else{}"
                                      "}";



//TODO test _

    // Define your test cases and their expected exit codes
    Test tests[MAX_TESTS] = {
            {accept_compare_null_null,     0},
            {check_compare_with_null3,     7},
            {check_compare_with_null2,     7},
            {check_compare_with_null,     7},
            {accept_compare_with_null3,     0},
            {accept_compare_with_null2,     0},
            {accept_compare_with_null,     0},
            {accept_if_nullable_string_convert_to_non_nullable,     0},
            {check_if_with_not_nullable,       7},
            {check_arithmetic_with_null,       0},
            {accept_expression,       0},
            {accept_arithmetic2,       0},
            {accept_arithmetic,       0},
            {check_null_arithmetic2,       7},
            {check_null_arithmetic,       7},
            {accept_expression16,       0},
            {redefinition_of_fn_param2,       5},
            {redefinition_of_fn_param,       5},
            {check_not_used_variable_in_if,       9},
            {check_not_used_variable_in_while,       9},
            {check_not_used_variable,       9},
            {check_multiple_expression_unknown_2_value_,       7},
            {accept_multiple_expression_unknown_1_value_,       0},
            {check_expression_unknown_1_value,       7},
            {check_expression_2_unknown_value_variables2,       7},
            {check_expression_2_unknown_value_variables,       7},
            {check_float_nullable_to_string_nullable_variable,       7},
            {check_float_to_string_nullable_variable,       7},
            {check_int_nullable_to_string_nullable_variable,       7},
            {check_int_to_string_nullable_variable,       7},
            {check_float_nullable_to_string_variable,       7},
            {check_float_to_string_variable,       7},
            {check_int_nullable_to_string_variable,       7},
            {check_int_to_string_variable,       7},
            {accept_ifj24_example3,       0},
            {accept_ifj24_example2,       0},
            {accept_ifj24_example1,       0},
            {accept_funexp2,       0},
            {accept_funexp,       0},
            {check_string_in_expression2,       7},
            {check_string_in_expression,       7},
            {check_string_assignment,       7},
            {check_string_assignment_init,       8},
            {accept_expression_with_null2,       0},
            {accept_expression_with_null,       0},
            {check_expression_with_null2,       7},
            {check_expression_with_null,       7},
            {check_expression6,       7},
            {accept_expression15,       0},
            {accept_expression14,       0},
            {accept_expression13,       0},
            {accept_expression12,       0},
            {accept_expression11,       0},
            {accept_expression10,       0},
            {accept_expression9,       0},
            {accept_expression8,       0},
            {accept_expression7,       0},
            {accept_expression6,       0},
            {accept_expression5,       0},
            {accept_expression4,       0},
            {accept_expression3,       0},
            {accept_expression2,       0},
            {accept_expression1,       0},
            {while_with_nullable_var_redefined_nested3,       5},
            {while_with_nullable_var_redefined_nested2,       5},
            {while_with_nullable_var_redefined_nested,       5},
            {while_with_nullable_var_redefined,       5},
            {while_with_not_null_expected_bool_with_null_given,       7},
            {while_with_null_expected_bool_given,       7},
            {if_with_nullable_var_redefined_nested3,       5},
            {accept_if_with_nullable_var_redefined_in_else,       0},
            {if_with_nullable_var_redefined_nested2,       5},
            {if_with_nullable_var_redefined_nested,       5},
            {if_with_nullable_var_redefined,       5},
            {if_with_not_null_expected_bool_with_null_given,       7},
            {if_with_null_expected_bool_given,       7},
            {redefinition_of_param_in_nested2,       5},
            {redefinition_of_param_in_nested,       5},
            {not_defined_in_nested,       3},
            {multiple_redefinition_nested_block,       5},
            {accepted_redefinition_nested_block,       0},
            {accepted_redefinition_in_3_1_nested_block,       0},
            {redefinition_in_1_3_nested_block,       5},
            {redefinition_in_3_nested_block,       5},
            {redefinition_in_else_block,       5},
            {redefinition_in_if_block,       5},
            {while_expression11,       7},
            {while_expression10,       0},
            {while_expression9,       7},
            {while_expression8,       7},
            {while_expression7,       7},
            {while_expression6,       7},
            {while_expression5,       7},
            {while_expression4,       7},
            {while_expression3,       7},
            {while_expression2,       7},
            {while_expression,       7},
            {expression1,       8},
            {expression2,       7},
            {expression3,       7},
            {expression4,       7},
            {expression5,       7},
            {argument_non_used,       9},
            {returns_value_when_no_value_expected,       6},
            {returns_none,       6},
            {return_with_wrong_type,       4},
            {no_return_value_when_expected,       6},
            {redefinition_with_const_of_fn_param,       5},
            {redefinition_with_var_of_fn_param,       5},
            {redefinition_of_2th_fn_params,       5},
            {redefinition_of_fn_params2,       5},
            {redefinition_of_fn_params,       5},
            {redefinition_of_main,       5},
            {redefinition_of_fn,       5},
            {redefinition_const_no_variable_type_in_same_block,       5},
            {redefinition_const_in_same_block,       5},
            {redefinition_var_in_same_block,       5},
            {main_contains_params,       4},
            {main_is_not_void,       4},
            {main_missing,       3},

            // Add more tests as needed
    };
    int num_tests = 0;
    for (int i = 0; i < MAX_TESTS; ++i) {
        if(tests[i].test_case == NULL){
            num_tests = i;
            break;
        }

    }
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