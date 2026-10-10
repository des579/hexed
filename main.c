#include <stdio.h>

#include <stdlib.h>
#include <curses.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "array.h"
// #include "file.h"
#include "defines.h"

// global macro (used for every window)
#define ROW_AMOUNT 21

// hexadecimal window
#define HEXBYTESWIN_HEX_IN_ROW 8
#define HEXBYTESWIN_ROW_SIZE (HEXBYTESWIN_HEX_IN_ROW * 2 + HEXBYTESWIN_HEX_IN_ROW -1)

// window representing current amount of bytes (the left window)
// will use uint64_t to represent the size
#define SIZEWIN_ROW_SIZE (8*2) // 8*2 hex amount of uint64_t

// vertical line
#define VERTICALWIN_ROW_SIZE 1 // just the character '|'

// offsets for each window
#define SIZEWIN_OFFSET 0
#define VERTICALWIN_OFFSET_LEFT (SIZEWIN_ROW_SIZE + SIZEWIN_OFFSET + 1)
#define HEXBYTESWIN_OFFSET (VERTICALWIN_OFFSET_LEFT + VERTICALWIN_ROW_SIZE + 1)
#define VERTICALWIN_OFFSET_RIGHT (HEXBYTESWIN_OFFSET + HEXBYTESWIN_ROW_SIZE + 1)



typedef struct {
    int x;
    int y;
} position_t;

position_t align_position(position_t pos, int maxx, int maxy);
position_t translate_to_hexwin(position_t pos);

int map_arrow_key_binds(int ch, position_t* offset);
size_t position_to_size(position_t position, float x_unit, float y_unit);

void update_array(array_t* array, FILE* fptr, size_t file_position);
void char_to_hex(int c, char hex[2]);
void size_t_to_hex4(size_t value, char output[9]);


int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: 'hexed <filename>/<options>'\nSee 'hexed --help' for help\n");
        return 0;
    }

    
    char* filename = argv[1];
    FILE* fptr = fopen(filename, "r+b");
    if (!fptr)
        fptr = fopen(filename, "w+b");


    // array to save changes
    array_t* hex_data = array_create();

    // curses initialization shit
    initscr();
    keypad(stdscr, TRUE);
    noecho();
    cbreak();

    // windows
    WINDOW *hexwin = newwin(HEXBYTESWIN_ROW_SIZE, ROW_AMOUNT, 0, HEXBYTESWIN_OFFSET);
    WINDOW *vertwin_left = newwin(VERTICALWIN_ROW_SIZE, ROW_AMOUNT, 0, VERTICALWIN_OFFSET_LEFT);
    WINDOW *vertwin_right = newwin(VERTICALWIN_ROW_SIZE, ROW_AMOUNT, 0, VERTICALWIN_OFFSET_RIGHT);
    WINDOW *sizewin = newwin(SIZEWIN_ROW_SIZE, ROW_AMOUNT, 0, SIZEWIN_OFFSET);


    // the screen cursor position and global cursor position
    position_t cursor_position = {0, 0};
    position_t global_cursor_position = {0, 0};
    // array buffer and file position
    size_t file_position = 0;

    int ch = 0;
    do {
        position_t new_position_offset; // used for map_arrow_key_binds

        // if hexadecimal, put the byte there
        // translate position_t to index on array, update the array
        // if arrow, change position depending on what arrow is it

        // hexadecimal
        if (isxdigit((unsigned char) ch)) {
            if (isalpha(ch))
                ch = toupper(ch);
            // setting the char to array
            size_t index = position_to_size(cursor_position, 0.5, 1);
            array_setc(hex_data, ch, index);

            cursor_position.x++;
        }

        // arrows
        else if (map_arrow_key_binds(ch, &new_position_offset) == ISUCCESS) {
            // change the cursor_position
            cursor_position.x += new_position_offset.x;
            cursor_position.y += new_position_offset.y;
        }
        
        // aligning the position
        
        // translate to global


//         wmove_cursor(hexwin, cursor_position);
    } while ((ch = getch()) != 'q');


    delwin(hexwin);
    delwin(vertwin_right);
    delwin(vertwin_left);
    delwin(sizewin);

    endwin();
    clear();
//     save_array(hex_data, fptr);
    array_free(hex_data);
    if (fptr)
        fclose(fptr);

    return 0;
}


position_t translate_to_hexwin(position_t pos) {
    position_t new;
    new.x = pos.x + pos.x / 2;
    new.y = pos.y;
    return new;
}

position_t align_position(position_t pos, int maxx, int maxy) {
    uint32_t x_m = pos.x / maxx;
    uint32_t y_m = pos.y - maxy;
    if (y_m < 0) y_m = 0;
    position_t offset = {pos.x - maxx * x_m, y_m};
    return offset;
}

size_t position_to_size(position_t position, float x_unit, float y_unit) {
    return (size_t) position.x * x_unit + (size_t) position.y * y_unit;
}


int map_arrow_key_binds(int ch, position_t* offset) {
    int retval = ISUCCESS;
    switch (ch) {
        case KEY_UP:
            offset->y = -1;
            break;
        case KEY_DOWN:
            offset->y = 1;
            break;
        case KEY_LEFT:
            offset->x = -1;
            break;
        case KEY_RIGHT:
            offset->y = 1;
            break;
        default:
            retval = IERROR;
            break;
    }

    return retval;
}



// void print_file_data(WINDOW* window, size_t position, FILE* fprt) {
//     ;
// }
// 
// void wprint_file(WINDOW* window, array_t* hex_data, size_t position) {
//     // the way to print
//     //
//     // hex hex hex
//     // hex hex hex
// 
//     wmove(window, 0, 0);
//     size_t block = position / (BLOCK_SIZE/2);
// 
//     for (size_t i = 0; i < HEX_ROWS; i++) {
//         char* row_data = get_block(hex_data, block + i);
//         for (size_t j = 0; j < DATA_PER_ROW; j++) {
//             char a[3];
//             a[0] = row_data[j*2];
//             a[1] = row_data[j*2+1];
//             a[2] = 0;
//             wprintw(window, "%s ", a);
//         }
//         free(row_data);
//     }
// 
//     refresh();
//     wrefresh(window);
// }


void char_to_hex(int c, char hex[2]) {
    static const char digits[] = "0123456789ABCDEF";
    hex[0] = digits[(c >> 4) & 0xF];
    hex[1] = digits[c & 0xF];
}

void size_t_to_hex4(size_t value, char output[9]) {
    uint32_t truncated = (uint32_t)(value & 0xFFFFFFFF);
    sprintf(output, "%08X", truncated);
}




