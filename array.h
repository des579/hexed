#ifndef ARRAY_H
#define ARRAY_H

// made for strings only

#include <stdint.h>

#define ARRAY_BLOCK_SIZE 128

#define ARRAY_ACTION_ISUCCESS 0
#define ARRAY_ACTION_IERROR 1
#define ARRAY_ACTION_INOTHING 2


typedef struct {
    // effective size of the array
    uint32_t size;
    uint32_t amount_of_blocks;

    // maximum size of the array
    // a flag that shows whether it is necessary to allocate another block or no
    uint32_t max_size;
    uint8_t **array;
} array_t;

array_t* array_create();
void array_free(array_t* array);

int array_append(array_t* array, uint8_t val);
int array_setcyx(array_t* array, uint32_t row, uint32_t col, uint8_t val);
int array_getcyx(array_t* array, uint32_t row, uint32_t col, uint8_t* buffer);
int array_getc(array_t* array, uint32_t index, uint8_t* buffer);
int array_setc(array_t* array, uint32_t index, uint8_t val);
int array_expand(array_t* array, uint32_t target_size);

// TODO
int array_appendstr(array_t* array, uint8_t* vals, uint32_t len);


#endif // ARRAY_H
