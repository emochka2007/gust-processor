#include "../include/core.h"
#include "../include/helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/math.h"

void print_instruction(void) {
    char command[COMMAND_LENGTH] = "0000";
    char mode[3] = "00";
    char address[ADDRESS_LENGTH] = "0000000000";
    store_address(address);
    store_address_mode(mode);
    store_command(command);
    const int command_index = index_of(BIN_COMMANDS_STR, 12, command);
    if (command_index == -1) {
        fprintf(stderr, "Index eq -1\n");
        abort();
    }
    char *command_mnemo = COMMANDS_STR[command_index];
    int mode_index = index_of(BIN_MODES, 4, mode);
    if (mode_index == -1) {
        abort();
    }
    char *mode_mnemo = MNEMO_MODES[mode_index];
    const int value = bin_to_int(address);
    if (strcmp(command_mnemo, "HALT") == 0) {
        printf("HALT\n");
        return;
    }
    printf("%s %s%d\n", command_mnemo, mode_mnemo, value);
}

void print_memory(const int from, const int to) {
    for (int i = from; i <= to; i++) {
        printf("MEMORY index=[%d] - bin=[%s] - int[%d]\n", i, memory[i], bin_to_int(memory[i]));
    }
}

void print_each_register(void) {
    // printf("IR %s\n", IR);
    // printf("PC %s\n", PC);
    // printf("MAR %s\n", MAR);
    // printf("MBR %s\n", MBR);
    // printf("XR %s\n", XR);
    // printf("AC %s\n", AC);
    // print_memory(0, 1);
    print_memory(100, 107);
    // print_memory(110, 115);
    print_instruction();
    printf("IR %s|||", IR);
    printf("PC %s = %d |||", PC, bin_to_int(PC));
    printf("MAR %s = %d |||", MAR, bin_to_int(MAR));
    printf("MBR %s = %d |||", MBR, bin_to_int(MBR));
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
