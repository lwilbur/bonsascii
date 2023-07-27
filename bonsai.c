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
    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_YELLOW);
    init_pair(2, COLOR_GREEN, COLOR_GREEN);
}


/**
 * Grow the tree, starting in the center-bottom of the stdscr.
*/
void generate_tree() {
    // Determine screen dimensions
    getmaxyx(stdscr, maxY, maxX);

    // Start tree in center bottom
    int currY = maxY - 1;
    int currX = maxX/2;
    
    fprintf(logFile, "starting tree at (y, x) = (%d, %d)\n", currY, currX);
    
    // Begin recursive, randomized growth pattern
    grow(currY, currX, 0, TRUNK);
    refresh();
}



void grow(int y, int x, int dist, enum type t) {
    msleep(25);
    refresh(); 
    
    // Draw at curr location, if not currently occupied
    int currCh = mvinch(y, x) & A_CHARTEXT;
    if (currCh == ' ')
        draw_char(y, x, t);

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

            // (100-5*dist)% chance to grow into adj squares
            fprintf(logFile, "\tvalid, growing\n");
            int percent_roll = rand() % 100;
            fprintf(logFile, "\tpercent roll = %d\n", percent_roll);
            if (percent_roll > 5*dist) {
                isGrowing = true;
                draw_char(adjY, adjX, t);
                grow(adjY, adjX, dist + 1, t);
            }
        }
    }

    fclose(logFile);

    // TODO: handle wrapping off edge of screen
    
    // If this square spawned no adjacent growths...
    if (!isGrowing) {
        // Change to leaves if currently trunk and try again
        if (t == TRUNK)
            grow(y, x, dist, LEAF);
        
        // If currently leaf, this branch of recursion ends
    }

    refresh();
}


void draw_char(int y, int x, enum type t) {
    // Determine character
    char ch;
    int colorIdx;
    if (t == TRUNK) {
        ch = TRUNK_CHAR;
        colorIdx = TRUNK_COLOR;
    }
    else if (t == LEAF) {
        ch = LEAF_CHAR;
        colorIdx = LEAF_CHAR;
    }
    else
        exit(1);
    
    attron(COLOR_PAIR(colorIdx));
    attron(A_BOLD);
    mvwaddch(stdscr, y, x, ch);
    attroff(A_BOLD);
    attroff(COLOR_PAIR(colorIdx));
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
 * Sleep for a certain number of miliseconds
*/
void msleep(int miliseconds) {
    usleep(1000 * miliseconds);
}


/**
 * Terminate stdscr window and exit program
 */
void leave(int code) {
    endwin();
    fclose(logFile);
    exit(code);
}
