#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "array.h"
#include "file.h"

#define ADDRESSES_COLUMNS 8
#define DATA_PER_ROW 16

#define HEX_COLUMNS (DATA_PER_ROW*3) // 2 for HEX, 1 for ' '
#define HEX_ROWS 21

#define WINDOW_COLUMNS (ADDRESSES_COLUMNS + 3 + HEX_COLUMNS + 1 + DATA_PER_ROW)
#define WINDOW_ROWS 21

#define FIRST_LINE_OFFSET 9 
#define SECOND_LINE_OFFSET (WINDOW_COLUMNS - 1 - DATA_PER_ROW)
#define HEX_WINDOW_OFFSET 11

#define HEX_WINDOW_BOTTOM_RIGHT (DATA_PER_ROW * 2 * HEX_ROWS)



void update_array(array_t* array, FILE* fptr, size_t file_position);

void char_to_hex(int c, char hex[2]);
void size_t_to_hex4(size_t value, char output[9]);

void wprint_line(WINDOW* window, char* data);
void wprint_adresses(WINDOW* window, size_t position);
void wprint_file(WINDOW* window, array_t* hex_data, size_t position);
void wprint_separate_line(WINDOW* window);

void update_cursor_position(int* cursor_position);

void wmove_cursor(WINDOW* window, int cursor_position);

size_t get_file_position(int cursor_position, size_t current_file_position);

int map_buttons(int ch, array_t* array, int cursor_position);
void map_arrow_key_binds(int ch, int* cursor_position);




int main(int argc, char** argv) {
    if (argc != 2) {
        printf("PLS CONSIDER USING AS ARG FILENAME!!\nNO OTHER ARGS\n");
        return 0;
    }

    char* filename = argv[1];
    FILE* fptr = fopen(filename, "r+b");
    if (!fptr)
        fptr = fopen(filename, "w+b");

    int cursor_position = 0;
    size_t file_position = 0;

    array_t* hex_data = create_array();


    initscr();
    keypad(stdscr, TRUE);
    noecho();
    cbreak();

    WINDOW *hex_window = newwin(HEX_ROWS, HEX_COLUMNS, 0, HEX_WINDOW_OFFSET);
    WINDOW *terminal_window = newwin(WINDOW_ROWS, WINDOW_COLUMNS, 0, 0); 

    int ch = 0;
    do {
        int global_cursor_position = 2 * file_position + cursor_position;
        if (map_buttons(ch, hex_data, global_cursor_position) == 0) {
            cursor_position++;
            add_new_change(global_cursor_position, ch);
        }

        map_arrow_key_binds(ch, &cursor_position);
        file_position = get_file_position(cursor_position, file_position);

        wmove(terminal_window, 0, 0);
        wprint_adresses(terminal_window, file_position);
        wmove(terminal_window, 0, FIRST_LINE_OFFSET);
        wprint_separate_line(terminal_window);
        wmove(terminal_window, 0, SECOND_LINE_OFFSET);
        wprint_separate_line(terminal_window);

        update_array(hex_data, fptr, file_position);
        wprint_file(hex_window, hex_data, file_position);

        update_cursor_position(&cursor_position);

        wmove_cursor(hex_window, cursor_position);
    } while ((ch = getch()) != 'q');


    delwin(terminal_window);
    delwin(hex_window);
    endwin();

    clear();

    save_array(hex_data, fptr);

    free_changes();
    arrfree(hex_data);
    if (fptr)
        fclose(fptr);

    return 0;
}

int map_buttons(int ch, array_t* array, int cursor_position) {
    if (ch == '0') {
        // handle 0
    } else if (ch == '1') {
        // handle 1
    } else if (ch == '2') {
        // handle 2
    } else if (ch == '3') {
        // handle 3
    } else if (ch == '4') {
        // handle 4
    } else if (ch == '5') {
        // handle 5
    } else if (ch == '6') {
        // handle 6
    } else if (ch == '7') {
        // handle 7
    } else if (ch == '8') {
        // handle 8
    } else if (ch == '9') {
        // handle 9
    } else if (ch == 'a' || ch == 'A') {
        // handle A/a
    } else if (ch == 'b' || ch == 'B') {
        // handle B/b
    } else if (ch == 'c' || ch == 'C') {
        // handle C/c
    } else if (ch == 'd' || ch == 'D') {
        // handle D/d
    } else if (ch == 'e' || ch == 'E') {
        // handle E/e
    } else if (ch == 'f' || ch == 'F') {
        // handle F/f
    } else {
        return 1;
    }

    char c = (char) ch;
    if (isalpha(c)) c = toupper(c);

    if (set_char(array, c, cursor_position) != SET_SUCCESS)
        return 1;

    refresh();

    return 0;
}

void wmove_cursor(WINDOW* window, int cursor_position) {
    int y = cursor_position / (DATA_PER_ROW*2);
    int x = cursor_position % (DATA_PER_ROW*2);
    int amount_of_spaces = x / 2;
    wmove(window, y, x + amount_of_spaces);
    refresh();
    wrefresh(window);
}

void update_cursor_position(int* cursor_position) {
    if (*cursor_position < 0)
        *cursor_position += DATA_PER_ROW*2;
    else if (*cursor_position >= HEX_WINDOW_BOTTOM_RIGHT)
        *cursor_position -= DATA_PER_ROW*2;
}

size_t get_file_position(int cursor_position, size_t current_file_position) {
    size_t file_position = current_file_position;

    if (cursor_position < 0 && file_position >= DATA_PER_ROW)
        file_position -= DATA_PER_ROW;
    else if (cursor_position >= HEX_WINDOW_BOTTOM_RIGHT)
        file_position += DATA_PER_ROW;

    return file_position;
}

void update_array(array_t* array, FILE* fptr, size_t file_position) {
    size_t block_index = file_position / (BLOCK_SIZE / 2); //e.g. file_position/16
    size_t start_file_position = file_position - file_position % (BLOCK_SIZE / 2);

    while (block_index + HEX_ROWS >= array->amount_of_blocks) {
        if (add_new_block(array) != BLOCK_ADD_SUCCESS) {
            fprintf(stderr, "create new block error\n");
            exit(1);
        }
    }

    fseek(fptr, start_file_position, SEEK_SET);

    char hex[33];
    hex[32] = '\0';
    for (int i = 0; i < HEX_ROWS; i++) {
        if (array->array[block_index + i][0] != 0)
            continue;
        for (int j = 0; j < DATA_PER_ROW; j++) {
            int c = fgetc(fptr);
            if (c == EOF) c = 0;
            char tmp[2];
            char_to_hex(c, tmp);
            strncpy(&hex[j*2], tmp, 2);
        }
        set_block(array, hex, block_index + i);
    }
}

void map_arrow_key_binds(int ch, int* cursor_position) {
    switch (ch) {
        case KEY_UP:
            *cursor_position -= DATA_PER_ROW*2;
            break;
        case KEY_DOWN:
            *cursor_position += DATA_PER_ROW*2;
            break;
        case KEY_LEFT:
            *cursor_position -= 1;
            break;
        case KEY_RIGHT:
            *cursor_position += 1;
            break;
    }
}

void wprint_adresses(WINDOW* window, size_t position) {
    for (int i = 0; i < WINDOW_ROWS; i++) {
        char hex_line[9];
        size_t_to_hex4(position + i * DATA_PER_ROW, hex_line);
        wprint_line(window, hex_line);
    }
    refresh();
    wrefresh(window);
}

void wprint_separate_line(WINDOW* window) {
    for (int i = 0; i < WINDOW_ROWS; i++) {
        wprint_line(window, "|");
    }
    refresh();
    wrefresh(window);
}

void print_file_data(WINDOW* window, size_t position, FILE* fprt) {
    ;
}

void wprint_file(WINDOW* window, array_t* hex_data, size_t position) {
    // the way to print
    //
    // hex hex hex
    // hex hex hex

    wmove(window, 0, 0);
    size_t block = position / (BLOCK_SIZE/2);

    for (size_t i = 0; i < HEX_ROWS; i++) {
        char* row_data = get_block(hex_data, block + i);
        for (size_t j = 0; j < DATA_PER_ROW; j++) {
            char a[3];
            a[0] = row_data[j*2];
            a[1] = row_data[j*2+1];
            a[2] = 0;
            wprintw(window, "%s ", a);
        }
        free(row_data);
    }

    refresh();
    wrefresh(window);
}

void wprint_line(WINDOW* window, char* data) {
    int current_y;
    int current_x;
    getyx(window, current_y, current_x);
    wprintw(window, "%s", data);
    wmove(window, current_y+1, current_x);
}

void char_to_hex(int c, char hex[2]) {
    static const char digits[] = "0123456789ABCDEF";
    hex[0] = digits[(c >> 4) & 0xF];
    hex[1] = digits[c & 0xF];
}

void size_t_to_hex4(size_t value, char output[9]) {
    uint32_t truncated = (uint32_t)(value & 0xFFFFFFFF);
    sprintf(output, "%08X", truncated);
}




