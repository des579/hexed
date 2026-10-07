#ifndef FILE_H
#define FILE_H

#include <stdlib.h>
#include "array.h"

#define FILE_SAVE_SUCCESS 0
#define FILE_SAVE_ERROR 1


typedef struct change_s {
    size_t file_position;
    int ch;
    struct change_s* next;
} change_t;


int add_new_change(size_t file_position, int ch);

int save_array(array_t* array, FILE* fptr);
int save_changes(FILE* fptr);

void free_changes();

#endif // FILE_H
