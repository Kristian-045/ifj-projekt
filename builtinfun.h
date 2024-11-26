/**
 * @file codegenerator.c
 * @brief Generator for built in functions for the IFJ project.
 *
 * @author Silvia Šlachtovská (xslachs00)
 *
 * @date 2024
 */

#include <stdio.h>
#ifndef BUILTIN_H
#define BUILTIN_H

/* Functions for reading literals and outputting terms */

// Reads a string input and returns it
void read_string();
// Reads an integer input and returns it
void read_int();
// Reads a floating-point number and returns it
void read_float();

// Outputs a term (value) to standard output
void write_term();

/* Functions for numeric type conversions */

// Converts an integer value to a floating-point value
void convert_int2_float();
// Converts a floating-point value to an integer value
void convert_float2_int();


/* Functions for string manipulation */

// Calculates and returns the length of a string
void fun_length();
// Concatenates two strings and returns the result
void fun_concat();
// Extracts a substring from a string given start and end indices
void fun_substring();
// Compares two strings and returns an integer based on comparison
void fun_strcmp();
// Returns the ASCII value of a character at a specific index in a string
void fun_ord_value();
// Returns a single-character string from an ASCII value
void fun_char();

#endif
