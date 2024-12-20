#include <stdio.h>
#include <_stdlib.h>

#include "../include/core.h"
#include "../include/helper.h"
#include "../include/memory.h"
#include "../include/math.h"

FILE *read_file(const char *path) {
    FILE *file = fopen(path, "r");
    return file;
}

void read_from_file_and_store(FILE *file, unsigned int size, char *res) {
    int index = 0;
    while (index < size) {
        const int display = fgetc(file);
        if (display == EOF) {
            printf("\n", display);
            printf("Error in reading bytes from file %d\n", __LINE__);
            exit(EXIT_FAILURE);
        }
        res[index] = display;
        index++;
    }
    res[size] = '\0';
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
