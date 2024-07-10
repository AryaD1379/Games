#pragma once
#include "Game.h"
#include "ChessSquare.h"

//
// the classic game of Rock, Paper, Scissors
//
const int pieceSize = 64;

enum ChessPiece {
    Pawn = 1,
    Knight, 
    Bishop,
    Rook,
    Queen, 
    King
};

enum Castling {
    GameStarted = 0,
    WhiteKingRookMoved = 0x1,
    WhiteQueenRookMoved = 0x2,
    BlackKingRookMoved = 0x4,
    BlackQueenRookMoved = 0x8,
    WhiteKingMoved = 0x10,
    BlackKingMoved = 0x20 
};

class Chess : public Game
{
public:
    Chess();
    ~Chess();

    struct Move {
        std::string from;
        std::string to;
        bool enPassantMove;
    };

    // set up the board
    void setUpBoard() override;

    Player* checkForWinner() override;
    bool checkForDraw() override;
    bool isBoardFull();
    std::string initialStateString() override;
    std::string stateString() override;
    void setStateString(const std::string &s) override;
    bool actionForEmptyHolder(BitHolder &holder) override {return false;}
    bool canBitMoveFrom(Bit&bit, BitHolder &src) override;
    bool canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst) override;
    void bitMovedFromTo(Bit& bit, BitHolder& src, BitHolder& dst) override;
    void stopGame() override;
    BitHolder &getHolderAt(const int x, const int y) override { return _grid[y][x]; }
    char pieceColor(const char color) const;
    //my added code
    virtual bool gameHasAI() override {return true;};
    int negamax(char* state, int depth, int alpha, int beta, int playerColor);
    int evaluateBoard(const char* state);
    void updateAI() override;
    int notationToIndex(std::string& notation);
    void filterOutIllegalMoves(std::vector<Chess::Move>& moves, char color);

private:
    Bit *PieceForPlayer(const int playerNumber, ChessPiece piece);
    void addMoveIfValid(const char *state, std::vector<Move>& moves, int fromRow, int fromCol, int toRow, int toCol, bool enPassant = false);
    std::string indexToNotation(int row, int col);
    const char pieceNotation(int row, int column) const;
    void generateKnightMoves(const char *state, std::vector<Move>&moves, int row, int col);
    void generatePawnMoves(const char *state, std::vector<Move>&moves, int row, int col, char color, std::string lastMove);
    void generateLinearMoves(const char *state, std::vector<Move>&moves, int row, int col, const std::vector<std::pair<int,int>>&directions);
    void generateBishopMoves(const char *state, std::vector<Move>&moves, int row, int col);
    void generateRookMoves(const char *state, std::vector<Move>&moves, int row, int col);
    void generateQueenMoves(const char *state, std::vector<Move>&moves, int row, int col);
    void generateKingMoves(const char *state, std::vector<Move>&moves, int row, int col, int castleStatus, char color);
    bool isEnPassantMove(ChessSquare &srcSquare, ChessSquare &dstSquare);
    
    void FENtoBoard(const std::string& fen);
    int _castleStatus;
    char oppositeColor(char color);
    void    scanForMouse();
    char stateNotation(const char* state, int row, int col);
    char stateColor(const char* state, int row, int col);

    std::vector<Chess::Move> generateMoves(const char *state, char color, std::string lastMove, bool filter);
    ChessSquare  _grid[8][8];
    std::vector<Move> _moves;
};

