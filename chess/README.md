# Chess Game with filterOutIllegalMoves fucntion

This Chess game features an implementation of the `filterOutIllegalMoves` function for checking checkmate and stalemate conditions.

## Implementation Details

- The game logic is encapsulated within the `Chess` class.
- Players are denoted by symbols: '0' and '1'.
- The user interface is designed using ImGui.
- The negamax algorithm is employed for the AI opponent.
- The `filterOutIllegalMoves` function is integrated to ascertain if the user is in checkmate or stalemate, leading to game termination when appropriate.


## Function Descriptions

### `void Chess::filterOutIllegalMoves(std::vector<Chess::Move>& moves, char color)`

This function filters out illegal moves from the provided list of moves based on whether the move exposes the current player's king to a threat. It iterates through each move, simulates the move, generates the opponent's moves, and checks if any of the opponent's moves put the player's king in check. If so, the move is deemed illegal and removed from the list. Otherwise, the move is considered legal and retained.

## Challenges

One of the notable challenges encountered during the implementation of the `filterOutIllegalMoves` function was managing the complexities associated with correctly identifying and filtering illegal moves, particularly when considering the intricate rules of chess.