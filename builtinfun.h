/* built in functions */
/* code generator */
/*ifj november 2024 */

#include <stdio.h>
#ifndef BUILTIN_H
#define BUILTIN_H
/*funkcie pre nacitanie literalov a vypis temov*/
void read_string();
void read_int();
void read_float();

void write_term();

/*funkcie pre konverziu ciselnych typov*/
void convert_int2_float();
void convert_float2_int();

/*praca s retazcovymi literalmi*/
//void fun_string();
void fun_length();
void fun_concat();
void fun_substring();
void fun_strcmp();
void fun_ord_value();
void fun_char();

#endif