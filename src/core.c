#include "../include/core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<unistd.h>

#include "../include/helper.h"
#include "../include/math.h"
#include "../include/debug.h"

char DIRECT_MODE[ADDRESS_MODE_LENGTH] = "00";
char IMMEDIATE_MODE[ADDRESS_MODE_LENGTH] = "01";
char INDEXED_MODE[ADDRESS_MODE_LENGTH] = "10";
char INDIRECT_MODE[ADDRESS_MODE_LENGTH] = "11";
char *BIN_MODES[4] = {DIRECT_MODE, IMMEDIATE_MODE, INDEXED_MODE, INDIRECT_MODE};
char *MNEMO_MODES[4] = {" ", "=", "$", "@"};
char memory[MEMORY_LENGTH][INSTRUCTION_LENGTH];
char IR[INSTRUCTION_LENGTH] = "0000000000000000";
char MBR[INSTRUCTION_LENGTH] = "0000000000000000";
// 10 cause the address field in memory cell is 10 bytes
char PC[ADDRESS_LENGTH] = "0000000000";
char MAR[ADDRESS_LENGTH] = "0000000000";
char XR[ADDRESS_LENGTH] = "0000000000";
char AC[INSTRUCTION_LENGTH] = "0000000000000000";

// COMMANDS
// Stop command
char HALT[COMMAND_LENGTH] = "0000";
char LOAD[COMMAND_LENGTH] = "0001";
// Store value into memory by address
char STORE[COMMAND_LENGTH] = "0010";
// not implemented
char CALL[COMMAND_LENGTH] = "0011";
// Unconditional branch
char BR[COMMAND_LENGTH] = "0100";
// if AC == 0
char BREQ[COMMAND_LENGTH] = "0101";
// If AC >= 0
char BRGE[COMMAND_LENGTH] = "0110";
// If AC < 0
char BRLT[COMMAND_LENGTH] = "0111";
char ADD[COMMAND_LENGTH] = "1000";
char SUB[COMMAND_LENGTH] = "1001";
char MUL[COMMAND_LENGTH] = "1010";
char DIV[COMMAND_LENGTH] = "1011";
char *COMMANDS_STR[COMMANDS_COUNT] = {
    "HALT", "LOAD", "STORE", "CALL", "BR", "BREQ", "BRGE", "BRLT", "ADD", "SUB", "MUL", "DIV"
};
char *BIN_COMMANDS_STR[COMMANDS_COUNT] = {
    "0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111", "1000", "1001", "1010", "1011"
};


// Is HALT command executed, to stop the loop
unsigned int is_halt = 0;
// MAX iteration to prevent infinite loop
unsigned int MAX_ITER = 1000;

/**
 *
 * @param start index of starting byte
 * @param end index of end byte
 * @param result var we store to from IR register
 */
void get_bits_from_instruction(const int start, const int end, char result[]) {
    for (int i = start, j = 0; i <= end; i++, j++) {
        result[j] = IR[i];
    }
}

/**
 * Get from memory first 16 bytes and store them in variable
 * @param bin_index Address in binary format, which would be used as index in memory
 * @param instruction Variable we store address into,
 */
void get_instruction_from_memory(char bin_index[ADDRESS_LENGTH], char instruction[INSTRUCTION_LENGTH - 1]) {
    const int index = bin_to_int(bin_index);
    if (index >= MEMORY_LENGTH) {
        fprintf(stderr, "Index out of range\n");
        abort();
    }
    for (int i = 0; i < INSTRUCTION_LENGTH - 1; i++) {
        instruction[i] = memory[index][i];
    }
}

/**
 *
 *
 * @param mode variable we store in the address
 * store address into MAR register
 * DIRECT mode - do nothing
 * IMMEDIATE_MODE - copy from MAR to MBR
 * INDEXED_MODE - MAR = MAR+MBR
 */
void match_address_mode(char mode[ADDRESS_MODE_LENGTH]) {
    store_address(MAR);
    if (strcmp(mode, DIRECT_MODE) == 0) {
        //do nothing
    } else if (strcmp(mode, IMMEDIATE_MODE) == 0) {
        copy_diff_len_str(MAR, MBR, 16 - 10);
    } else if (strcmp(mode, INDEXED_MODE) == 0) {
        sum_two_bin(MAR, MBR, MAR);
    } else {
        fprintf(stderr, "Mode not found\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * If, else func for processor logic
 * @param command Instruction from binary
 * @param mode Mode from binary
 */
void match_command(char command[COMMAND_LENGTH], char mode[ADDRESS_MODE_LENGTH]) {
    if (strcmp(command, LOAD) == 0) {
        if (strcmp(mode, IMMEDIATE_MODE) != 0) {
            get_instruction_from_memory(MAR, MBR);
        }
        copy_str(MBR, AC);
    } else if (strcmp(command, ADD) == 0) {
        if (strcmp(mode, IMMEDIATE_MODE) != 0) {
            get_instruction_from_memory(MAR, MBR);
        }
        sum_two_bin(AC, MBR, AC);
    } else if (strcmp(command, SUB) == 0) {
        if (strcmp(mode, IMMEDIATE_MODE) != 0) {
            get_instruction_from_memory(MAR, MBR);
        }
        sub_complement(AC, MBR, AC);
    } else if (strcmp(command, MUL) == 0) {
        if (strcmp(mode, IMMEDIATE_MODE) != 0) {
            get_instruction_from_memory(MAR, MBR);
        }
        multiply_bin(MBR, AC, AC);
    } else if (strcmp(command, DIV) == 0) {
        if (strcmp(mode, IMMEDIATE_MODE) != 0) {
            get_instruction_from_memory(MAR, MBR);
        }
    } else if (strcmp(command, STORE) == 0) {
        // Direct mode of addressing
        //    MAR <- IR<address field>
        // Immediate mode of addressing
        //    MAR <- IR<address field>
        //    MBR <- MAR
        copy_str(AC, MBR); //  MBR <- AC
        const int index = bin_to_int(MAR);
        if (index == 1) {
            copy_from_long_to_short(AC, XR);
        }
        copy_str(MBR, memory[index]);
    } else if (strcmp(command, BR) == 0) {
        copy_str(MAR, PC);
    } else if (strcmp(command, BREQ) == 0) {
        if (is_zero(AC)) {
            copy_str(MAR, PC);
        }
    } else if (strcmp(command, BRGE) == 0) {
        if (!is_negative(AC)) {
            copy_str(MAR, PC);
        }
    } else if (strcmp(command, BRLT) == 0) {
        if (is_negative(AC)) {
            copy_str(MAR, PC);
        }
    } else if (strcmp(command, HALT) == 0) {
        is_halt = 1;
    }
}


/**
 * Iteration loop that waits until HALT command is executed or MAX_ITER is overflowed
 * 1. PC -> IR (0 is passed, pc holds the index, and in IR we store the instruction with such index)
 * 2. Get and match mode from inst
 * 3. Get the command and match it
 * 4. Increment PC
 */
void instruction_cycle(void) {
    while (!is_halt && MAX_ITER > 0) {
        // Read the next instruction into the IR
        get_instruction_from_memory(PC, IR);
        // exec mode logic
        char mode[ADDRESS_MODE_LENGTH] = "00";
        store_address_mode(mode);
        match_address_mode(mode);
        // exec operation
        char operation[COMMAND_LENGTH] = "0000";
        store_command(operation);
        match_command(operation, mode);
        print_each_register();
        // Increment PC
        sum_two_bin(PC, "0000000001", PC);
        sleep(1);
        MAX_ITER--;
    }
}
