#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#include "array.h"

static int array_allocate_block(array_t* array) {
    uint8_t* block = malloc(sizeof(uint8_t) * ARRAY_BLOCK_SIZE);
    if (!block) 
        return ARRAY_ACTION_IERROR;
    
    uint8_t** tmp = reallocarray(array->array,
            array->amount_of_blocks + 1, sizeof(uint8_t*));
    if (!tmp) {
        free(block);
        return ARRAY_ACTION_IERROR;
    }

    tmp[array->amount_of_blocks] = block;
    array->array = tmp;
    array->amount_of_blocks++;
    array->max_size += ARRAY_BLOCK_SIZE;
    
    return ARRAY_ACTION_ISUCCESS;
}

array_t* array_create() {
    array_t* array = (array_t*) calloc(1, sizeof(array_t));
    if (!array)
        return NULL;

    array->array = (uint8_t**) calloc(1, sizeof(uint8_t*));
    if (!array->array) {
        free(array);
        return NULL;
    }

    return array;
}

void array_free(array_t* array) {
    for (size_t i = 0; i < array->amount_of_blocks; i++) {
        if (array->array[i])
            free(array->array[i]);
    }
    free(array->array);
    free(array);
}

int array_append(array_t* array, uint8_t val) {
    // allocate a new block if necessary
    if (array->size >= array->max_size)
        if (array_allocate_block(array) == ARRAY_ACTION_IERROR)
            return ARRAY_ACTION_IERROR;

    uint32_t block = array->size / ARRAY_BLOCK_SIZE;
    uint32_t index = array->size % ARRAY_BLOCK_SIZE;

    array->array[block][index] = val;
    array->size++;

    return ARRAY_ACTION_ISUCCESS;
}

int array_setcyx(array_t* array, uint32_t row, uint32_t col, uint8_t val) {
    if (row > array->max_size || col > ARRAY_BLOCK_SIZE)
        return ARRAY_ACTION_IERROR;

    array->array[row][col] = val;

    return ARRAY_ACTION_ISUCCESS;
}


int array_getcyx(array_t* array, uint32_t row, uint32_t col, uint8_t* buffer) {
    if (row > array->max_size || col > ARRAY_BLOCK_SIZE)
        return ARRAY_ACTION_IERROR;

    *buffer = array->array[row][col];

    return ARRAY_ACTION_ISUCCESS;
}

int array_setc(array_t* array, uint32_t index, uint8_t val) {
    if (index >= array->max_size)
        return ARRAY_ACTION_IERROR;
    uint32_t row = index / ARRAY_BLOCK_SIZE;
    uint32_t col = index % ARRAY_BLOCK_SIZE;

    array->array[row][col] = val;
    if (index > array->size)
        array->size = index;

    return ARRAY_ACTION_ISUCCESS;
}

int array_getc(array_t* array, uint32_t index, uint8_t* buffer) {
    if (index >= array->size)
        return ARRAY_ACTION_IERROR;
    uint32_t row = index / ARRAY_BLOCK_SIZE;
    uint32_t col = index % ARRAY_BLOCK_SIZE;

    *buffer = array->array[row][col];

    return ARRAY_ACTION_ISUCCESS;
}

int array_expand(array_t *array, uint32_t target_size) {
    if (target_size <= array->size)
        return ARRAY_ACTION_INOTHING;
    if (target_size <= array->max_size) {
        array->size = target_size;
        return ARRAY_ACTION_INOTHING;
    }

    while (array->max_size < target_size) {
        if (array_allocate_block(array) == ARRAY_ACTION_IERROR)
            return ARRAY_ACTION_IERROR;
    }

    array->size = target_size;

    return ARRAY_ACTION_ISUCCESS;
}

int array_appendstr(array_t* array, uint8_t* vals, uint32_t size) {
    uint32_t prev_size = array->size;
    if (array_expand(array, array->size + size) == ARRAY_ACTION_IERROR)
        return ARRAY_ACTION_IERROR;

    for (uint32_t i = 0; i < size; i++) {
        array_setc(array, i + prev_size, vals[i]);
    }

    return ARRAY_ACTION_ISUCCESS;
}

