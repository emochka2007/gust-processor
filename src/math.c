#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/math.h"
#include "../include/helper.h"

//     17
//     15
//     85
//    17
//    255
void mult_two_bin(char bin1[], char bin2[], char res[]) {
    assert(strlen(bin1) == strlen(bin2));
    char temp[strlen(bin2)];
    fill_with_zeros(temp, strlen(bin2));
    for (int i=0; i<strlen(bin2); i++){
        mult_bin_by_digit(bin1, bin2[i] - '0', temp);
        left_shift_bin(temp, strlen(bin2) - i - 1);
        sum_two_bin(res, temp, res);
    }
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
    fill_with_zeros(res, len);
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
void twos_complement_bin() {
    
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
void twos_complement(char from[], char inverted[]) {
    unsigned long from_len = strlen(from);
    unsigned long inverted_len = strlen(inverted);
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
void sub_complement(char from[], char to[], char res[]) {
    twos_complement(to, res);
    sum_two_bin(from, res, res);
}
//TODO fix
// void sub_and_print_bin(char from[], char to[], char res[]) {
//     printf("%s FROM\n", from);
//     printf("%s to\n", to);
//     sub_complement(from, to, res);
//     signed char sign = res[0];
//     printf("%s res\n", res);
//     if (sign == '1') {
//         char negative_res[strlen(res)];
//         fill_with_zeros(negative_res, strlen(res));
//         twos_complement(res, negative_res);
//         printf("-%d line:%d\n", bin_to_int_with_sign(negative_res), __LINE__);
//     } else if (sign == '0') {
//         printf("%d line:%d\n", bin_to_int(res), __LINE__);
//     } else {
//         abort();
//     }
// }