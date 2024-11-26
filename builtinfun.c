/**
 * @file builtinfun.c
 * @brief Generator of built in function for the IFJ project.
 *
 * @author Silvia Šlachtovská (xslachs00)
 *
 * @date 2024
 */

#include "builtinfun.h"
#include <stdio.h>


/*pub fn ifj.readstr() ?[]u8*/
void read_string() {
    printf("JUMP $end_reads\n");
    printf("LABEL $reads\n");

    printf("PUSHFRAME\n");

    //define and read input
    printf("DEFVAR LF@retval\n");
    printf("READ LF@retval string\n");

    //push result to the stack
    printf("PUSHS LF@retval\n");

    printf("POPFRAME\n");
    printf("RETURN\n");

    printf("LABEL $end_reads\n");
}

/*pub fn ifj.readi32() ?i32*/
void read_int() {
    printf("JUMP $end_readi\n");
    printf("LABEL $readi\n");

    printf("PUSHFRAME\n");


    printf("DEFVAR LF@retval\n");
    printf("READ LF@retval int\n");

    //push the result to the stack 
    printf("PUSHS LF@retval\n");

    printf("POPFRAME\n");
    printf("RETURN\n");


    printf("LABEL $end_readi\n");
}


/*pub fn ifj.readf64() ?f64*/
void read_float() {
    printf("JUMP $end_readf\n");
    printf("LABEL $readf\n");
    printf("PUSHFRAME\n");

    // Define the return value variable
    printf("DEFVAR LF@retval1\n");
    printf("READ LF@retval1 float\n");

    printf("POPFRAME\n");
    printf("RETURN\n");

    printf("LABEL $end_readf\n");

}

/*pub fn ifj.write(term) void*/
void write_term() {
    printf("JUMP $write_end1");
    printf("LABEL $write_term");
    printf("PUSHFRAME\n");

    // Define local variables for type checking
    printf("DEFVAR LF@type\n");
    printf("DEFVAR LF@temp\n");

    printf("MOVE LF@temp TF@arg1\n"); 

    //determine the type of the term
    printf("TYPE LF@type LF@temp\n");

    //handle each type
    printf("JUMPIFEQ $write_int LF@type string@int\n");
    printf("JUMPIFEQ $write_float LF@type string@float\n");
    printf("JUMPIFEQ $write_string LF@type string@string\n");
    printf("JUMPIFEQ $write_nil LF@type string@nil\n");


    //integer handling
    printf("LABEL $write_int\n");
    printf("WRITE LF@temp\n");
    printf("JUMP $write_end\n");

    //float handling
    printf("LABEL $write_float\n");
    printf("WRITE float@\n\n");
    printf("JUMP $write_end\n");

    //string handling
    printf("LABEL $write_string\n");
    printf("WRITE LF@temp\n");
    printf("JUMP $write_end\n");

    //nil handling
    printf("LABEL $write_nil\n");
    printf("WRITE nil@nil\n");
    printf("JUMP $write_end\n");

    //end of function
    printf("LABEL $write_end\n");
    printf("POPFRAME\n");
    //printf("RETURN\n");
    printf("LABEL $write_end1\n");
}

/*pub fn ifj.i2f(term ∶ i32) f64*/
void convert_int2_float() {
    printf("JUMP $int2float_end\n");
    printf("LABEL $int2float\n");
    printf("PUSHFRAME\n"); 

    //define local variables
    printf("DEFVAR LF@input\n");  
    printf("DEFVAR LF@output\n"); 

    //move
    printf("MOVE LF@input TF@arg1\n");

    //convert integer to float
    printf("INT2FLOAT LF@output LF@input\n");

    //return the converted value
    printf("MOVE TF@retval1 LF@output\n");

    printf("POPFRAME\n"); 
    printf("RETURN\n");

    //end label
    printf("LABEL $int2float_end\n");
}

/*pub fn ifj.f2i(term ∶ f64) i32 */
void convert_float2_int() {
    printf("JUMP $float2int_end\n");
    printf("LABEL $float2int\n");
    printf("PUSHFRAME\n"); 

    //define local variables
    printf("DEFVAR LF@input\n");  
    printf("DEFVAR LF@output\n"); 

    //move the input from the first argument 
    printf("MOVE LF@input TF@arg1\n");

    //convert float to integer
    printf("FLOAT2INT LF@output LF@input\n");

    //return the converted value
    printf("MOVE TF@retval1 LF@output\n");

    printf("POPFRAME\n"); 
    printf("RETURN\n");

    printf("LABEL $float2int_end\n");
}

/*pub fn ifj.length(𝑠 : []u8) i32*/
void fun_length() {
    printf("JUMP $length_end\n");
    printf("LABEL $length\n");
    printf("PUSHFRAME\n"); 


    printf("DEFVAR LF@input\n");  
    printf("DEFVAR LF@result\n"); 

   
    printf("MOVE LF@input TF@arg1\n");

    //calculate the length of the string
    printf("STRLEN LF@result LF@input\n");

    //set the return value
    printf("MOVE TF@retval1 LF@result\n");


    printf("POPFRAME\n");
    printf("RETURN\n");


    printf("LABEL $length_end\n");
}

/*pub fn ifj.concat(𝑠1 : []u8, 𝑠2 : []u8) []u8*/
void fun_concat() {
    printf("JUMP $concat_end\n");   
    printf("LABEL $concat\n");       
    printf("PUSHFRAME\n");           

    //define local variables
    printf("DEFVAR LF@str1\n");     //first string argument
    printf("DEFVAR LF@str2\n");     //second string argument
    printf("DEFVAR LF@result\n");   //concatenation result

    //move arguments into local variables
    printf("MOVE LF@str1 TF@arg1\n");
    printf("MOVE LF@str2 TF@arg2\n");

    //concatenate the two strings
    printf("CONCAT LF@result LF@str1 LF@str2\n");

    //set the return value
    printf("MOVE TF@retval1 LF@result\n");

    printf("POPFRAME\n");
    printf("RETURN\n");

    printf("LABEL $concat_end\n");
}


/*pub fn ifj.substring(𝑠 : []u8, 𝑖 : i32, 𝑗 : i32) ?[]u8*/
void fun_substring() {
   printf("JUMP $substring_end\n");            
    printf("LABEL $substring\n");               
    printf("PUSHFRAME\n");                      

    //define local variables
    printf("DEFVAR LF@string\n");               // input string
    printf("DEFVAR LF@start\n");                // start index
    printf("DEFVAR LF@end\n");                  // end index
    printf("DEFVAR LF@retval\n");               // result substring
    printf("DEFVAR LF@boolval\n");              // bool for conditions
    printf("DEFVAR LF@index\n");                // loop index

    //move input parameters into local variables
    printf("MOVE LF@string TF@arg1\n");
    printf("MOVE LF@start TF@arg2\n");
    printf("MOVE LF@end TF@arg3\n");

    //initialize the return value as an empty string
    printf("MOVE TF@retval1 string@\n");

    // validate parameters: start < 0, end < 0, start > end
    printf("LT LF@boolval LF@start int@0\n");
    printf("JUMPIFEQ $substring_error LF@boolval bool@true\n");
    printf("LT LF@boolval LF@end int@0\n");
    printf("JUMPIFEQ $substring_error LF@boolval bool@true\n");
    printf("GT LF@boolval LF@start LF@end\n");
    printf("JUMPIFEQ $substring_error LF@boolval bool@true\n");

    //calculate string length using the `STRLEN` instruction
    printf("DEFVAR LF@length\n");
    printf("STRLEN LF@length LF@string\n");

    //validate parameters: start >= length, end > length
    printf("GE LF@boolval LF@start LF@length\n");
    printf("JUMPIFEQ $substring_error LF@boolval bool@true\n");
    printf("GT LF@boolval LF@end LF@length\n");
    printf("JUMPIFEQ $substring_error LF@boolval bool@true\n");

    //loop to extract characters and build the substring
    printf("MOVE LF@index LF@start\n");
    printf("LABEL $substring_loop\n");
    printf("JUMPIFEQ $substring_end LF@index LF@end\n");
    printf("GETCHAR LF@boolval LF@string LF@index\n");
    printf("CONCAT LF@retval LF@retval LF@boolval\n");
    printf("ADD LF@index LF@index int@1\n");
    printf("JUMP $substring_loop\n");

    //handle errors: return nil@nil
    printf("LABEL $substring_error\n");
    printf("MOVE LF@retval nil@nil\n");

    printf("LABEL $substring_end\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
}

/*pub fn ifj.strcmp(𝑠1 : []u8, 𝑠2 : []u8) i32*/
void fun_strcmp() {
    printf("JUMP $strcmp_end\n");                  
    printf("LABEL $strcmp\n");                   
    printf("PUSHFRAME\n");                        

    //define local variables
    printf("DEFVAR LF@result\n");                 //result of the comparison
    printf("DEFVAR LF@i\n");                      //loop index
    printf("DEFVAR LF@len1\n");                   // length of the first string
    printf("DEFVAR LF@len2\n");                   // length of the second string
    printf("DEFVAR LF@char1\n");                  // current character from the first string
    printf("DEFVAR LF@char2\n");                  // current character from the second string

    printf("MOVE LF@result int@0\n");             // initialize result to 0
    printf("MOVE LF@i int@0\n");                  // initialize loop index to 0

    //get the lengths of the input strings
    printf("STRLEN LF@len1 TF@arg1\n");            
    printf("STRLEN LF@len2 TF@arg2\n");             

    //loop to compare characters of both strings
    printf("LABEL $strcmp_loop\n");
    printf("LT LF@result LF@i LF@len1\n");        // check if index < length of s1
    printf("JUMPIFEQ $strcmp_end LF@result bool@false\n");
    printf("LT LF@result LF@i LF@len2\n");        // check if index < length of s2
    printf("JUMPIFEQ $strcmp_end LF@result bool@false\n");

    printf("GETCHAR LF@char1 TF@arg1 LF@i\n");      // get character at index i from s1
    printf("GETCHAR LF@char2 TF@arg2 LF@i\n");      // get character at index i from s2

    // compare the current characters
    printf("JUMPIFEQ $strcmp_next LF@char1 LF@char2\n");  // characters are equal
    printf("LT LF@result LF@char1 LF@char2\n");   // check if char1 < char2
    printf("JUMPIFEQ $strcmp_smaller LF@result bool@true\n");
    printf("JUMP $strcmp_greater\n");             // otherwise, char1 > char2

    //move to the next character
    printf("LABEL $strcmp_next\n");
    printf("ADD LF@i LF@i int@1\n");              // increment index
    printf("JUMP $strcmp_loop\n");                // continue the loop

    // end of comparison
    printf("LABEL $strcmp_end\n");
    printf("LT LF@result LF@len1 LF@len2\n");     // check if s1 is shorter than s2
    printf("JUMPIFEQ $strcmp_smaller LF@result bool@true\n");
    printf("GT LF@result LF@len1 LF@len2\n");     // check if s1 is longer than s2
    printf("JUMPIFEQ $strcmp_greater LF@result bool@true\n");

    // if lengths and chars match, the strings are equal
    printf("MOVE LF@result int@0\n");
    printf("JUMP $strcmp_return\n");

    // string s1 is smaller than s2
    printf("LABEL $strcmp_smaller\n");
    printf("MOVE LF@result int@-1\n");
    printf("JUMP $strcmp_return\n");

    // string s1 is greater than s2
    printf("LABEL $strcmp_greater\n");
    printf("MOVE LF@result int@1\n");

    // return the result
    printf("LABEL $strcmp_return\n");
    printf("PUSHS LF@result\n");                  
    printf("POPFRAME\n");                         
    printf("RETURN\n");                           

    printf("LABEL $strcmp_end\n");
    printf("\n");
}

/*pub fn ifj.ord(𝑠 : []u8, 𝑖 : i32) i32*/
void fun_ord_value() {
    printf("JUMP $ord_end\n");                
    printf("LABEL $ord\n");                  

    printf("PUSHFRAME\n");      

    // define local variables
    printf("DEFVAR LF@output\n");             // result of the function
    printf("DEFVAR LF@cont\n");               // bool for conditions
    printf("DEFVAR LF@strlen\n");             // length of the input string
    printf("DEFVAR LF@char\n");               // character extracted from the string

    //initialize the result to 0
    printf("MOVE LF@output int@0\n");

    // calculate the length of the input string
    printf("STRLEN LF@strlen TF@arg1\n");       

    // check if the string is empty or the index is out of limit
    printf("LT LF@cont TF@arg2 int@0\n");       
    printf("JUMPIFEQ $ord_error LF@cont bool@true\n");
    printf("GE LF@cont TF@arg2 LF@strlen\n");
    printf("JUMPIFEQ $ord_error LF@cont bool@true\n");

    // extract the character at the specified index
    printf("STRI2INT LF@output TF@arg1 TF@arg2\n"); 

    // jump to the return label
    printf("JUMP $ord_return\n");

    // error handling for invalid index or empty string
    printf("LABEL $ord_error\n");
    printf("MOVE LF@output int@0\n");

    // return result
    printf("LABEL $ord_return\n");
    printf("PUSHS LF@output\n");             
    printf("POPFRAME\n");                    
    printf("RETURN\n");                      

    // End of the function
    printf("LABEL $ord_end\n");
    printf("\n");

}

/*pub fn ifj.chr(𝑖 : i32) []u8*/
void fun_char() {
    printf("JUMP $char_end\n");                 
    printf("LABEL $char\n");                   

    printf("PUSHFRAME\n");                      

    //define local variables
    printf("DEFVAR LF@retval\n");               
    printf("DEFVAR LF@check\n");                

    //check if the input is out of range 
    printf("LT LF@check TF@arg1 int@0\n");       
    printf("JUMPIFEQ $char_error LF@check bool@true\n");
    printf("GT LF@check TF@arg1 int@255\n");
    printf("JUMPIFEQ $char_error LF@check bool@true\n");

    // convert integer to character and store in result
    printf("INT2CHAR LF@retval TF@arg1\n");       // convert ASCII code to character
    printf("JUMP $char_return\n");

    // error handling: return nil if input is out of range
    printf("LABEL $char_error\n");
    printf("MOVE LF@retval nil@nil\n");


    printf("LABEL $char_return\n");
    printf("PUSHS LF@retval\n");                // Push result to stack
    printf("POPFRAME\n");                       
    printf("RETURN\n");                        


    printf("LABEL $char_end\n");
    printf("\n");
}

