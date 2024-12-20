#include "../include/core.h"
#include "../include/helper.h"
#include "../include/math.h"

void fill_memory_with_zeros(void) {
    for (int i = 0; i < 1024; i++) {
        for (int j = 0; j < INSTRUCTION_LENGTH; j++) {
            memory[i][j] = '0';
        }
        memory[i][INSTRUCTION_LENGTH - 1] = '\0';
    }
}

void store_bin_int(const int num, const int memory_index) {
    char bin[INSTRUCTION_LENGTH];
    fill_with_zeros(bin, INSTRUCTION_LENGTH - 1);
    int_to_bin(num, INSTRUCTION_LENGTH - 1, bin);
    copy_str(bin, memory[memory_index]);
}

