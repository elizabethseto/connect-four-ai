The main data structures included are: Game, Board and Grid. The Game structure owns a Board and a Board owns a 2-dimensional array.
The board contains a 2-dimensional grids where an array of Grid pointers of BOARD_HEIGHT size is allocated.
Then, memory of an array of Grid structure of BOARD_WIDTH size is allocated for each of the pointer. The alogrithm to find the next move of the AI starts with
calculating the highest score along all the columns if the human is taking the grid. The AI will take the highest score grid if this grid results in a score
higher than 2 if the human is taking it. This first step is attempt to block the human from improving. If the human's highest score is less than or equals to 2,
the search for highest score for the AI is conducted. The AI will then take over the highest score grid in order to improve the AI's situation.
