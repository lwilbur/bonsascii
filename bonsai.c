#include "bonsai.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>
#include <ncurses.h>

const char TRUNK_CHAR = '#';
const char LEAF_CHAR  = '*';
const int TRUNK_COLOR = 1;
const int LEAF_COLOR  = 2;
int maxY, maxX;

FILE* logFile;

/**
 * Create window, grow tree, allow user interaction, and exit
 */
int main(int argc, char* argv[]) {
    logFile = fopen("./log.txt", "w");
    srand(time(NULL));
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(logFile, NULL, _IONBF, 0);
    ncurses_init();
    generate_tree();
    interact_loop();
    leave(0);
}


/**
 * Set traits for ncurses' stdscr
 */
void ncurses_init() {
    // Window traits
    initscr();             // setup
    cbreak();              // typed chars read one-at-atime
    noecho();              // terminal doesn't echo typing
    keypad(stdscr, TRUE);  // allow arrow keys, del, etc.

    // Colors
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
}


/**
 * Grow the tree, starting in the center-bottom of the stdscr.
*/
void generate_tree() {
    // Determine screen dimensions
    getmaxyx(stdscr, maxY, maxX);

    // Start tree in center bottom
    int currY = maxY/2;
    int currX = maxX/2;
    
    fprintf(logFile, "starting tree at (y, x) = (%d, %d)\n", currY, currX);
    
    // Begin recursive, randomized growth pattern
    grow(currY, currX, TRUNK);
    refresh();
}



void grow(int y, int x, enum type t) {
    sleep(1);
    refresh(); 

    // Determine character
    char ch;
    if (t == TRUNK)
        ch = TRUNK_CHAR;
    else if (t == LEAF)
        ch = LEAF_CHAR;
    else
        exit(1);
    
    // Draw at curr location, if not currently occupied
    int currCh = mvinch(y, x) & A_CHARTEXT;
    if (currCh == ' ')
        mvwaddch(stdscr, y, x, ch);

    // Consider adjacent squares, decide growth randomly
    bool isGrowing = false;
    for (int x_offset = -1; x_offset <= 1; x_offset++) {
        for (int y_offset = -1; y_offset <= 0; y_offset++) {
            int adjY = y + y_offset;
            int adjX = x + x_offset;
            char adjCh = mvinch(adjY, adjX) & A_CHARTEXT;

            fprintf(logFile, "considering (y, x) loc (%d, %d), which has value '%d'\n", adjY, adjX, adjCh);

            // Don't overwrite existing squares
            if (adjCh != ' ') {
                fprintf(logFile, "\toccupied -- continuing\n");
                continue;
            }

            // 25% chance to grow into adj squares
            fprintf(logFile, "\tvalid, growing\n");
            int percent_roll = rand() % 100;
            fprintf(logFile, "\tpercent roll = %d\n", percent_roll);
            if (percent_roll > 75) {
                isGrowing = true;
                mvwaddch(stdscr, adjY, adjX, ch); // TODO: figure out why this doesn't work, but wrong order (x, y) does
                grow(adjY, adjX, t);
            }
        }
    }

    fclose(logFile);

    // TODO: handle wrapping off edge of screen
    
    // If this square spawned no adjacent growths...
    if (!isGrowing) {
        // Change to leaves if currently trunk and try again
        if (t == TRUNK)
            grow(y, x, LEAF);
        
        // If currently leaf, this branch of recursion ends
    }

    refresh();
}


/**
 * 
 */
void interact_loop() {
    move(maxX/2, maxY/2);
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

            // Quit on 'q'
            case 'q': {
                go = false;
                break;
            }
        }

        // Update screen
        refresh();
    }
}


/**
 * Terminate stdscr window and exit program
 */
void leave(int code) {
    endwin();
    fclose(logFile);
    exit(code);
}
