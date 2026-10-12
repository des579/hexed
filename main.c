#include <stdio.h>
#include <ncurses.h>
#include <ctype.h>

#include "array.h"

#define ROWS_AMOUNT 21

#define HEXWIN_BYTES_IN_ROW 8
#define HEXWIN_COLS_AMOUNT (HEXWIN_BYTES_IN_ROW * 2 + HEXWIN_BYTES_IN_ROW - 1)

#define BYTE_HEX(b, lp, bp)                                   \
    do {                                                      \
        uint8_t byte_hex_v_ = (uint8_t) (b);                  \
        *(bp) = "0123456789ABCDEF"[byte_hex_v_ >> 4];         \
        *(lp) = "0123456789ABCDEF"[byte_hex_v_ & 0x0F];       \
    } while (0)

typedef struct {
    int x;
    int y;
} position_t;

static void draw_hexwin(WINDOW* hexwin, array_t* hex_array, uint32_t line) {
    // draw byte then space
    uint32_t byte_index = 0;
    for (int row = 0; row < ROWS_AMOUNT; row++) {
//         int count = 0;
        for (int col = 0; col < HEXWIN_BYTES_IN_ROW; col++) {
            uint8_t byte = 0;
            int res = array_getc(hex_array, byte_index, &byte);
            if (res == ARRAY_ACTION_IERROR)
                byte = 0;


            // i pick a hex and print it!
            char l, b;
            BYTE_HEX(byte, &l, &b);
            wprintw(hexwin, "%c%c", l, b);
            // except the last byte
            if (col != HEXWIN_BYTES_IN_ROW - 1)
                waddch(hexwin, ' ');

            byte_index++;
        }
    }
    wrefresh(hexwin);
}

static int is_arrow(int ch) {
    switch (ch) {
        case KEY_UP:
        case KEY_DOWN:
        case KEY_LEFT:
        case KEY_RIGHT:
            return 1;
    }

    return 0;
}

static position_t align_position(position_t pos, int maxx, int maxy, uint32_t *offset) {
    if (pos.x >= 0 && pos.x < maxx && pos.y >= 0 && pos.y < maxy) {
        *offset = 0;
        return pos;
    }

    int wraps = pos.x / maxx;
    int rem = pos.x % maxx;

    if (rem < 0) {
        rem += maxx;
        wraps--;
    }

    pos.x = rem;
    pos.y += wraps;

    if (pos.y < 0) {
        *offset = pos.y;
        pos.y = 0;
    } else if (pos.y >= maxy) {
        *offset = pos.y - maxy + 1;
        pos.y = maxy - 1;
    }

    return pos;
}

static position_t get_arrow_key_offset(int ch) {
    position_t offset = { .x = 0, .y = 0 };

    switch (ch) {
        case KEY_RIGHT:
            offset.x = 1;
            break;
        case KEY_LEFT:
            offset.x = -1;
            break;
        case KEY_UP:
            offset.y = -1;
            break;
        case KEY_DOWN:
            offset.y = 1;
            break;
    }

    return offset;
}

static int hex_value(int ch) {
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
    if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
    return -1;
}

static void _main() {
    array_t* hex_array = array_create();

    // curses initialization
    initscr();
    keypad(stdscr, TRUE);
    noecho();
    cbreak();
    
    WINDOW* hexwin = newwin(ROWS_AMOUNT, HEXWIN_COLS_AMOUNT, 0, 0);
    WINDOW* debug = newwin(ROWS_AMOUNT, 40, 0, 30);
    WINDOW* input = newwin(1, 1, 1000, 1000);
    wclear(debug);
    wclear(hexwin);

    // draw every window first
    draw_hexwin(hexwin, hex_array, 0);

    // cursor position on hexwin
    // byte part of the byte that cursor position shows
    // shows the byte and its part (least of most significant)
    position_t cursor_position = {0, 0};
    uint8_t byte_part = 0; // 0 means least
                           // 1 means most

    // offset from the beginning of the file to top column of screen (hexwin)
    uint32_t column_offset = 0;

    int ch;
    while ((ch = wgetch(input)) != 'q') {
        wclear(debug);
        // if ch is hexadecimal, save it to array
        uint32_t index = (column_offset + cursor_position.y) * ARRAY_BLOCK_SIZE + cursor_position.x;
        if (isxdigit(ch)) {
            // TODO: error handling
            int ae_res = array_expand(hex_array, index);
            wprintw(debug, "array_expand result: %d\n", ae_res);
            
            // after array_expand there should be enough space to get the next char
            uint8_t curr_ch;
            array_getc(hex_array, index, &curr_ch);

            uint8_t new_ch;
            uint8_t hex_val = hex_value(ch);
            if (byte_part == 0) { // least significant
                new_ch = (curr_ch & 0xF0) | (hex_val);
            } else { // most significant
                new_ch = (curr_ch & 0x0F) | (hex_val << 4);
            }

            wprintw(debug, "setting new_ch: %d at index: %d\n", new_ch, index);
            int r = array_setc(hex_array, index, new_ch);
            wprintw(debug, "setc res: %d\n", r);

            byte_part++;
            if (byte_part > 1) {
                cursor_position.x++;
                byte_part = 0;
            }
        } 
        // if ch is arrow keys, move it move it
        else if (is_arrow(ch)) {
            position_t offset = get_arrow_key_offset(ch);
            byte_part += offset.x;
            if (byte_part > 1) {
                cursor_position.x++;
                byte_part = 0;
            }
            cursor_position.y += offset.y;
        }

        // clear windows before doing something
        wclear(hexwin);

        // align position
        cursor_position = align_position(cursor_position, 
                HEXWIN_BYTES_IN_ROW, ROWS_AMOUNT, &column_offset);

        // draw any changes
        draw_hexwin(hexwin, hex_array, column_offset);

        // debug window
        wprintw(debug, "cursor_position: %d %d\n", cursor_position.x, cursor_position.y);
        wprintw(debug, "column_offset: %d\n", column_offset);
        wrefresh(debug);
    }

    // update the cursor position x by 1
    // if arrow keys, update the position respectively
    //
    // draw all the chages

    // free everything
    endwin();
    delwin(hexwin);
    array_free(hex_array);
}

int main() {

    _main();

    return 0;
}



