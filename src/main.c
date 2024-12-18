#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/core.h"
#include "../include/math.h"
#include "../include/helper.h"
#include "../include/main.h"
#include "../include/debug.h"

int is_halt = 0;
unsigned int MAX_ITER = 100;

void print_memory(int from, int to) {
    for (int i = from; i <= to; i++) {
        printf("MEMORY index=[%d] - bin=[%s] - int[%d]\n", i, memory[i], bin_to_int(memory[i]));
    }
}

FILE *read_file(char *path) {
    FILE *file;
    file = fopen(path, "r");
    return file;
}

void read_from_file_and_store(FILE *file, unsigned int size, char *res) {
    // printf("%s - res in read_from file\n", res);
    int dispay;
    int index = 0;
    while (index < size) {
        dispay = fgetc(file);
        if (dispay == EOF) {
            printf("\n", dispay);
            printf("Error in reading bytes from file %d\n", __LINE__);
            abort();
        }
        // printf("%c", dispay);
        // printf("index - %d\n", index);
        res[index] = dispay;
        index++;
    }
    res[size] = '\0';
}

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
    } else if (strcmp(mode, INDEXED_MODE) == 0) {
        sum_two_bin(MAR, MBR, MAR);
        printf("MAR %s \n", MAR);
    } else {
        fprintf(stderr, "Mode not found\n");
        abort();
    }
}

int is_zero(char value[]) {
    // printf("%s - value \n", value);
    // printf("%d \n", bin_to_int(value));
    for (unsigned long i = 0; i < strlen(value); i++) {
        if (value[i] != '0') {
            return 0;
        }
    }
    return 1;
}

int is_negative(char value[]) {
    return value[0] == '1';
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
        sub_complement(AC, MBR, AC);
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
        // Direct mode of addressing
        //    MAR <- IR<address field>
        // Immediate mode of addressing
        //    MAR <- IR<address field>
        //    MBR <- MAR
        copy_str(AC, MBR); //  MBR <- AC

        int index = bin_to_int(MAR);
        if (index == 1) {
            printf("index %d \n", index);
            copy_from_long_to_short(AC, XR);
        }
        copy_str(MBR, memory[index]);
    } else if (strcmp(inst, BR) == 0) {
        copy_str(MAR, PC);
    } else if (strcmp(inst, BREQ) == 0) {
        if (is_zero(AC)) {
            copy_str(MAR, PC);
        }
    } else if (strcmp(inst, BRGE) == 0) {
        if (!is_negative(AC)) {
            copy_str(MAR, PC);
        }
    } else if (strcmp(inst, BRLT) == 0) {
        if (is_negative(AC)) {
            copy_str(MAR, PC);
        }
    } else if (strcmp(inst, HALT) == 0) {
        is_halt = 1;
    }
}


void instruction_cycle(void) {
    int max_iter = 0;
    while (!is_halt && max_iter < MAX_ITER) {
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
        // print_each_register();
        instruction_steps(operation, mode);
        print_each_register();
        max_iter++;
    }
}

void fill_memory_with_zeros() {
    for (int i = 0; i < 1024; i++) {
        for (int j = 0; j < INSTRUCTION_LENGTH; j++) {
            memory[i][j] = '0';
        }
        memory[i][INSTRUCTION_LENGTH - 1] = '\0';
    }
}

void main_loop(void) {
    char res[INSTRUCTION_LENGTH];
    fill_with_zeros(res, INSTRUCTION_LENGTH - 1);
    instruction_cycle();
}

void store_bin_int(int num, int memory_index) {
    char bin[INSTRUCTION_LENGTH];
    fill_with_zeros(bin, INSTRUCTION_LENGTH - 1);
    int_to_bin(num, INSTRUCTION_LENGTH - 1, bin);
    copy_str(bin, memory[memory_index]);
}

void make_inst(char mnemo_command[], char mnemo_mode[], unsigned int num, unsigned int memory_index) {
    char bin_str[INSTRUCTION_LENGTH];
    fill_with_zeros(bin_str, INSTRUCTION_LENGTH - 1);
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
    // printf("%s bin_str\n", bin_str);
    copy_str(bin_str, memory[memory_index]);
    // printf("%s bin_int\n", bin_int);
}

void read_from_file_and_store_into_memory() {
    FILE *file = NULL;
    file = read_file("./emulator.obj");
    char commands_length[ADDRESS_LENGTH];
    fill_with_zeros(commands_length, ADDRESS_LENGTH - 1);
    read_from_file_and_store(file, ADDRESS_LENGTH - 1, commands_length);
    char res[INSTRUCTION_LENGTH];
    fill_with_zeros(res, INSTRUCTION_LENGTH - 1);
    fill_memory_with_zeros();
    for (int i = 0; i < bin_to_int(commands_length); i++) {
        read_from_file_and_store(file, INSTRUCTION_LENGTH - 1, res);
        copy_str(res, memory[i]);
    }
}

// int fact(int n)
// {
//     int sum = 1;
//     for (; n >= 1; n--)
//     {
//         sum *= n;
//     }
//     return sum;
// }
int main(void) {
    read_from_file_and_store_into_memory();
    main_loop();
    return 0;
}
