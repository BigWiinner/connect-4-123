Using the provided code that can be downloaded from this github add a new class that inherits from game.cpp in the same way TicTacToe.cpp does and implement a working version of the game Connect 4. The game should be added as a fourth choice from the menu so the choices are Tic Tac Toe, Checkers, Othello and now Connect 4.

The game must be playable by both by 2 people and vs. an AI. Your implementation must check for the winning condition, and display that on the right hand side the same way the current setup does for tic-tac-toe. The stopGame() method is expected to clean up and delete any memory allocated.

Look at the new Grid.cpp class and see how it is used in Checkers.cpp for a good understanding about laying out a useable grid.

For extra credit, when a game piece is added to the board, make it animate into place instead of just appearing in it's final spot.

Graphic files are provided for the game pieces called yellow.png and red.png.

For the submission, create a new github based on the above code-base and provide a link to that along with a complete readme.md on how your implementation works.

## Implementation ##
Connect 4 has three gameplay options: Human vs. Human, AI vs. Human where the AI plays first as the yellow pieces, and Human vs. AI where AI plays second as the red pieces.
Select one of the options to play. Click on an empty space in a column to place a piece in that column.
Implementation uses negamax with alpha-beta pruning to figure out what moves to make in order for the AI to win. If the negamax doesn't return a certain path to victory, the bot will
  instead prioritize placing pieces in the center columns first before branching out to the outer spots on the board.
In order to check for winner, the program uses bit shifting to check for vertical, horizontal, diagonal \, and diagonal / wins.
