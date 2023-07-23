#ifndef BONSAI_H
#define BONSAI_H

#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

enum type {trunk, leaf};

int main(int argc, char* argv[]);
void ncurses_init();
void main_loop();
void leave(int code);
void grow(int x, int y, enum type t);

#endif 