#ifndef IR_H
#define IR_H
#include "list.h"
#include "symtab.h"
#include "ast.h"

// TODO: implement arrays

typedef struct tac_line_s {
    enum { TAC_ASSIGN, TAC_OPERATE, TAC_JMP, TAC_COND_JMP, TAC_PUSH, TAC_POP, TAC_CALL, TAC_RET, TAC_RET_VAL } kind; 

    /*
        INSTRUCTION SET SPECS

                arg1        arg2
    ASSIGN      rvalue      ------
    OPERATE     operand     operand
    JMP         label       ------
    COND_JMP    condition   label
    PUSH        value       ------
    POP         bytes       ------
    CALL        label       ------
    RET         ------      ------
    RET_VAL     value       ------
    */

    union { 
        int temp_var;
        int int_val;
        int double_val;
        char* str_val;
    } arg1, arg2; 

} tac_line_t; 

// struct operate_s {
//     enum { PLUS, MINUS, TIMES, DIVIDE, LT, GT, LEQ, GEQ, EQ, NEQ } operation;

//     union {
//         int int_val;
//         double double_val;
//         char* str_val; 
//         symbol_t* id_val; 
//     } arg1, arg2; 

// }; 

// struct comp_s {
//     int cond_loc;
//     int goto_loc; 
// };

// struct proc_call_s {

// };


// typedef struct tac_line_s {
    
//     enum { OPERATE, COMP, GOTO, PROC_CALL } kind;

//     union {
//         struct operate_s op_line;
//         struct comp_s comp_line;
//         int goto_line;
//         struct proc_call_s proc_line;
//     } children; 

//     int loc; 

// } tac_line_t; 

// typedef struct tac_prog_s {

// } tac_prog_t; 

#endif