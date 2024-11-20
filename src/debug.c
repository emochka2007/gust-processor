#include "../include/core.h"
#include "../include/helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/main.h"
#include "../include/math.h"

void print_instruction(void)
{
    char command[COMMAND_LENGTH] = "0000";
    char mode[3] = "00";
    char address[ADDRESS_LENGTH] = "0000000000";
    store_address(address);
    store_address_mode(mode);
    store_command(command);
    int command_index = index_of(BIN_COMMANDS_STR, 12, command);
    if (command_index == -1)
    {
        fprintf(stderr, "Index eq -1\n");
        abort();
    }
    char *command_mnemo = COMMANDS_STR[command_index];
    int mode_index = index_of(BIN_MODES, 4, mode);
    if (mode_index == -1)
    {
        abort();
    }
    char *mode_mnemo = MNEMO_MODES[mode_index];
    int value = bin_to_int(address);
    if (strcmp(command_mnemo, "HALT") == 0)
    {
        printf("HALT\n");
        return;
    }
    printf("%s %s%d\n", command_mnemo, mode_mnemo, value);
}

void print_each_register(void)
{
    printf("IR %s\n", IR);
    printf("PC %s\n", PC);
    printf("MAR %s\n", MAR);
    printf("MBR %s\n", MBR);
    printf("XR %s\n", XR);
    printf("AC %s\n", AC);
}