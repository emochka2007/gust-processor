#include "../include/helper.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/math.h"

void fill_with_zeros(char *bin, unsigned long len) {
    for (unsigned long i = 0; i < len; i++) {
        bin[i] = '0';
    }
    bin[len] = '\0';
}

void copy_str(char from[], char to[]) {
    assert(strlen(from) == strlen(to));
    for (unsigned long i = 0; i < strlen(from); i++) {
        to[i] = from[i];
    }
}

int index_of(char *arr[], int arr_len, char *elem) {
    for (int i = 0; i < arr_len; i++) {
        if (strcmp(arr[i], elem) == 0) {
            return i;
        }
    }
    return -1;
}

void store_command(char to[COMMAND_LENGTH]) {
    get_bits_from_instruction(0, 3, to);
}

void store_address_mode(char to[ADDRESS_MODE_LENGTH]) {
    get_bits_from_instruction(4, 5, to);
}

/**
 * 
 * @param to Value we address from
 * We store from 6th to 15th byte
 */
void store_address(char to[ADDRESS_LENGTH]) {
    get_bits_from_instruction(6, 15, to);
}

void copy_diff_len_str(char from[], char to[], unsigned long diff) {
    assert(strlen(from) != strlen(to));
    for (unsigned long i = 0; i < diff; i++) {
        to[i] = '0';
    }
    for (unsigned long i = diff, j = 0; i < strlen(to); i++, j++) {
        to[i] = from[j];
    }
}

void copy_from_long_to_short(char from_long[], char to_short[]) {
    const int from_len = strlen(from_long);
    const int to_len = strlen(to_short);
    assert(from_len>to_len);
    for (int i = 1; i <= to_len; i++) {
        to_short[to_len - i] = from_long[from_len - i];
    }
}

void make_inst(char mnemo_command[], char mnemo_mode[], const unsigned int num, const unsigned int memory_index) {
    char bin_str[INSTRUCTION_LENGTH];
    fill_with_zeros(bin_str, INSTRUCTION_LENGTH - 1);
    const int command_index = index_of(COMMANDS_STR, 12, mnemo_command);
    if (command_index == -1) {
        fprintf(stderr, "Index eq -1\n");
        abort();
    }
    const char *command = BIN_COMMANDS_STR[command_index];
    const int mode_index = index_of(MNEMO_MODES, 4, mnemo_mode);
    if (mode_index == -1) {
        fprintf(stderr, "Mode Index out of range. Line %d\n", __LINE__);
        exit(EXIT_FAILURE);
    }
    const char *mode = BIN_MODES[mode_index];
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
    copy_str(bin_str, memory[memory_index]);
}
