#include "../include/core.h"
#include "../include/helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/math.h"


// todo make it read from IR
void print_decoded(char *bin, unsigned int size, int is_ir) {
    printf("BIN %s, ", bin);
    if (size == INSTRUCTION_LENGTH - 1 || is_ir == 1) {
        char command[COMMAND_LENGTH];
        char mode[ADDRESS_MODE_LENGTH];
        char address[ADDRESS_LENGTH];
        fill_with_zeros(command, COMMAND_LENGTH - 1);
        fill_with_zeros(mode, ADDRESS_MODE_LENGTH - 1);
        fill_with_zeros(address, ADDRESS_LENGTH - 1);
        get_bits_from_instruction(0, 3, command, bin);
        get_bits_from_instruction(4, 5, mode, bin);
        get_bits_from_instruction(6, 15, address, bin);

        const int command_index = index_of(BIN_COMMANDS_STR, 12, command);
        if (command_index == -1) {
            fprintf(stderr, "Command Index eq -1, %s\n", command);
            exit(1);
        }

        char *command_mnemo = COMMANDS_STR[command_index];
        int mode_index = index_of(BIN_MODES, 4, mode);
        if (mode_index == -1) {
            fprintf(stderr, "Mode Index eq -1\n");
            exit(1);
        }
        char *mode_mnemo = MNEMO_MODES[mode_index];
        const int value = bin_to_int(address);
        if (strcmp(command_mnemo, "HALT") == 0) {
            if (bin_to_int(bin) == 0) {
                printf("HALT\n");
            } else if (is_ir == 1) {
                printf("Size: %d, COMMAND:%s, MODE: %s, VALUE: %d (reading from IR)\n", size, command_mnemo, mode_mnemo,
                       value);
            } else {
                printf("DATA: %d\n", bin_to_int(bin));
            }
            return;
        }
        printf("Size: %d, COMMAND:%s, MODE: %s, VALUE: %d\n", size, command_mnemo, mode_mnemo, value);
    } else if (size == ADDRESS_LENGTH - 1) {
        printf("Size: %d, END/ORG %d\n", size, bin_to_int(bin));
    }
}

void print_memory(const int from, const int to) {
    for (int i = from; i <= to; i++) {
        printf("MEMORY index=[%d] - bin=[%s] - int[%d]\n", i, memory[i], bin_to_int(memory[i]));
    }
}

void print_each_register(void) {
    print_memory(0, 5);
    printf("IR %s|||", IR);
    printf("PC %s = %d |||", PC, bin_to_int(PC));
    // printf("MAR %s = %d |||", MAR, bin_to_int(MAR));
    // printf("MBR %s = %d |||", MBR, bin_to_int(MBR));
    printf("XR %s = %d |||", XR, bin_to_int(XR));
    printf("AC %s = %d |||", AC, bin_to_int(AC));
    printf("\n");
}

// void print_each_register(void) {
//     printf("\n================ Processor State ================\n");
//     printf("Instruction: ");
//     print_instruction();
//     printf("\nMemory (9-10):\n");
//     print_memory(9, 10);
//     printf("\n");

//     printf("Register States:\n");
//     printf("  IR  : %-16s\n", IR);
//     printf("  PC  : %-16s (Decimal: %d)\n", PC, bin_to_int(PC));
//     // Uncomment MAR and MBR if needed for debugging
//     // printf("  MAR : %-16s (Decimal: %d)\n", MAR, bin_to_int(MAR));
//     // printf("  MBR : %-16s (Decimal: %d)\n", MBR, bin_to_int(MBR));
//     printf("  XR  : %-16s (Decimal: %d)\n", XR, bin_to_int(XR));
//     printf("  AC  : %-16s (Decimal: %d)\n", AC, bin_to_int(AC));
//     printf("=================================================\n");
// }
