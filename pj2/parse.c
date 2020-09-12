/*

**************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   CS311 KAIST                                               */
/*   parse.c                                                   */
/*                                                             */
/***************************************************************/

#include <stdio.h>

#include "util.h"
#include "parse.h"

int text_size;
int data_size;

/**************************************************************/
/*                                                            */
/* Procedure : parsing_instr                                  */
/*                                                            */
/* Purpose   : parse binary format instrction and return the  */
/*             instrcution data                               */
/*                                                            */
/* Argument  : buffer - binary string for the instruction     */
/*             index  - order of the instruction              */
/*                                                            */
/**************************************************************/
instruction parsing_instr(const char *buffer, const int index) {
    instruction instr = {0, };
    
    char op[7];
    char rs[6];
    char rt[6];
    char rd[6];
    char sh[6];
    char fc[7];
    char im[17];
    char tg[27];

    memcpy (op, buffer, 6);
    *(op + 6) = '\0';

    if (strtol (op, NULL, 2) == 0x00) {
            memcpy (rs, buffer + 6, 5);
            *(rs + 5) = '\0';
            memcpy (rt, buffer + 11, 5);
            *(rt + 5) = '\0';
            memcpy (rd, buffer + 16, 5);
            *(rd + 5) = '\0';
            memcpy (sh, buffer + 21, 5);
            *(sh + 5) = '\0';
            memcpy (fc, buffer + 26, 6);
            *(op + 6) = '\0';
            
    //        printf ("InstructionR op:%s, rs:%s, rt:%s, rd:%s, sh:%s, fc:%s\n"
    //                        , op, rs, rt, rd, sh, fc);

            instr.opcode = strtol (op, NULL, 2);
            instr.r_t.r_i.rs = strtol (rs, NULL, 2);
            instr.r_t.r_i.rt = strtol (rt, NULL, 2);
            instr.r_t.r_i.r_i.r.rd = strtol (rd, NULL, 2);
            instr.r_t.r_i.r_i.r.shamt = strtol (sh, NULL, 2);
            instr.func_code = strtol (fc, NULL, 2);
    }
   
    else if (strtol (op, NULL, 2) == 0x02 || strtol (op, NULL, 2) == 0x03) {
            memcpy (tg, buffer + 6, 26); 
            *(tg + 26) = '\0';

    //        printf ("InstructionJ op:%s, tg:%s\n", op, tg);

            instr.opcode = strtol (op, NULL, 2);
            instr.r_t.target = strtol (tg, NULL, 2);
    }

    else {
            memcpy (rs, buffer + 6, 5);
            *(rs + 5) = '\0';
            memcpy (rt, buffer + 11, 5);
            *(rt + 5) = '\0';
            memcpy (im, buffer + 16, 16);
            *(im + 16) = '\0';

    //        printf ("InstructionI, op:%s, rs:%s, rt:%s, im:%s\n", op, rs, rt, im);

            instr.opcode = strtol (op, NULL, 2);
            instr.r_t.r_i.rs = strtol (rs, NULL, 2);
            instr.r_t.r_i.rt = strtol (rt, NULL, 2);
            instr.r_t.r_i.r_i.imm = strtol (im, NULL, 2);
    }
    
    return instr;
}
/**************************************************************/
/*                                                            */
/* Procedure : parsing_data                                   */
/*                                                            */
/* Purpose   : parse binary format data and store data into   */
/*             the data region                                */
/*                                                            */
/* Argument  : buffer - binary string for data                */
/*             index  - order of data                         */
/*                                                            */
/**************************************************************/
void parsing_data(const char *buffer, const int index) {
    
    mem_write_32 (MEM_DATA_START + index, strtol (buffer, NULL, 2));

}
