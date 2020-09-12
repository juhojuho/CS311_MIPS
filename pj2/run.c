/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   CS311 KAIST                                               */
/*   run.c                                                     */
/*                                                             */
/***************************************************************/

#include <stdio.h>

#include "util.h"
#include "run.h"

/***************************************************************/
/*                                                             */
/* Procedure: process_instruction                              */
/*                                                             */
/* Purpose: Process one instrction                             */
/*                                                             */
/***************************************************************/
void process_instruction(){

    unsigned char rs;
    unsigned char rt;
    short imm;
    unsigned char rd;
    unsigned char shamt;
    uint32_t target; 

    instruction current = INST_INFO [(CURRENT_STATE.PC - MEM_TEXT_START) / 4];
    CURRENT_STATE.PC += 4;

    // if next PC exceeds text region, turn off RUN_BIT.
    if (CURRENT_STATE.PC >= MEM_TEXT_START + NUM_INST * 4) {
            RUN_BIT = FALSE;
    }

    // aaddiu
    if (current.opcode == 0x09) {
            instructionI (&rs, &rt, &imm);
            reg_write (rt, reg_read (rs) + sign_extend (imm));
    }
    // addu
    if (current.opcode == 0x00 && current.func_code == 0x21) {
           instructionR (&rs, &rt, &rd, &shamt); 
           reg_write (rd, reg_read (rs) + reg_read (rt));
    }
    // and
    if (current.opcode == 0x00 && current.func_code == 0x24) {
            instructionR (&rs, &rt, &rd, &shamt);
            reg_write (rd, reg_read (rs) & reg_read (rt));
    }
    // andi
    if (current.opcode == 0x0C) {
            instructionI (&rs, &rt, &imm);
            reg_write (rt, reg_read (rs) & (0x0000 << 16 | imm));
    }
    // beq
    if (current.opcode == 0x04) {
            instructionI (&rs, &rt, &imm);
            if (reg_read (rs) == reg_read (rt))
                    CURRENT_STATE.PC += sign_extend (imm) << 2;
    }
    // bne
    if (current.opcode == 0x05) {
            instructionI (&rs, &rt, &imm);
            if (reg_read (rs) != reg_read (rt))
                    CURRENT_STATE.PC += sign_extend (imm) << 2;
    }
    // j
    if (current.opcode == 0x02) {
            instructionJ (&target);
	    CURRENT_STATE.PC = (CURRENT_STATE.PC & 0xF0000000) | (target << 2);
    }
    // jal
    if (current.opcode == 0x03) {
            instructionJ (&target);
            reg_write (31, CURRENT_STATE.PC);
            CURRENT_STATE.PC = (CURRENT_STATE.PC & 0xF0000000) | (target << 2);
    }
    // jr
    if (current.opcode == 0x00 && current.func_code == 0x08) {
            instructionR (&rs, &rt, &rd, &shamt);
            CURRENT_STATE.PC = reg_read (rs);
    }
    // lui
    if (current.opcode == 0x0F) {
            instructionI (&rs, &rt, &imm);
            reg_write (rt, imm << 16);
    }
    // lw
    if (current.opcode == 0x23) {
            instructionI (&rs, &rt, &imm);
            reg_write (rt, mem_read_32 (reg_read (rs) + (int) sign_extend (imm)));
    }
    // nor
    if (current.opcode == 0x00 && current.func_code == 0x27) {
            instructionR (&rs, &rt, &rd, &shamt);
            reg_write (rd, ~ (reg_read (rs) | reg_read (rt)));
    }
    // or
    if (current.opcode == 0x00 && current.func_code == 0x25 ) {
            instructionR (&rs, &rt, &rd, &shamt);
            reg_write (rd, (reg_read (rs) | reg_read (rt)));
    }
    // ori
    if (current.opcode == 0x0D) {
            instructionI (&rs, &rt, &imm);
            reg_write (rt, reg_read (rs) | (0x0000 << 16 | imm));
    }
   // sltiu
    if (current.opcode == 0x0B) {
            instructionI (&rs, &rt, &imm);
            if (reg_read (rs) < sign_extend (imm))
                    reg_write (rt, 1);
            else
                    reg_write (rt, 0);
    }
    // sltu
    if (current.opcode == 0x00 && current.func_code == 0x2B) {
            instructionR (&rs, &rt, &rd, &shamt);
            if (reg_read (rs) < reg_read (rt))
                    reg_write (rd, 1);
            else
                    reg_write (rd, 0);
    }
    // sll
    if (current.opcode == 0x00 && current.func_code == 0x00) {
            instructionR (&rs, &rt, &rd, &shamt);
            reg_write (rd, reg_read (rt) << shamt);
    }
    // srl
    if (current.opcode == 0x00 && current.func_code == 0x02) {
            instructionR (&rs, &rt, &rd, &shamt);
            reg_write (rd, reg_read (rt) >> shamt);
    }
    // sw
    if (current.opcode == 0x2B) {
            instructionI (&rs, &rt, &imm);
            mem_write_32 (reg_read (rs) + (int) sign_extend (imm), reg_read (rt));
    }
    // subu
    if (current.opcode == 0x00 && current.func_code == 0x23) {
            instructionR (&rs, &rt, &rd, &shamt);
            reg_write (rd, reg_read (rs) - reg_read (rt));
    }
}
