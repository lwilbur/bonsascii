#include "bonsai.h"
#include <stdlib.h>
#include <stdio.h>
#include <ncurses.h>
#include <stdbool.h>


int main(int argc, char* argv[]) {
    printf("LKSJDF:LSKDJF\n");

    ncurses_init();

    int h, w;
    getmaxyx(stdscr, h, w);
    move(h/2, w/2);
    
    main_loop();
    endwin();
    leave(0);
}


void ncurses_init() {
    initscr();  // setup
    cbreak();   // typed chars read one-at-atime
    noecho();   // terminal doesn't echo typing
    keypad(stdscr, TRUE);  // allow arrow keys, del, etc.

    return;
}


void leave(int code) {
    endwin();
    exit(code);
}


void main_loop() {
    bool go = true;
    while(go) {
        // Determine cursor position
        int y, x;
        getyx(stdscr, y, x);

        // Move cursor by user choice
        int pressed = getch();
        switch (pressed) {
            case KEY_UP: {
                y -= 1;
                move(y, x);
                break;
            }

            case KEY_DOWN: {
                y += 1;
                move(y, x);
                break;
            }

            case KEY_LEFT: {
                x -= 1;
                move(y, x);
                break;
            }

            case KEY_RIGHT: {
                x += 1;
                move(y, x);
                break;
            }

            case 'q': {
                go = false;
                break;
            }
        }

        // Update screen
        refresh();
    }
}


void grow(int x, int y, enum type t) {
    
}
