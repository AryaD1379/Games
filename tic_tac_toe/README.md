# Tic-Tac-Toe Game

This program implements Tic-Tac-Toe game in C++ with a graphical user interface created using the ImGui library.

## Implementation Details

- The game logic is within the `tic_tac_toe` class.
- Players are represented by symbols: 'X' for the human and 'O' for the AI.
- The negamax algorithm is for AI decision-making.
- The interface is created using ImGui.
- The `PieceForPlayer` function was modified to allow the AI to take turns.

## Challenges Faced

1. **Negamax Function Setup**: making the negamax function to evaluate the game state and determine the best AI move was challenging due to its recursive nature.
2. **updateAI Function Modification**: making the `updateAI` function to work with the negamax required careful adjustments to ensure good evaluation and selection of the best move.
3. **PieceForPlayer Function Update**: Modifying the `PieceForPlayer` function to enable the AI to take its turn while ensuring correct player symbol assignment presented difficulties in managing player turns and symbol allocation.

## Usage

To use the program, include the necessary libraries and run the application. Players take turns clicking on empty squares to place their symbols. The game continues until one player wins or the board is full.