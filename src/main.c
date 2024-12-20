#include <stdio.h>

#include "../include/core.h"
#include "../include/file.h"

int main(void) {
    read_from_file_and_store_into_memory();
    instruction_cycle();
    printf("AC - %s\n", AC);
    return 0;
}
