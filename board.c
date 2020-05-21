#include "board.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


static const char BOARD_DEFAULT_VALUE = '.',
                  PLAYER_CHAR = 'C',
                  ENEMY_CHAR = '&';  


/**
 * General structure of the game board. It's modeled after an undirected graph. Each position of the board represents a node of the graph. Each node is connected to the nodes represented by the board positions adjacent to its own position.
 */
struct Board  {
    int w, h;                   //width and height of the board
    int player_i, player_j;     //player's position on the board
    int enemy_i, enemy_j;       //enemy's position on the board
    char **matrix;              //char matrix of dimension w x h representing the board
};


/**
 * Creates a char matrix with the given dimensions and default value.
 * 
 * @param w width of the matrix.
 * @param h height of the matrix.
 * @return a pointer to the newly created matrix or NULL if the required memory couldn't be allocated.
 */
static char** create_matrix(int w, int h, char default_value) {
    char** m = malloc(sizeof(char*) * h);
    if(m != NULL) {
        int i;
        for(i = 0; i < h; i++) {
            m[i] = malloc(sizeof(char) * w);
            if(m[i] == NULL)
                break;
        }

        if(i != h) {                        //free the already allocated memory in case the allocation process doesn't work
            for(i; i >= 0; i--)
                free(m[i]);
            free(m);
            m = NULL;
        }
        else {                              //set the board positions to a default value
            for(i = 0; i < h; i++) {
                for(int j = 0; j < w; j++) 
                    m[i][j] = default_value;
            }
        }
    }

    return m;
}


/**
 * Creates a new board.
 * 
 * @param w width of the board.
 * @param h height of the board.
 * @return a pointer to the newly created board or NULL if the required memory couldn't be allocated.
 */
Board* create_board(int w, int h) {
    Board *board = malloc(sizeof(Board));
    if(board != NULL) {
        board->matrix = create_matrix(w, h, BOARD_DEFAULT_VALUE);
        if(board->matrix == NULL) {
            free(board);
            board = NULL;
        }
        else {
            board->w = w;
            board->h = h;
            board->player_i = board->player_j = 0;
            board->enemy_i = w - 1;
            board->enemy_j = h - 1;

            board->matrix[0][0] = PLAYER_CHAR;
            board->matrix[h - 1][w - 1] = ENEMY_CHAR;
        }
    }

    return board;
}


/**
 * Frees the memory allocated during the board's creation.
 * 
 * @param board a pointer to the pointer pointing to the board. The last pointer will be set to NULL at the end of the call.
 * @return
 */
void free_board(Board **board) {
    Board *b = *board;
    for(int i = 0; i < b->h; i++)
            free(b->matrix[i]);
    free(b->matrix);

    free(b);
    *board = NULL;
}


/**
 * Executes the wavefront algorithm from the enemy's position and updates the given int matrix with the info obtained.
 * 
 * @param board a pointer to the board.
 * @param pos the int matrix.
 * @return 
 */
static void wavefront(Board *board, int pos[board->w][board->h]) {
    for(int i = 0; i < board->h; i++) {
        for(int j = 0; j < board->w; j++) 
            pos[i][j] = abs(board->enemy_i - i) + abs(board->enemy_j - j);
    }

    /* debug 
    for(int i = 0; i < board->h; i++) {
        for(int j = 0; j < board->w; j++) {
            printf(" %d ", pos[i][j]);
        }
        printf("\n");
    }
    printf("\n");*/
}


/**
 * Changes the player's position based on the results of the wavefront algorithm.
 * 
 * @param board a pointer to the board.
 * @param pos an int matrix with the results obtained by the wavefront algorithm.
 * @return -
 */
static void move_player(Board *board, int pos[board->w][board->h]) {
    int curr_i = board->player_i, curr_j = board->player_j;
    int next_i, next_j, weight = -1;

    for(int k = -1; k <= 1; k += 2) {
        int to_j = curr_j + k;
        if(to_j >= 0 && to_j < board->h && (weight == -1 || pos[curr_i][to_j] < weight)) {
            next_i = curr_i; 
            next_j = to_j;
            weight = pos[curr_i][to_j];
        }

        int to_i = curr_i + k;
        if(to_i >= 0 && to_i < board->h && (weight == -1 || pos[to_i][curr_j] < weight)) {
            next_i = to_i; 
            next_j = curr_j;
            weight = pos[to_i][curr_j];
        }
    }

    board->matrix[next_i][next_j] = PLAYER_CHAR;
    board->matrix[curr_i][curr_j] = BOARD_DEFAULT_VALUE;
    board->player_i = next_i;
    board->player_j = next_j;
}


/**
 * Changes the enemy's position to a random adjacent slot.
 * 
 * @param board a pointer to the board.
 * @return -
 */
static void move_enemy(Board *board) {
    static int last_mov = 2; //keeping track of the enemy's last movement in order to avoid walking forward and backward in consecutive moves
    do {
        int i = board->enemy_i, j = board->enemy_j, p;
        do {
            p = rand() % 4;

            //north
            if(p == 0 && last_mov != 1 && (i - 1) >= 0) { i -= 1; break; }
            //south
            if(p == 1 && last_mov != 0 && (i + 1) < board->h) { i += 1; break; }
            //west
            if(p == 2 && last_mov != 3 && (j - 1) >= 0) { j -= 1; break; }
            //east
            if(last_mov != 2 && (j + 1) < board->w) { j += 1; break; }
        } while(true);
        last_mov = p;

        board->matrix[i][j] = ENEMY_CHAR;
        board->matrix[board->enemy_i][board->enemy_j] = BOARD_DEFAULT_VALUE;
        board->enemy_i = i;
        board->enemy_j = j;
    } while(next2enemy(board));
}


/**
 * Updates the board by moving the player (closer to the enemy) and the enemy (random movement).
 * 
 * @param board a pointer to the board.
 * @return -
 */
void update_board(Board *board) {
    int pos[board->h][board->w];  
    wavefront(board, pos);
    move_enemy(board);
    move_player(board, pos);
}


/**
 * Check whether the player and the enemy are in adjacent nodes.
 * 
 * @param board a pointer to the board.
 * @return true if the player and the enemy are in adjacent nodes or false otherwise.
 */
bool next2enemy(Board *board) {
    int dist = abs(board->player_i - board->enemy_i) + abs(board->player_j - board->enemy_j);
    return dist <= 1;
}


/**
 * Prints the board to the stdout.
 * 
 * @param board a pointer to the board.
 * @return -
 */
void print_board(Board *board) {
    for(int i = 0; i < board->h; i++) {
        for(int j = 0; j < board->w; j++) 
            printf(" %c ", board->matrix[i][j]);
        printf("\n");
    }
}