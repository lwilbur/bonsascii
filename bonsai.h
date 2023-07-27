#ifndef BONSAI_H
#define BONSAI_H

#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

enum type {TRUNK, LEAF};

int main(int argc, char* argv[]);
void ncurses_init();
void generate_tree();
void grow(int x, int y, int dist, enum type t);
void interact_loop();
void msleep(int miliseconds);
void draw_char(int y, int x, enum type t);
void leave(int code);

#endif 