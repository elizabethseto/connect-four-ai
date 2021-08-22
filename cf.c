/*
 * Elizabeh Seto             6/20/2021
 */
#include <stdio.h>
#include <stdlib.h>

/* Board dimenension */
#define BOARD_WIDTH   7
#define BOARD_HEIGHT  6

/*
 * Maximum entries
 * - Maximum entries in the board
 * - Used for checking game over when there's no winner
 */
#define MAX_ENTRIES (BOARD_WIDTH * BOARD_HEIGHT)

/*
 * Player type
 * - Used to identified the current player
 * - PLAYER_AI represents the computer
 * - PLAYER_HUMAN represents the human player
 */
typedef enum PlayerType {
                         PLAYER_NONE,
                         PLAYER_AI,
                         PLAYER_HUMAN,
                         PLAYER_MAX
} PlayerType;

/*
 * Grid structure
 * - Used to represents each entry on the board
 * - data can be either 'X' or 'O'
 * - up is the pointer of neigbhor directly above
 * - down is the pointer of neigbhor directly below
 * - left is the pointer of neigbhor directly left
 * - right is the pointer of neigbhor directly right
 * - upLeft is the pointer of neigbhor on upper left
 * - upRight is the pointer of neigbhor on upper right
 * - downLeft is the pointer of neigbhor on lower left
 * - downRight is the pointer of neigbhor on lower right
*/
typedef struct Grid {
    char         data;
    struct Grid *up;
    struct Grid *down;
    struct Grid *left;
    struct Grid *right;
    struct Grid *upLeft;
    struct Grid *upRight;
    struct Grid *downLeft;
    struct Grid *downRight;
} Grid;

/*
 * Move structure
 * - Used to store historical data
 * - data can be 'A' to 'G'. i.e. the column label
 * - next is one Move of the linear linked list
 */
typedef struct Move {
    char         data;
    struct Move *next;
} Move;

/*
 * Board structure
 * - Used to represent the game board
 * - width is the width of the board
 * - height is the height of the board
 * - columnHeadings is the column headings ('A' through 'G' from left to right)
 * - rowHeadings is the row headings ('6' to '1' from top to bottom)
 * - grids represents the 2-dimensional grids that made up the game board
 */
typedef struct Board {
    int  width;
    int  height;
    char *columnHeadings;
    char *rowHeadings;
    Grid **grids;
} Board;

/*
 * Game structure
 * - Used to represents the connect 4 game
 * - board is the game board
 * - history is a linear linked list of Move structure
 * - firstPlayer is type of the first player (either PLAYER_AI or PLAYER_HUMAN)
 * - numFilled is the count of number of 'X' or 'O' on the board
 * - AIDisc - 'X' if computer goes first. 'O' if computer goes second.
 * - humanDisc - 'X' if human goes first. 'O' if human goes second.
 */
typedef struct Game {
    Board      *board;
    Move       *history;
    PlayerType  firstPlayer;
    int         numFilled;
    char        AIDisc;
    char        humanDisc;
} Game;

/*
 * Get grid
 * - Used to return pointer of a grid at location (x, y)
 * - x is the column index (left to right) and y is the row index (top to bottom)
 */
Grid *getGrid(Board *board, int x, int y) {
    return ((*(board->grids + y) + x));
}

/*
 * Setup grid
 * - Used to setup a grid at location (x, y)
 * - data is initialized to '.'
 * - Pointers of all 8 neighbors of the grid are setup
 * - NULL is used for non-exists neighbors (out of bounds)
 */
void setupGrid(Board *board, int x, int y) {
    Grid *grid = NULL;
    Grid *up = NULL;
    Grid *down = NULL;
    Grid *left = NULL;
    Grid *right = NULL;
    Grid *upLeft = NULL;
    Grid *upRight = NULL;
    Grid *downLeft = NULL;
    Grid *downRight = NULL;

    if (board != NULL) {
        grid = getGrid(board, x, y);

        /* Up grid - NULL if y == 0 */
        if (y == 0) {
            up = NULL;
        }
        else {
            up = getGrid(board, x, y-1);
        }

        /* Down Grid - NULL if y == (height - 1) */
        if (y == (board->height - 1)) {
            down = NULL;
        }
        else {
            down = getGrid(board, x, y+1);
        }

        /* Left Grid - NULL if x == 0 */
        if (x == 0) {
            left = NULL;
        }
        else {
            left = getGrid(board, x-1, y);
        }

        /* Right Grid - NULL if x == (width - 1) */
        if (x == (board->width - 1)) {
            right = NULL;
        }
        else {
            right = getGrid(board, x+1, y);
        }

        /* Up Left Grid - NULL if x == 0 || y == 0 */
        if (x == 0 || y == 0) {
            upLeft = NULL;
        }
        else {
            upLeft = getGrid(board, x-1, y-1);
        }

        /* Up Right Grid - NULL if x == (width - 1) || y == 0 */
        if (x == (board->width - 1) || y == 0) {
            upRight = NULL;
        }
        else {
            upRight = getGrid(board, x+1, y-1);
        }

        /* Down Left Grid - NULL if x == 0 || y == (height - 1) */
        if (x == 0 || y == (board->height - 1)) {
            downLeft = NULL;
        }
        else {
            downLeft = getGrid(board, x-1, y+1);
        }

        /* Down Right Grid - NULL if x == (width - 1) || y == (height - 1) */
        if (x == (board->width - 1) || y == (board->height - 1)) {
            downRight = NULL;
        }
        else {
            downRight = getGrid(board, x+1, y+1);
        }

        grid->up = up;
        grid->down = down;
        grid->left = left;
        grid->right = right;
        grid->upLeft = upLeft;
        grid->upRight = upRight;
        grid->downLeft = downLeft;
        grid->downRight = downRight;
    }
}

/*
 * Create board
 * - Used to create the game board
 * - Allocate memories for the grids, column and row headings
 * - Initializes each grid by calling setupGrid()
 * - Initializes the column headings from 'A' to 'G' (left to right)
 * - Initializes the row headings from '6 to '1 (top to bottom)
 */
Board *createBoard(int width, int height) {
    Board *board = NULL;
    int i, j;
    board = (Board *) calloc(1, sizeof(Board));
    if (board != NULL) {
        board->width = width;
        board->height = height;
        board->grids = (Grid **) calloc(1, board->height * sizeof(Grid *));
        for (i=0; i < board->height; i++) {
            (*(board->grids + i)) = (Grid *) calloc(board->width, sizeof(Grid));
        }

        /* Initialize data */
        for (i=0; i < board->width; i++) {
            for (j=0; j < board->height; j++) {
                ((*(board->grids+j))+i)->data = '.';
                setupGrid(board, i, j);
            };
        }
        /* initialize column headings */
        board->columnHeadings = (char *) calloc(1, board->width);
        for (i=0; i < board->width; i++) {
            *(board->columnHeadings) = (char)(((int) 'A') + i);
        }
        /* initialize row headings */
        board->rowHeadings = (char *) calloc(1, board->height);
        for (i=0; i < board->height; i++) {
            *(board->rowHeadings+i) = (char)(((int) '6') - i);
        }
    }

    return board;
}

/*
 * Delete board
 * - Delete the game board and free all the allocated memories
 */
void deleteBoard(Board *board) {
    int i, j;
    if (board) {
        if (board->columnHeadings) {
            free(board->columnHeadings);
        }
        if (board->rowHeadings) {
            free(board->rowHeadings);
        }
        if (board->grids) {
            for (i=0; i < board->height; i++) {
                free(*(board->grids + i));
            }
            free(board->grids);
        }
        free(board);
    }
}

/*
 * Create game
 * - Create a game structure by allocating memory from the heap
 * - Call createBoard() to create a game board
 * - Setup the history, firstPlayer, numFilledd, AIDisc and humanDisc parameters
 */
Game *createGame(PlayerType first, int width, int height) {
    Game *game = NULL;
    Board *board = NULL;

    game = (Game *) calloc(1, sizeof(Game));
    if (game != NULL) {
        board = createBoard(width, height);
        if (board != NULL) {
            game->board = board;
            game->history = NULL;
            game->firstPlayer = first;
            game->numFilled = 0;
            game->AIDisc = (game->firstPlayer == PLAYER_AI) ? 'X' : 'O';
            game->humanDisc = (game->firstPlayer == PLAYER_HUMAN) ? 'X' : 'O';
        }
        else {
            free(game);
            game = NULL;
        }
    }
    return game;
}

/*
 * Delete game
 * - Delete the game structure and free all the allocated memories
 */
void deleteGame(Game *game) {
    Move *move = NULL;
    Move *next = NULL;
    if (game != NULL) {
        if (game->board) {
            deleteBoard(game->board);
        }

        if (game->history != NULL) {
            move = game->history;
            while (move != NULL) {
                next = move->next;
                free(move);
                move = next;
            }
        }
        free(game);
    }

}

/*
 * isWinHorizontal
 * - Check if the game is won along the horizontal direction
 * - board is the game board
 * - (x, y) is the location of the newly added grid
 * - data is either 'X or 'O'
 * - neighbors left and right of the current gridof the same type are counted
 * - Total count is returned as score
 * - If total counter is greater than or equal to 4, this is the winner
 */
int isWinHorizontal(Board *board, int x, int y, char data, int *score) {
    int winning = 0;
    Grid *grid = NULL;
    int total = 1;

    if (board) {
        /* Check left */
        grid = getGrid(board, x, y);
        while (grid->left != NULL && grid->left->data == data) {
            total++;
            grid = grid->left;
        }
        /* Check right */
        grid = getGrid(board, x, y);
        while (grid->right != NULL && grid->right->data == data) {
            total++;
            grid = grid->right;
        }

        if (total >= 4) {
            winning = 1;
        }
        *score = total;
    }

    return winning;
}

/*
 * isWinVertical
 * - Check if the game is won along the vertical direction
 * - board is the game board
 * - (x, y) is the location of the newly added grid
 * - data is either 'X or 'O'
 * - neighbors above and below the current gridof the same type are counted
 * - Total count is returned as score
 * - If total counter is greater than or equal to 4, this is the winner
 */
int isWinVertical(Board *board, int x, int y, char data, int *score) {
    int winning = 0;
    Grid *grid = NULL;
    int total = 1;

    if (board) {
        /* Check up */
        grid = getGrid(board, x, y);
        while (grid->up != NULL && grid->up->data == data) {
            total++;
            grid = grid->up;
        }
        /* Check down */
        grid = getGrid(board, x, y);
        while (grid->down != NULL && grid->down->data == data) {
            total++;
            grid = grid->down;
        }

        if (total >= 4) {
            winning = 1;
        }
        *score = total;
    }

    return winning;
}

/*
 * isWinLeftDiagonal
 * - Check if the game is won along the left diagonal direction
 * - board is the game board
 * - (x, y) is the location of the newly added grid
 * - data is either 'X or 'O'
 * - neighbors along upper left and lower right of the current gridof the same type are counted
 * - Total count is returned as score
* - If total counter is greater than or equal to 4, this is the winner
 */
int isWinLeftDiagonal(Board *board, int x, int y, char data, int *score) {
    int winning = 0;
    Grid *grid = NULL;
    int total = 1;

    if (board) {
        /* Check upLeft */
        grid = getGrid(board, x, y);
        while (grid->upLeft != NULL && grid->upLeft->data == data) {
            total++;
            grid = grid->upLeft;
        }
        /* Check downRight */
        grid = getGrid(board, x, y);
        while (grid->downRight != NULL && grid->downRight->data == data) {
            total++;
            grid = grid->downRight;
        }

        if (total >= 4) {
            winning = 1;
        }
        *score = total;
    }

    return winning;
}

/*
 * isWinRightDiagonal
 * - Check if the game is won along the right diagonal direction
 * - board is the game board
 * - (x, y) is the location of the newly added grid
 * - data is either 'X or 'O'
 * - neighbors along upper right and lower left of the current gridof the same type are counted
 * - Total count is returned as score
 * - If total counter is greater than or equal to 4, this is the winner
 */
int isWinRightDiagonal(Board *board, int x, int y, char data, int *score) {
    int winning = 0;
    Grid *grid = NULL;
    int total = 1;

    if (board) {
        /* Check upRight */
        grid = getGrid(board, x, y);
        while (grid->upRight != NULL && grid->upRight->data == data) {
            total++;
            grid = grid->upRight;
        }
        /* Check downLeft */
        grid = getGrid(board, x, y);
        while (grid->downLeft != NULL && grid->downLeft->data == data) {
            total++;
            grid = grid->downLeft;
        }

        if (total >= 4) {
            winning = 1;
        }
        *score = total;
   }

    return winning;
}

/*
 * isWin
 * - Check if a winner after a new data is added to location (x, y)
 * - 4 paths are checked: horizonal, vertical, left diagonal and right diagonal
 * - data is either 'X' or 'O'
 * - score is highest total count among all 4 paths
 */
int isWin(Board *board, int x, int y, char data, int *score) {
    int winning = 0;
    int horizontalScore = 0;
    int verticalScore = 0;
    int leftDiagonalScore = 0;
    int rightDiagonalScore = 0;
    int highestScore = 0;

    if (isWinHorizontal(board, x, y, data, &horizontalScore)) {
        winning = 1;
    }
    else if (isWinVertical(board, x, y, data, &verticalScore)) {
        winning = 1;
    }
    else if (isWinLeftDiagonal(board, x, y, data, &leftDiagonalScore)) {
        winning = 1;
    }
    else if (isWinRightDiagonal(board, x, y, data, &rightDiagonalScore)) {
        winning = 1;
    }

    highestScore = horizontalScore;
    if (verticalScore > highestScore) {
        highestScore = verticalScore;
    }
    if (leftDiagonalScore > highestScore) {
        highestScore = leftDiagonalScore;
    }
    if (rightDiagonalScore > highestScore) {
        highestScore = rightDiagonalScore;
    }

    *score = highestScore;
    return winning;
}

/*
 *isGameOver
 * - Used to return if game is over without a winner (tie game)
 * - Tie game is determined when all grids are filled but no coonect 4.
 */
int isGameOver(Game *game) {
    int gameOver = 0;

    if (game != NULL) {
        gameOver = (game->numFilled == MAX_ENTRIES);
    }

    return gameOver;
}

/* dropDisc */
int dropDisc(Game *game, char column, char data, char *row, int *won, int *score) {
    int success = 0;
    int columnIndex = (int) (column - 'A');
    Grid *grid = NULL;
    int i=0;
    Move *move = NULL, *tmpMove;
    Board *board = game->board;

    if (board && (columnIndex >= 0 && columnIndex < game->board->width)) {
        for (i=game->board->height - 1; i >= 0; i--) {
            grid = getGrid(board, columnIndex, i);
            if (grid->data != '.') {
                continue;
            }
            else {
                grid->data = data;
                success = 1;
                move = (Move *) calloc(1, sizeof(Move));
                move->data = column;
                if (game->history == NULL) {
                    game->history = move;
                }
                else {
                    tmpMove = game->history;
                    while (tmpMove->next != NULL) {
                        tmpMove = tmpMove->next;
                    }
                    tmpMove->next =  move;
                }
                *row = (char) ((int) '0') + (game->board->height - i);
                *won = isWin(board, columnIndex, i, data, score);
               break;
            }
        }
    }

    return success;
}

/*
 * getScore
 * - Used to return the score of a grid
 * - The grid's column index is represented by columnIndex
 * - The row index is the first '.' from bottom
 * - The score is the highest count of the dame data among all 4 paths
 */
int getScore(Game *game, int columnIndex, char data, int *score) {
    int success = 0;
    Grid *grid = NULL;
    int i=0;
    Move *move = NULL, *tmpMove;
    Board *board = game->board;
    int won = 0;

    if (board && (columnIndex >= 0 && columnIndex < game->board->width)) {
        for (i=game->board->height - 1; i >= 0; i--) {
            grid = getGrid(game->board, columnIndex, i);
            if (grid->data != '.') {
                continue;
            }
            else {
                grid->data = data;
                success = 1;
                won = isWin(board, columnIndex, i, data, score);
                /* Reset data */
                grid->data = '.';
               break;
            }
        }
    }

    return success;
}

/*
 * print board
 * - Used to print the game board
 */
void printBoard(Board *board) {
    int i, j;
    Grid *grid;
    if (board) {
        printf("  ");
        for (i=0; i < board->width; i++) {
            printf("%c ", *(board->columnHeadings + i));
        }
        printf("\n");

        for (i=0; i < board->height; i++) {
            printf("%c ", *(board->rowHeadings+i));
            for (j=0; j < board->width; j++) {
                grid = getGrid(board, j, i);
                printf("%c ", grid->data);
            }
            printf("\n");
        }
    }
}

/*
 * AI next move
 * - Find the next move (i.e. column) for the computer
 * - The algorithm is:
 *   1. Find the score of the next grid on each column if the human is taken over
 *   2. If the highest score is greater than 2, the computer will use this as next move
 *   3. The idea is to prevent the human from getting a better score in the next move
 *   4. If the human's highest score is less than or equals to 2, the algorithm will
 *      search for the highest score for the computer
 *   5. This is done to improve the winning situation of the computer
 *   6. Finally, a random number generator is used if all fail
 */
char getAINextMove(Game *game) {
    char nextMove;
    int score = 0;
    int i = 0;
    int success = 0;
    int highestScore = 0;
    int highestColumn = -1;

    for (i=0; i < game->board->width; i++) {
        success = getScore(game, i, game->humanDisc, &score);
        if (success) {
            if (score > highestScore) {
                highestScore = score;
                highestColumn = i;
            }
        }
    }

    if (highestScore > 2) {
        nextMove = (char) (highestColumn) + 'A';
    }
    else {
        for (i=0; i < game->board->width; i++) {
            success = getScore(game, i, game->AIDisc, &score);
            if (success) {
                if (score > highestScore) {
                    highestScore = score;
                    highestColumn = i;
                }
            }
        }

        if (highestColumn != -1) {
            nextMove = (char) (highestColumn) + 'A';
        }
        else {
            /* Random chosen next move */
            nextMove = (char) (rand() % 6) + 'A';
        }

    }

    return nextMove;
}

/*
 * Human next move
 * - Prompt the user for the next move ('A' to 'G')
 */
char getHumanNextMove(Game *game) {
    char nextMove = 'A';
    char carriageReturn;

    printf("Enter column ('A' - 'G'): ");
    scanf("%c", &nextMove);
    do {
        scanf("%c", &carriageReturn);
    } while (carriageReturn != '\n');

    return nextMove;
}

/*
 * Process human move
 * - Process the human's next move by
 *   1. Get the next move column
 *   2. Drop the disc to the column
 *   3. Determine if the human is the winner
*/
int processHumanMove(Game *game, PlayerType *winner)
{
    int success = 0;
    int retries = 0;
    char next = ' ';
    int won = 0;
    char row;
    int score = 0;

    while (!success && retries < 5) {
        next = getHumanNextMove(game);
        if (next >= 'A' && next <= 'G') {
            success = dropDisc(game, next, game->humanDisc, &row, &won, &score);
            if (success) {
                if (won) {
                    *winner = PLAYER_HUMAN;
                }
                printf("Human adds '%c' to [%c%c]\n", game->humanDisc, next, row);
            }
            else {
                retries++;
            }
        }
        else {
            retries++;
        }
    }

    return success;
}

/*
 * Process AI move
 * - Process the computer's next move by
 *   1. Get the next move column
 *   2. Drop the disc to the column
 *   3. Determine if the computer is the winner
 */
int processAIMove(Game *game, PlayerType *winner)
{
    int success = 0;
    int retries = 0;
    char next = ' ';
    int won = 0;
    char row;
    int score = 0;
    while (!success && retries < 5) {
        next = getAINextMove(game);
        success = dropDisc(game, next, game->AIDisc, &row, &won, &score);
        if (success) {
            if (won) {
                *winner = PLAYER_AI;
            }
            printf("Computer adds '%c' to [%c%c]\n", game->AIDisc, next, row);
        }
        else {
            retries++;
        }
    }

    return success;
}

/*
 * Print game stat
 * - Print the game stat including
 *   1. the board
 *   2. the history list
 */
void gameStat(Game *game) {
    Move *move = NULL;

    printf("Finally board is\n");
    printBoard(game->board);
    printf("\n");

    printf("Moves\n");
    move = game->history;
    printf("[");
    while (move != NULL) {
        printf("%c", move->data);
        if (move->next != NULL) {
            printf(",");
        }
        else {
            printf("]\n");
        }
        move = move->next;
    }
}

/*
 * Play game
 * - Play the game
 * - Starts with computer if it supposed to go first
 * - Loop until the same is done (tie or a winner is found)
 * - Human and computer are alternaing to play inside the loop
 */
int playGame(Game *game) {
    int success = 0;
    char nextMove = ' ';
    int done = 0;
    PlayerType winner = PLAYER_NONE;
    if (game != NULL) {
        success = 1;
        if (game->firstPlayer == PLAYER_AI) {
            success = processAIMove(game, &winner);
            if (success && winner == PLAYER_NONE) {
                game->numFilled++;
                printBoard(game->board);
            }
        }

        while (success && !done && winner == PLAYER_NONE) {
            success = processHumanMove(game, &winner);
            if (success && winner == PLAYER_NONE) {
                game->numFilled++;
                printBoard(game->board);
            }
            if (isGameOver(game)) {
                printf("Game over!\n");
                done = 1;
            }

            if (success && !done && winner == PLAYER_NONE) {
                success = processAIMove(game, &winner);
                if (success && winner == PLAYER_NONE) {
                    game->numFilled++;
                    printBoard(game->board);
                }
                if (isGameOver(game)) {
                    printf("Game over!\n");
                    done = 1;
                }
            }
        }

        if (!success) {
            printf("Error while playing game\n");
        }
        else {
            if (winner == PLAYER_AI) {
                printBoard(game->board);
                printf("Computer wins!\n");
            }
            else if (winner == PLAYER_HUMAN) {
                printBoard(game->board);
                printf("Human wins!\n");
            }
            gameStat(game);
        }
    }
    return success;
}

/*
 * main function
 * - Main function to start the connect 4 game
 * - Prompt the human if prefers to go first
 * - Allows repeated game by prompting human the current game is done.
 */
int main(int argc, char *argv[]) {
    int rc = -1;
    Game *game = 0;
    char yesOrNo = 'n';
    char carriageReturn = '\n';
    PlayerType firstPlayer = PLAYER_AI;
    int done = 0;

    printf("Welcome to connect 4 game!\n");
    while (!done) {
        firstPlayer = PLAYER_AI;
        printf("Do you want to play first (Y/y)?\n");
        scanf("%c", &yesOrNo);
        if (yesOrNo == 'y' || yesOrNo == 'Y')  {
            firstPlayer = PLAYER_HUMAN;
        }
        do {
            scanf("%c", &carriageReturn);
        } while (carriageReturn != '\n');

        if (firstPlayer == PLAYER_HUMAN) {
            printf("Human plays first using 'X' disc\n");
            printf("Computer plays using 'O' disc\n");
        }
        else {
            printf("Computer plays first using 'X' disc\n");
            printf("Human plays using 'O' disc\n");
        }
        game = createGame(firstPlayer, BOARD_WIDTH, BOARD_HEIGHT);
        if (game != NULL) {
            if (playGame(game) == 1) {
                printf("Game completed successfully\n");
            }
            else {
                printf("Game exits with error\n");
            }

            deleteGame(game);
        }
        else {
            printf("Failed to create game\n");
        }
        printf("Play again (Y/y)?\n");
        scanf("%c", &yesOrNo);
        if (yesOrNo != 'y' && yesOrNo != 'N')  {
            done = 1;
        }
        do {
            scanf("%c", &carriageReturn);
        } while (carriageReturn != '\n');
    }

    return rc;
}
