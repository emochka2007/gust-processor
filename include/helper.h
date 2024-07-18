#include "core.h"
void fill_with_zeros(char *bin, unsigned long len);

int index_of(char *arr[], int arr_len, char *elem);

void copy_str(char from[], char to[]);

void store_address_mode(char to[ADDRESS_MODE_LENGTH]);

void store_command(char to[COMMAND_LENGTH]);

void store_address(char to[ADDRESS_LENGTH]);
void copy_diff_len_str(char from[], char to[], unsigned long diff);
void add_prefix_zeros(char bin[], char to[], int prefix_len);
