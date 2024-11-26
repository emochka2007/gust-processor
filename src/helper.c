#include "../include/helper.h"
#include <assert.h>
#include <string.h>
#include "../include/core.h"
#include "../include/main.h"
#include <stdio.h>

void fill_with_zeros(char *bin, unsigned long len) {

    for (unsigned long i = 0; i < len; i++) {
        bin[i] = '0';
    }
    bin[len] = '\0';
}

void copy_str(char from[], char to[]) {
    printf("from %d\n", strlen(from));
    printf("to %d\n", strlen(to));
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

void add_prefix_zeros(char bin[], char to[], int prefix_len) {
    fill_with_zeros(to, INSTRUCTION_LENGTH);
    assert(strlen(to) == strlen(bin) + prefix_len);
    for (unsigned long i = 0; i < strlen(bin); i++) {
        to[prefix_len + i] = bin[i];
    }
}
