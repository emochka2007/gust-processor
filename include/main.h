#include "core.h"
void main_loop(void);
void get_bits_from_instruction(int start, int end, char *result);
void get_value_from_address(char bin_addr[ADDRESS_LENGTH], char value[INSTRUCTION_LENGTH - 1]);
void make_inst(char mnemo_command[], char mnemo_mode[], int num, char bin_str[INSTRUCTION_LENGTH]);