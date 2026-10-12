#include <stdio.h>

#include "array.h"

int main() {
    array_t* array = array_create();

    array_append(array, 0x0A);
    uint8_t val = 0x00;
    array_getc(array, 0, &val);
    printf("%02x\n", val);

    array_expand(array, 100);
    array_setc(array, 99, 0xBB);
    array_getc(array, 99, &val);
    printf("%02x\n", val);

    array_free(array);
    return 0;
}
