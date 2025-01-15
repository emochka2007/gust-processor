#include <stdio.h>
#include <stdlib.h>

#include "../include/core.h"
#include "../include/helper.h"
#include "../include/memory.h"
#include "../include/math.h"
#include "../include/debug.h"

FILE *read_file(const char *path) {
    FILE *file = fopen(path, "r");
    return file;
}

void read_from_file_and_store(FILE *file, unsigned int size, char *res) {
    int index = 0;
    while (index < size) {
        const int display = fgetc(file);
        if (display == EOF) {
            // printf("\n", display);
            // printf("Error in reading bytes from file %d\n", __LINE__);
            // exit(EXIT_FAILURE);
            return;
        }
        res[index] = display;
        index++;
    }
    res[size] = '\0';
    print_decoded(res, size);
    // printf("res - %s\n", res);
}

void read_from_file_and_store_into_memory() {
    FILE *file = NULL;
    file = read_file("./emulator.obj");
    fill_memory_with_zeros();
    char end_address[ADDRESS_LENGTH];
    fill_with_zeros(end_address, ADDRESS_LENGTH - 1);
    read_from_file_and_store(file, ADDRESS_LENGTH - 1, end_address);
    copy_str(end_address, PC);
    while (feof(file) == 0) {
        char org_address[ADDRESS_LENGTH];
        fill_with_zeros(org_address, ADDRESS_LENGTH - 1);
        read_from_file_and_store(file, ADDRESS_LENGTH - 1, org_address);

        char commands_length[ADDRESS_LENGTH];
        fill_with_zeros(commands_length, ADDRESS_LENGTH - 1);
        read_from_file_and_store(file, ADDRESS_LENGTH - 1, commands_length);
        const int command_index = bin_to_int(commands_length);
        const int org_index = bin_to_int(org_address);
        if (command_index == 0) {
            return;
        }
        char res[INSTRUCTION_LENGTH];
        fill_with_zeros(res, INSTRUCTION_LENGTH - 1);
        // printf("org_address - %d\n", bin_to_int(org_address));
        // printf("command length - %d\n", bin_to_int(commands_length));
        for (int i = org_index; i < org_index + command_index; i++) {
            read_from_file_and_store(file, INSTRUCTION_LENGTH - 1, res);
            copy_str(res, memory[i]);
        }
    }
}
