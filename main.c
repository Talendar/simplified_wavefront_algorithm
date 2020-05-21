/**
 * Simple program that implements the wavefront algorithm for graphs in the context of a simple "catch game".
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "board.h"
#include <unistd.h>


#define USE_SCREEN_CLEANER 1      //if 1, the program will use a command to clean the terminal when needed; if 0, line breaks are used instead.
#define WAITING_TIME 700          //time, in ms, for the program to wait after each update to the board


/* 
 * Clean the terminal's window.
 */
void clean_screen() {
    if(USE_SCREEN_CLEANER)
        system("@cls||clear");
    else
        printf("\n\n\n");
}


int main(void) 
{
    srand(time(NULL));  //set the seed used for the pseudo-random number generator
    printf("Enter the width and the height of the board:\n");
    int w, h;  scanf(" %d %d", &w, &h);

    Board *board = create_board(w, h);
    clean_screen();
    print_board(board);  printf("\n\n");

    while(!next2enemy(board)) {
        usleep(WAITING_TIME * 1000);
        clean_screen();
        update_board(board);
        print_board(board);  printf("\n\n");
    }

    free_board(&board);
    return 0;
}