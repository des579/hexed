#include <stdlib.h>
#include <ncurses.h>

#include "array.h"
#include "file.h"

static change_t* first_change = NULL;
static change_t* last_change = NULL;


static int hex_to_char(char* hex);


int save_changes(FILE* fptr) {
    if (!fptr)
        return FILE_SAVE_ERROR;
    /*
    change_t* current = first_change;
    while (current) {
        fseek(fptr, current->file_position, SEEK_SET);
        int ch = hex_to_char(str + index*2);
        fputc(ch, fptr);

        current = current->next;
    }
    */

    return FILE_SAVE_SUCCESS;
}

int save_array(array_t* array, FILE* fptr) {
    if (!array || !fptr)
        return FILE_SAVE_ERROR;


    fseek(fptr, 0, SEEK_SET);
    for (size_t block = 0; block < array->amount_of_blocks; block++) {
        char* str = get_block(array, block);

        for (size_t index = 0; index < BLOCK_SIZE / 2; index++) {
            int ch = hex_to_char(str + index*2);
            fputc(ch, fptr);
        }
        free(str);
    }

    return FILE_SAVE_SUCCESS;
}

int add_new_change(size_t file_position, int ch) {
    change_t* new = calloc(1, sizeof(change_t));
    if (!new)
        return 1;
    new->file_position = file_position;
    new->ch = ch;

    if (!last_change)
        first_change = new;
    else
        last_change->next = new;
    last_change = new;

    return 0;
}

void free_changes() {
    change_t* current = first_change;
    while (current) {
        change_t* tmp = current;
        current = current->next;
        free(tmp);
    }
}

static int hex_to_char(char* hex) {
    int hi = hex[0];
    int lo = hex[1];

    hi = (hi >= '0' && hi <= '9') ? hi - '0' :
         (hi >= 'A' && hi <= 'F') ? hi - 'A' + 10 :
         (hi >= 'a' && hi <= 'f') ? hi - 'a' + 10 : -1;

    lo = (lo >= '0' && lo <= '9') ? lo - '0' :
         (lo >= 'A' && lo <= 'F') ? lo - 'A' + 10 :
         (lo >= 'a' && lo <= 'f') ? lo - 'a' + 10 : -1;

    if (hi == -1 || lo == -1) {
        return -1;
    }

    return (hi << 4) | lo;
}


