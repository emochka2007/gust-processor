#include "core.h"
void main_loop(void);
void get_bits_from_instruction(int start, int end, char *result);
void get_value_from_address(char bin_addr[ADDRESS_LENGTH], char value[INSTRUCTION_LENGTH - 1]);
void make_inst(char mnemo_command[], char mnemo_mode[], unsigned int num, unsigned int);
void store_bin_int(int, int);
void print_memory(int from, int to);