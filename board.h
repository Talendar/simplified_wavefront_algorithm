#ifndef BOARD_H
    #define BOARD_H
    #include <stdbool.h>
    typedef struct Board Board;

    Board* create_board(int w, int h);
    void free_board(Board **board);
    void update_board(Board *board);
    bool next2enemy(Board *board);
    void print_board(Board *board);
#endif