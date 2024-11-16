#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/core.h"
#include "../include/math.h"
#include "../include/helper.h"
#include "../include/main.h"
#include "../include/debug.h"

int is_halt = 0;

void get_bits_from_instruction(int start, int end, char result[]) {
    for (int i = start, j = 0; i <= end; i++, j++) {
        result[j] = IR[i];
    }
}

void get_value_from_address(char bin_addr[ADDRESS_LENGTH], char value[INSTRUCTION_LENGTH - 1]) {
    int index = bin_to_int(bin_addr);
    if (index >= MEMORY_LENGTH) {

        fprintf(stderr, "Index out of range\n");
        abort();
    }
    for (int i = 0; i < INSTRUCTION_LENGTH - 1; i++) {
        value[i] = memory[index][i];
    }
}

void address_mode(char mode[ADDRESS_MODE_LENGTH]) {
    store_address(MAR);
    if (strcmp(mode, DIRECT_MODE) == 0) {
    } else if (strcmp(mode, IMMEDIATE_MODE) == 0) {

        copy_diff_len_str(MAR, MBR, 16 - 10);
    } else {
        fprintf(stderr, "Mode not found\n");
        abort();
    }
}

int valid_zero_value(char value[]) {
    for (unsigned long i = 0; i < strlen(value); i++) {
        if (value[i] != '0') {
            return 0;
        }
    }
    return 1;
}

void instruction_steps(char inst[COMMAND_LENGTH], char mode[ADDRESS_MODE_LENGTH]) {
    if (strcmp(inst, LOAD) == 0) {
        if (strcmp(mode, IMMEDIATE_MODE) != 0) {
            get_value_from_address(MAR, MBR);
        }
        copy_str(MBR, AC);
    } else if (strcmp(inst, ADD) == 0) {
        if (strcmp(mode, IMMEDIATE_MODE) != 0) {
            get_value_from_address(MAR, MBR);
        }
        sum_two_bin(AC, MBR, AC);
    } else if (strcmp(inst, SUB) == 0) {
        if (strcmp(mode, IMMEDIATE_MODE) != 0) {
            get_value_from_address(MAR, MBR);
        }
        subtract_two_bin(AC, MBR, AC);
    } else if (strcmp(inst, MUL) == 0) {
        if (strcmp(mode, IMMEDIATE_MODE) != 0) {
            get_value_from_address(MAR, MBR);
        }
        mult_two_bin(MBR, AC, AC);
    } else if (strcmp(inst, DIV) == 0) {
        if (strcmp(mode, IMMEDIATE_MODE) != 0) {
            get_value_from_address(MAR, MBR);
        }
    } else if (strcmp(inst, STORE) == 0) {
        copy_str(AC, MBR);
        int index = bin_to_int(MAR);
        copy_str(MBR, memory[index]);
    } else if (strcmp(inst, BR) == 0) {
        copy_str(MAR, PC);
    } else if (strcmp(inst, BREQ) == 0) {
        if (valid_zero_value(AC)) {
            copy_str(MAR, PC);
        }
    } else if (strcmp(inst, HALT) == 0) {
        is_halt = 1;
    }
    // BRGE & BRLT not implemented
}

void instruction_cycle(void) {
    int max_iter = 0;
    while (!is_halt && max_iter < 1000) {
        // print_each_register();
        // Read the next instruction into the IR
        get_value_from_address(PC, IR);
        char mode[ADDRESS_MODE_LENGTH] = "00";
        store_address_mode(mode);
        address_mode(mode);

        sum_two_bin(PC, "0000000001", PC);

        // Perform the operation
        char operation[COMMAND_LENGTH] = "0000";
        store_command(operation);
        instruction_steps(operation, mode);
//        print_instruction();
        max_iter++;
    }
}

void main_loop(void) {
    for (int i = 0; i < 1024; i++) {
        for (int j = 0; j < INSTRUCTION_LENGTH; j++) {
            memory[i][j] = '0';
        }
        memory[i][INSTRUCTION_LENGTH - 1] = '\0';
    }
    // INSTRUCTION CYCLE
    // LOAD =42 0001010000101010
//    char memory_cell[INSTRUCTION_LENGTH] = "0001010000101010";
//    copy_str(memory_cell, memory[0]);
//    // 3
//    // add direct 4
//    char add_memory_cell[INSTRUCTION_LENGTH] = "1000000000000100";
//    copy_str(add_memory_cell, memory[1]);
//    char add_int[ADDRESS_LENGTH] = "0000000011";
//    copy_diff_len_str(add_int, memory[4], INSTRUCTION_LENGTH - ADDRESS_LENGTH);
    char res[INSTRUCTION_LENGTH] = "0000000000000000";
    make_inst("LOAD", "=", 42, res);
//    printf("memory[0] %s\n", memory[0]);
//    printf("res %s\n", res);
    copy_str(res, memory[0]);
    make_inst("SUB", "=", 1, res);
    copy_str(res, memory[1]);
    make_inst("BREQ", " ", 4, res);
    copy_str(res, memory[2]);
    make_inst("BR", " ", 1, res);
    copy_str(res, memory[3]);
//    printf("%s res\n", res);
    instruction_cycle();
}

void make_inst(char mnemo_command[], char mnemo_mode[], int num, char bin_str[INSTRUCTION_LENGTH]) {
    int command_index = index_of(COMMANDS_STR, 12, mnemo_command);
    if (command_index == -1) {
        fprintf(stderr, "Index eq -1\n");
        abort();
    }
    char *command = BIN_COMMANDS_STR[command_index];
    int mode_index = index_of(MNEMO_MODES, 4, mnemo_mode);
    if (mode_index == -1) {
        fprintf(stderr, "Mode Index out of range. Line %d\n", __LINE__);
        abort();
    }
    char *mode = BIN_MODES[mode_index];
    char bin_int[ADDRESS_LENGTH] = "0000000000";
    int_to_bin(num, ADDRESS_LENGTH - 1, bin_int);
    for (int i = 0; i < 4; i++) {
        bin_str[i] = command[i];
    }
    for (int i = 4, j = 0; i < 6; i++, j++) {
        bin_str[i] = mode[j];
    }
    for (int i = 6, j = 0; i < 17; i++, j++) {
        bin_str[i] = bin_int[j];
    }
}

int main(void) {
    char left[] = "0011";
    char right[] = "0010";
    char res[3] = "0000"; //0110
     mult_two_bin(left, right, res);
    printf("%s\n", res);
    // main_loop();
    return 0;
}
