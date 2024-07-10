#pragma once
#include "Game.h"
#include "Square.h"

//
// the classic game of Rock, Paper, Scissors
//
const int pieceSize = 100;

class tic_tac_toe : public Game
{
public:
    tic_tac_toe();
    ~tic_tac_toe();

    // set up the board
    void setUpBoard() override;

    Player* checkForWinner() override;
    bool checkForDraw() override;
    bool isBoardFull();
    std::string initialStateString() override;
    std::string stateString() override;
    void setStateString(const std::string &s) override;
    bool actionForEmptyHolder(BitHolder &holder) override;
    bool canBitMoveFrom(Bit&bit, BitHolder &src) override;
    bool canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst) override;
    void stopGame() override;
    BitHolder &getHolderAt(const int x, const int y) override { return _grid[x+y*3]; }
    
    //my added code
    virtual bool gameHasAI() override {return true;};
    int negamax(tic_tac_toe* state, int depth, int playerColor);
    int evaluateBoard();
    Square  _grid[9];
    void updateAI();
private:
    Bit *PieceForPlayer(const int playerNumber);
    Player* ownerAt(int index );
    void    scanForMouse();
};

