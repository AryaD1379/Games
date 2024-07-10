#include "tic_tac_toe.h"

const int AI_PLAYER = 1;
const int HUMAN_PLAYER = -1;

tic_tac_toe::tic_tac_toe()
{
}

tic_tac_toe::~tic_tac_toe()
{
}

//
// make a x, y piece for the player
//
Bit* tic_tac_toe::PieceForPlayer(const int playerNumber)
{
    const char *textures[] = { "o.png", "x.png"};
    //int random = rand() % 2;

    Bit *bit = new Bit();
    // should possibly be cached from player class?
    bit->LoadTextureFromFile(playerNumber == AI_PLAYER ? textures[0] : textures[1]);
    bit->setOwner(getPlayerAt(playerNumber == AI_PLAYER ? 1 : 0));
    bit->setGameTag(playerNumber);
    bit->setSize(pieceSize, pieceSize);
    return bit;
}

void tic_tac_toe::setUpBoard()
{
    srand((unsigned int)time(0));
    setNumberOfPlayers(2);
    // this allows us to draw the board correctly
    _gameOptions.rowX = 3;
    _gameOptions.rowY = 3;
    // setup the board
    int i = 0;
    for (int y=0; y<3; y++) {
        for (int x = 0; x < 3; x++){
            _grid[i].initHolder(ImVec2(100*(float)x + 100, 100*(float)y + 100), "square.png", x, y);
            i++;
        }
    }
    // if we have an AI set it up
    if (gameHasAI())
    {
        setAIPlayer(AI_PLAYER);
    }
    // setup up turns etc.
    startGame();
}

//
// about the only thing we need to actually fill out for rock paper scissors
//
bool tic_tac_toe::actionForEmptyHolder(BitHolder& holder)
{
    if (holder.bit()) {
        return false;
    }
    Bit *bit = PieceForPlayer(getCurrentPlayer()->playerNumber()==0 ? HUMAN_PLAYER : AI_PLAYER);
    if (bit) {
        bit->setPosition(holder.getPosition());
        holder.setBit(bit);
        endTurn();
        return true;
    }   
    return false;
}

bool tic_tac_toe::canBitMoveFrom(Bit& bit, BitHolder& src)
{
    // you can't move anything in rock paper scissors
    return false;
}

bool tic_tac_toe::canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst)
{
    // you can't move anything in rock paper scissors
    return false;
}

//
// free all the memory used by the game on the heap
//
void tic_tac_toe::stopGame()
{
    for (int x=0; x<9; x++) {
        _grid[x].destroyBit();
    }
}

//
// helper function for the winner check
//
Player* tic_tac_toe::ownerAt(int index )
{
    if (index < 0 || index > 8) {
        return nullptr;
    }
    if (!_grid[index].bit()) {
        return nullptr;
    }
    return _grid[index].bit()->getOwner();
}

Player* tic_tac_toe::checkForWinner()
{
    static const int kWinningTriples[8][3] =  { {0,1,2}, {3,4,5}, {6,7,8},  // rows
                                                {0,3,6}, {1,4,7}, {2,5,8},  // cols
                                                {0,4,8}, {2,4,6} };         // diagonals

    for (int i = 0; i < 8; i++){
        Player *p1 = ownerAt(kWinningTriples[i][0]);
        if (p1 != nullptr) {
            if (p1 == ownerAt(kWinningTriples[i][1])&& p1 == ownerAt(kWinningTriples[i][2])){
                return p1;
            }
        }
    }
    return nullptr;
}

bool tic_tac_toe::checkForDraw()
{
    if (checkForWinner()){
        return false;
    }
    for (int i = 0; i < 9; i++){
        if (!_grid[i].bit()){
            return false;
        }
    }
    return true;
}

//
// state strings
//
std::string tic_tac_toe::initialStateString()
{
    return "00";
}

//
// this still needs to be tied into imguis init and shutdown
// we will read the state string and store it in each turn object
//
std::string tic_tac_toe::stateString()
{
    std::string s;
    for (int x=0; x<9; x++) {
        Bit *bit = _grid[x].bit();
        if (bit) {
            s += bit->gameTag() == HUMAN_PLAYER ? "1" : "2";
        } else {
            s += "0";
        }
    }
    return s;
}

//
// this still needs to be tied into imguis init and shutdown
// when the program starts it will load the current game from the imgui ini file and set the game state to the last saved state
//
void tic_tac_toe::setStateString(const std::string &s)
{
    for (int x=0; x<2; x++) {
        _grid[x].setBit( PieceForPlayer(s[x] == '1' ? HUMAN_PLAYER : AI_PLAYER) );
    }
}
//checks if the baord is full
bool tic_tac_toe::isBoardFull()
{
    for (int i = 0; i < 9; i++){
        if (!_grid[i].bit()){
            return false;
        }
    }
    return true;
}

void tic_tac_toe::updateAI() {
    int bestMove = -1;
    int bestValue = -1000; // Initialize to a very low value

    // Loop through all possible moves
    for (int move = 0; move < 9; move++) {
        if (!_grid[move].bit()) { // Check if the cell is empty
            _grid[move].setBit(PieceForPlayer(AI_PLAYER)); // Assume AI
            int value = -negamax(this, 0, HUMAN_PLAYER); // Assuming HUMAN
            _grid[move].destroyBit();

            std::cout << "Move: " << move << " Value: " << value << std::endl;
            // Update the best move if a better move is found
            if (value > bestValue) {
                bestValue = value;
                bestMove = move;
            }
        }
    }

    // Make the best move
    if (bestMove != -1) {
        if (actionForEmptyHolder(_grid[bestMove])) {
        }
    }
}

int tic_tac_toe::evaluateBoard(){
    Player *p = checkForWinner();
    if (p){
        if (p->isAIPlayer()){
            return 10;
        }
        else {
            return -10;
        }
    } 
    else {
        return 0;
    }
}

//negamax function for out AI
int tic_tac_toe::negamax(tic_tac_toe* state, int depth, int playerColor){
    int score = state->evaluateBoard();

    if (score != 0){
        return playerColor * score;
    }
    if (state->isBoardFull()) return 0;
   
    int bestval = -1000;

    for (int y = 0; y < 9; y++){
            if (!state->_grid[y].bit()){
                state->_grid[y].setBit(PieceForPlayer(playerColor));
                bestval = std::max(bestval, -negamax(state, depth+1, -playerColor));
                state->_grid[y].destroyBit();
            } 
    }
    return bestval;
}
