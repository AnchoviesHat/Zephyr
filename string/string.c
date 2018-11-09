#include "../common.h"

u64 strlen(char *str) {
    u64 len = 0;
    while (*str++ != '\0') {
        ++len;
    }
    return len;
}

void strcpy(char *src, char *dest) {
    while(*src != '\0') {
        dest = src;
    }
}