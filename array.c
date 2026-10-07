#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#include "array.h"


array_t* create_array() {
    array_t* array = (array_t*) calloc(1, sizeof(array_t));
    if (!array)
        return NULL;

    array->array = (char**) calloc(1, sizeof(char*));
    if (!array->array)
        return NULL;

    return array;
}

void arrfree(array_t* array) {
    if (!array) return;

    for (size_t i = 0; i < array->amount_of_blocks; i++) {
        if (array->array[i])
            free(array->array[i]);
    }
    free(array->array);
    free(array);
}

int set_char(array_t* array, char c, size_t index) {
    if (index >= array->max_size)
        return SET_ERROR;

    size_t block = index / BLOCK_SIZE;
    size_t index_at_block = index % BLOCK_SIZE;

    array->array[block][index_at_block] = c;

    return SET_SUCCESS;
}

int add_new_block(array_t* array) {
    char *block = (char*) calloc(BLOCK_SIZE, sizeof(char));
    if (!block)
        return BLOCK_ADD_ERROR;

    char** tmp = realloc(array->array, (array->amount_of_blocks + 1) * sizeof(char*));
    if (!tmp) {
        free(block);
        return BLOCK_ADD_ERROR;
    }
    tmp[array->amount_of_blocks] = block;
    array->array = tmp;
    array->amount_of_blocks += 1;
    array->max_size = array->amount_of_blocks * BLOCK_SIZE;

    return BLOCK_ADD_SUCCESS;
}

int char_append(array_t* array, char data) {
    if (!array)
        return APPEND_ERROR;
    
    if (array->size + 1 >= array->max_size) {
        int block_add_result = add_new_block(array);
        if (block_add_result != BLOCK_ADD_SUCCESS)
            return APPEND_ERROR;

        size_t current_block = array->size / BLOCK_SIZE;
        size_t block_index = array->size % BLOCK_SIZE;

        array->array[current_block][block_index] = data;

        return APPEND_SUCCESS;
    }

    size_t current_block = array->size / BLOCK_SIZE;
    size_t block_index = array->size % BLOCK_SIZE;

    array->array[current_block][block_index] = data;
    array->size++;

    return APPEND_SUCCESS;
}

int set_block(array_t* array, char *new_block, size_t block_index) {
    if (!array || !new_block || block_index >= array->amount_of_blocks)
        return BLOCK_SET_ERROR;

    size_t len = strlen(new_block);
    len = MIN(len, BLOCK_SIZE);

    for (size_t i = 0; i < BLOCK_SIZE; i++) {
        if (i < len)
            array->array[block_index][i] = new_block[i];
        else 
            array->array[block_index][i] = 0;
    }

    return BLOCK_SET_SUCCESS;
}

char* get_block(array_t* array, size_t block_index) {
    if (!array || block_index >= array->amount_of_blocks)
        return NULL;

    char* block = calloc(BLOCK_SIZE + 1, sizeof(char));
    if (!block)
        return NULL;

    strncpy(block, array->array[block_index], BLOCK_SIZE);
    block[BLOCK_SIZE] = 0;

    return block;
}




