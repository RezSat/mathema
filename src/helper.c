#include <stdlib.h>
#include <string.h>

#include "include/helper.h"

char *duplicate_string(const char *src) {
    size_t length = strlen(src) + 1; // Include space for the null terminator
    char *dest = malloc(length);

    if (dest != NULL) {
        memcpy(dest, src, length);
    }

    return dest;
}