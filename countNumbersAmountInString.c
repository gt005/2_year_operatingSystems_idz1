#include <stdio.h>

int countNumbersAmountInString(const char *str) {
    int count = 0;
    int i = 0;
    while (str[i] != '\0') {
        if (str[i] >= '0' && str[i] <= '9') {
            count++;
            while (str[i] >= '0' && str[i] <= '9') {
                i++;
            }
        }
        i++;
    }
    return count;
}