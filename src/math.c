#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/math.h"
#include "../include/helper.h"

void multiply_bin(char bin1[], char bin2[], char res[]) {
    const unsigned long bin2len = strlen(bin2);
    const unsigned long bin1len = strlen(bin1);
    assert(bin1len == bin2len);
    char temp[bin2len + 1];
    char multiplier[bin2len + 1];
    fill_with_zeros(temp, bin2len);
    fill_with_zeros(multiplier, bin2len);
    for (int i = 0; i < bin2len; i++) {
        multiply_bin_by_dig(bin1, bin2[i] - '0', temp);
        left_shift_binary(temp, bin2len - i - 1);
        sum_two_bin(multiplier, temp, multiplier);
    }
    copy_str(multiplier, res);
}

void multiply_bin_by_dig(char mul[], const int value, char res[]) {
    const unsigned long mul_len = strlen(mul);
    for (unsigned long i = 0; i < mul_len; i++) {
        const unsigned long index = mul_len - 1 - i;
        const int prod = (mul[index] - '0') * value;
        res[index] = (char) (prod + '0');
    }
    res[mul_len] = '\0';
}

void left_shift_binary(char bin[], const unsigned long offset) {
    const unsigned long bin_len = strlen(bin);

    for (unsigned long i = 0; i < bin_len - offset; i++) {
        bin[i] = bin[i + offset];
    }
    for (int i = 0; i < offset; i++) {
        bin[bin_len - 1 - i] = '0';
    }
}

int to_power(const int num, const int power) {
    int result = 1;
    for (int i = 0; i < power; i++) {
        result *= num;
    }
    return result;
}

void int_to_bin(const int num, const int len, char res[]) {
    // 10 -> 10 % 2 -> 5%2->2%2->1%2=1
    //       0-1-0-1 (1010)
    fill_with_zeros(res, len);
    int abs_num = abs(num);
    for (unsigned long i = len - 1; i > 0 && abs_num > 0; i--) {
        const char bin = abs_num % 2 + '0';
        abs_num = abs_num / 2;
        res[i] = bin;
    }
    /**If its negative then we use complement method*/
    if (num < 0) {
        twos_complement(res, res);
    }
    res[len] = '\0';
}

unsigned int bin_to_uint(char bin[]) {
    const unsigned long bin_len = strlen(bin);
    unsigned int res = 0;
    // 2 ^ 0 + 2 ^ 1 * 1 + 2 ^ 2 * 0 + 2 ^ 3 * 1
    int power = 0;
    for (unsigned long i = bin_len - 1; i >= 1; i--) {
        const int bin_int = bin[i] - '0';
        res += to_power(2, power) * bin_int;
        power++;
    }
    return res;
}

int bin_to_int(char bin[]) {
    const char sign = bin[0];
    if (sign == '0') {
        return bin_to_uint(bin);
    }
    if (sign == '1') {
        char temp_bin[strlen(bin)];
        fill_with_zeros(temp_bin, strlen(bin));
        twos_complement(bin, temp_bin);
        return bin_to_uint(temp_bin) * -1;
    }
    printf("Error in sign when bin_to_int_with_sign - sign found %c \n", sign);
    abort();
}

void sum_two_bin(char a[], char b[], char sum[]) {
    const unsigned long a_len = strlen(a);
    const unsigned long b_len = strlen(b);
    if (a_len != b_len) {
        printf("Error in sum_two_bin, diff in lengths a - %d, b - %d", a_len, b_len);
        abort();
    }
    int carry = 0;
    for (int i = a_len - 1; i >= 0; i--) {
        int internal_sum = a[i] - '0' + b[i] - '0' + carry;
        carry = 0;
        if (internal_sum == 2) {
            carry = 1;
            internal_sum = 0;
        }
        if (internal_sum == 3) {
            carry = 1;
            internal_sum = 1;
        }
        const char symbol = internal_sum + '0';
        sum[i] = symbol;
    }
    sum[a_len] = '\0';
}

void subtract_two_bin(const char *a, const char *b, char diff[]) {
    const unsigned long a_len = strlen(a);
    const unsigned long b_len = strlen(b);
    const unsigned long diff_len = strlen(diff);
    if (a_len != b_len && a_len != diff_len) {
        printf("Error in subtract_two_bin, diff in lengths");
        abort();
    }
    int carry = 0;
    for (int i = a_len - 1; i >= 0; i--) {
        int sub = (a[i] - '0') - (b[i] - '0') - carry;
        if (sub == -1) {
            carry = 1;
            sub = 1;
        } else if (sub == -2) {
            carry = 1;
            sub = 0;
        } else {
            carry = 0;
        }
        diff[i] = sub + '0';
    }
    diff[a_len] = '\0';
}

int index_of_bin_start(char bin[]) {
    const int bin_len = strlen(bin);
    for (unsigned long i = 0; i < bin_len; i++) {
        if (bin[i] == '1') {
            return i;
        }
    }
    return bin_len - 1;
}

void twos_complement(char from[], char inverted[]) {
    const unsigned long from_len = strlen(from);
    const unsigned long inverted_len = strlen(inverted);
    if (from_len != inverted_len) {
        printf("Error in twos_complement, diff in lengths\n");
        abort();
    }
    for (unsigned long i = 0; i < strlen(from); i++) {
        if (from[i] == '0') {
            inverted[i] = '1';
        } else if (from[i] == '1') {
            inverted[i] = '0';
        } else {
            printf("Error in twos_complement, from[i] not eq to 1 or 0\n");
            abort();
        }
    }
    increment_bin(inverted);
}

void increment_bin(char bin[]) {
    char dig[strlen(bin) + 1];
    for (unsigned long i = 0; i < strlen(bin); i++) {
        dig[i] = '0';
        if (i == strlen(bin) - 1) {
            dig[i] = '1';
        }
    }
    dig[strlen(bin)] = '\0';
    sum_two_bin(bin, dig, bin);
}

// SUBTRACTION
// The Complement Method
void sub_complement(char from[], char to[], char from_res[]) {
    char res_complement[INSTRUCTION_LENGTH];
    fill_with_zeros(res_complement, INSTRUCTION_LENGTH - 1);
    // to subtract - pass here
    twos_complement(to, res_complement);
    sum_two_bin(from, res_complement, from_res);
}
int is_zero(char value[]) {
    for (unsigned long i = 0; i < strlen(value); i++) {
        if (value[i] != '0') {
            return 0;
        }
    }
    return 1;
}

int is_negative(char value[]) {
    return value[0] == '1';
}

