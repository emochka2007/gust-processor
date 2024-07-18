#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/math.h"
#include "../include/helper.h"

void mult_two_bin(char bin1[], char bin2[], char res[]) {
    assert(strlen(bin1) == strlen(bin2));
    char summary[INSTRUCTION_LENGTH];
    fill_with_zeros(summary, INSTRUCTION_LENGTH-1);

    char expanded_bin1[INSTRUCTION_LENGTH];
    add_prefix_zeros(bin1, expanded_bin1, INSTRUCTION_LENGTH - strlen(bin2));
    for (unsigned long i = 0; i < strlen(bin2); i++) {
        printf("summary: %s\n", summary);
        char mult_res[INSTRUCTION_LENGTH];
        fill_with_zeros(mult_res, INSTRUCTION_LENGTH - 1);
        printf("mul: %s\n", mult_res);
        int current_digit = bin2[strlen(bin2) - 1 - i] - '0';
        printf("Current digit: %d\n", current_digit);
        printf("expanded bin digit: %s\n", expanded_bin1);

        mult_bin_by_digit(expanded_bin1, current_digit, mult_res);
        left_shift_bin(mult_res, i);
        printf("summary: %s\n", summary);
        printf("mul: %s\n", mult_res);
//        sum_two_bin(mult_res, sum, sum);
    }
//    unsigned long sum_len = strlen(sum);
//    unsigned long res_len = strlen(res);
//    printf("Length of sum: %d\n", sum_len);
//    printf("Length of result array: %d\n", res_len);
//    copy_str(sum, res);
}

void mult_bin_by_digit(char mul[], int value, char res[]) {
    unsigned long mul_len = strlen(mul);
    for (unsigned long i = 0; i < mul_len; i++) {
        unsigned long index = mul_len - 1 - i;
        int prod = (mul[index] - '0') * value;
        res[index] = prod + '0';
    }
    res[mul_len] = '\0';
}

void left_shift_bin(char bin[], int offset) {
    unsigned long bin_len = strlen(bin);

    for (unsigned long i = 0; i < bin_len - offset; i++) {
        bin[i] = bin[i + offset];
    }
    for (int i = 0; i < offset; i++) {
        bin[bin_len - 1 - i] = '0';
    }
}

int to_power(int num, int power) {

    if (power == 0) {
        return 1;
    }

    return num * to_power(num, power - 1);
}

void int_to_bin(int num, int len, char res[]) {
    // 10 -> 10 % 2 -> 5%2->2%2->1%2=1
    //       0-1-0-1 (1010)
    for (int i = len - 1; i >= 0 && num > 0; i--) {
        char bin = num % 2 + '0';
        num = num / 2;
        res[i] = bin;
    }
    res[len] = '\0';
}

int bin_to_int(char bin[]) {
    unsigned long bin_len = strlen(bin);
    int res = 0;
    // 2 ^ 0 + 2 ^ 1 * 1 + 2 ^ 2 * 0 + 2 ^ 3 * 1
    int power = 0;
    for (int i = bin_len - 1; i >= 0; i--) {
        int bin_int = bin[i] - '0';
        res += to_power(2, power) * bin_int;
        power++;
    }
    return res;
}

void sum_two_bin(char *a, char *b, char sum[]) {
    unsigned long a_len = strlen(a);
    unsigned long b_len = strlen(b);
    if (a_len != b_len) {
        printf("Error in sum_two_bin, diff in lengths");
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
        char symbol = internal_sum + '0';
        sum[i] = symbol;
    }
    sum[a_len] = '\0';
}

void subtract_two_bin(char *a, char *b, char diff[]) {
    unsigned long a_len = strlen(a);
    unsigned long b_len = strlen(b);
    unsigned long diff_len = strlen(diff);
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
        }
        else if (sub == -2) {
            carry = 1;
            sub = 0;
        } else {
            carry = 0;
        }
        diff[i] = sub + '0';
    }
    diff[a_len] = '\0';
}
