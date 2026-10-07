#ifndef ARRAY_H
#define ARRAY_H

// made for strings only

#define BLOCK_SIZE 32

#define APPEND_SUCCESS 0
#define APPEND_ERROR 1

#define BLOCK_ADD_SUCCESS 0
#define BLOCK_ADD_ERROR 1

#define BLOCK_SET_SUCCESS 0
#define BLOCK_SET_ERROR 1

#define SET_SUCCESS 0
#define SET_ERROR 1

typedef struct {
    size_t size;
    size_t amount_of_blocks;
    size_t max_size;
    char **array;
} array_t;

array_t* create_array();

int char_append(array_t* array, char data);

int set_char(array_t* array, char c, size_t index);
int set_block(array_t* array, char *new_block, size_t block_index);

int add_new_block(array_t* array);

char getc_by_pos(array_t* array, size_t position);
char* getstr_by_pos(array_t* array, size_t position);
char* get_block(array_t* array, size_t block);

void arrfree(array_t* array);



#endif // ARRAY_H
