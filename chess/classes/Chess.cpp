#include "Chess.h"
const int AI_PLAYER = 1;
const int HUMAN_PLAYER = -1;

const char Chess::pieceNotation(int row, int column) const{
    const char *wpieces = {"?PNBRQK"};
    const char *bpieces = {"?pnbrqk"};
    char notation = '0';
    Bit *bit = _grid[row][column].bit();
    if (bit){
        notation = bit->gameTag() < 128 ? wpieces[bit->gameTag()] : bpieces[bit->gameTag()&127];
    }
    return notation;
}

Chess::Chess()
{
}

Chess::~Chess()
{
}

//
// make a x, y piece for the player
//
Bit* Chess::PieceForPlayer(const int playerNumber, ChessPiece piece)
{
    const char *pieces[] = { "pawn.png", "knight.png", "bishop.png", "rook.png",
    "queen.png", "king.png"};
    //int random = rand() % 2;

    Bit *bit = new Bit();
    // should possibly be cached from player class?
    const char *pieceName = pieces[piece-1];
    std::string spritePath = std::string("chess/") + (playerNumber == 0 ? "w_": "b_") + pieceName;
    bit->LoadTextureFromFile(spritePath.c_str());
    bit->setOwner(getPlayerAt(playerNumber));
    bit->setSize(pieceSize, pieceSize);
    return bit;
}

void Chess::FENtoBoard(const std::string& fen)
{
    // convert a FEN string to a board
    // FEN is a space delimited string with 6 fields
    // 1: piece placement (from white's perspective)
    // 2: active color (W or B)
    // 3: castling availability (KQkq or -)
    // 4: en passant target square (in algebraic notation, or -)
    // 5: halfmove clock (number of halfmoves since the last capture or pawn advance)

    // Clear the board
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
            _grid[i][j].setBit(nullptr);

    std::istringstream fenStream(fen);
    std::string boardPart;
    std::getline(fenStream, boardPart, ' ');

    int row = 7;
    int col = 0;
    for (char ch : boardPart) {
        if (ch == '/') {
            row--;
            col = 0;
        } else if (isdigit(ch)) {
            col += ch - '0'; // Skip empty squares
        } else {
            // convert ch to a piece
            ChessPiece piece = Pawn;
            switch (toupper(ch)) {
                case 'P':
                    piece = Pawn;
                    break;
                case 'N':
                    piece = Knight;
                    break;
                case 'B':
                    piece = Bishop;
                    break;
                case 'R':
                    piece = Rook;
                    break;
                case 'Q':
                    piece = Queen;
                    break;
                case 'K':
                    piece = King;
                    break;
            }
            Bit *bit = PieceForPlayer(isupper(ch) ? 0 : 1, piece);
            bit->setPosition(_grid[row][col].getPosition());
            bit->setParent(&_grid[row][col]);
            bit->setGameTag(isupper(ch) ? piece : (piece + 128));
            bit->setSize(pieceSize, pieceSize);
            _grid[row][col].setBit( bit );
            col++;
        }
    }        
}
void Chess::setUpBoard()
{
    setNumberOfPlayers(2);
    // this allows us to draw the board correctly
    _gameOptions.rowX = 8;
    _gameOptions.rowY = 8;
    // setup the board
    for (int y=0; y < 8; y++) {
        for (int x = 0; x < 8; x++){
            ImVec2 position((float)(pieceSize * x + pieceSize), (float)(pieceSize * (_gameOptions.rowY - y) + pieceSize));
            _grid[y][x].initHolder(position, "boardsquare.png", x, y);
            _grid[y][x].setGameTag(0);
            _grid[y][x].setNotation(indexToNotation(y, x));
        }
    }
    
    FENtoBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    // if we have an AI set it up
    if (gameHasAI())
    {
        setAIPlayer(AI_PLAYER);
    }
    // setup up turns etc.
    _moves = generateMoves(stateString().c_str(), 'W', "", true);
    startGame();
}

bool Chess::canBitMoveFrom(Bit& bit, BitHolder& src)
{
    ChessSquare &srcSquare = static_cast<ChessSquare&>(src);
    for (auto move : _moves){
        if (move.from == srcSquare.getNotation()){
            return true;
        }
    }
    return false;
}

bool Chess::canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst)
{
    ChessSquare &srcSquare = static_cast<ChessSquare&>(src);
    ChessSquare &dstSquare = static_cast<ChessSquare&>(dst);

    for (auto move : _moves){
        if (move.from == srcSquare.getNotation() && move.to == dstSquare.getNotation()){
            return true;
        }
    }
    return false;
}

bool Chess::isEnPassantMove(ChessSquare &srcSquare, ChessSquare &dstSquare)
{
    for (auto move : _moves){
        if (move.from == srcSquare.getNotation() && move.to == dstSquare.getNotation() && move.enPassantMove == true){
            return true;
        }
    }
    return false;
}

void Chess::bitMovedFromTo(Bit& bit, BitHolder& src, BitHolder& dst){
    // my added code 
    const char *bpieces = "pnbrqk";
    const char *wpieces = "PNBRQK";
    ChessSquare &srcSquare = static_cast<ChessSquare&>(src);
    ChessSquare &dstSquare = static_cast<ChessSquare&>(dst);
    _lastMove = "x-" + srcSquare.getNotation() + "-" + dstSquare.getNotation();
    _lastMove[0] = (bit.gameTag() < 128) ? wpieces[bit.gameTag()-1] : bpieces[bit.gameTag() - 129];

    switch (_lastMove[0]){
        case 'K':
            _castleStatus |= WhiteKingMoved;
            break;
        case 'k':
            _castleStatus |= BlackKingMoved;
            break;
        case 'R':
            if(_lastMove[1] == 'a'){
                 _castleStatus |= WhiteQueenRookMoved;
            }
            else if (_lastMove[1] == 'h'){
                _castleStatus |= WhiteKingRookMoved;
            }
            break;
        case 'r':
            if(_lastMove[1] == 'a'){
                 _castleStatus |= BlackQueenRookMoved;
            }
            else if (_lastMove[1] == 'h'){
                _castleStatus |= BlackKingRookMoved;
            }
            break;
    }
   
    //check for rook being taken
    if (dstSquare.getNotation() == "a1") _castleStatus |= WhiteQueenRookMoved;
    if (dstSquare.getNotation() == "h1") _castleStatus |= WhiteKingRookMoved;
    if (dstSquare.getNotation() == "a8") _castleStatus |= BlackQueenRookMoved;
    if (dstSquare.getNotation() == "h8") _castleStatus |= BlackKingRookMoved;

    //en passant
    if (((bit.gameTag()&127) == ChessPiece::Pawn) && (srcSquare.getColumn() != dstSquare.getColumn()) && isEnPassantMove(srcSquare, dstSquare)){
        int row = (bit.gameTag() > 128) ? 3 : 4;
        if (srcSquare.getRow() == row){
            BitHolder &enPassantSqaure = getHolderAt(dstSquare.getColumn(), row);
            Bit *enPassantBit = enPassantSqaure.bit();
            if (enPassantBit){
                enPassantSqaure.destroyBit();
            }
        }
    }
    
    //check for pawn promotion
    if (((bit.gameTag()&127) == ChessPiece::Pawn) && ((dstSquare.getRow() == 0) || (dstSquare.getRow() == 7))){
        int PlayerNumber = (bit.gameTag() < 128) ? 0 : 1;
        Bit *newBit = PieceForPlayer(PlayerNumber, Queen);
        newBit->setPosition(dstSquare.getPosition());
        newBit->setParent(&dstSquare);
        newBit->setGameTag((bit.gameTag() < 128) ? Queen : (Queen + 128));
        dstSquare.setBit(newBit);
    } 

    //check for castling 
    int distance = srcSquare.getDistance(dstSquare);
    if (((bit.gameTag()&127) == ChessPiece::King) && (distance == 2)){
        int rookSrcCol = (dstSquare.getColumn() == 6) ? 7 : 0;
        int rookDstCol = (dstSquare.getColumn() == 6) ? 5 : 3;
        BitHolder &rookSrc = getHolderAt(rookSrcCol, dstSquare.getRow());
        BitHolder &rookDst = getHolderAt(rookDstCol, dstSquare.getRow());
        Bit *rook = rookSrc.bit();
        rookDst.setBit(rook);
        rookSrc.setBit(nullptr);
        rook->setPosition(rookDst.getPosition());
    }

    Game::bitMovedFromTo(bit, src, dst);
    _moves = generateMoves(stateString().c_str(), (_gameOptions.currentTurnNo&1) ? 'B' : 'W', _lastMove, true); 

}
//
// free all the memory used by the game on the heap
//
void Chess::stopGame()
{
    for (int y = 0; y < 8; y++){
        for (int x=0; x<8; x++) {
            _grid[y][x].destroyBit();
        }
    }
}

char Chess::stateNotation(const char* state, int row, int col) 
{
    return state[row * 8 + col];
}

char Chess::stateColor(const char* state, int row, int col) 
{
    // return 0 for empty, 1 for white, -1 for black
    char piece = stateNotation(state, row, col);
    if (piece == '0') {
        return '0';
    }
    return (piece < 'a') ? 'W' : 'B';
}


void Chess::addMoveIfValid(const char *state, std::vector<Move>& moves, int fromRow, int fromCol, int toRow, int toCol, bool enPassant){
    if (toRow >= 0 && toRow < 8 && toCol >= 0 && toCol < 8){
        if (stateColor(state, fromRow, fromCol) != stateColor(state, toRow, toCol)){
            moves.push_back({indexToNotation(fromRow, fromCol), indexToNotation(toRow, toCol), enPassant});
        }
    }
}

char Chess::pieceColor(const  char color) const {
    if (color == '0'){
        return '0';
    }
    return isupper(color) ? 'W' : 'B';
}
std::string Chess::indexToNotation(int row, int col){
    return std::string(1, 'a' + col) + std::string(1, '1' + row); 
}

//generate moves for knight 
void Chess::generateKnightMoves(const char *state, std::vector<Move>&moves, int row, int col){
    static const int movesRow[] = {2, 1, -1, -2, -2, -1, 1, 2};
    static const int movesCol[] = {1, 2, 2, 1, -1, -2, -2, -1};

    for (int i = 0; i < 8; i++){
        int newRow = row + movesRow[i];
        int newCol = col + movesCol[i];
        addMoveIfValid(state, moves, row, col, newRow, newCol);
    }
}

//generate moves for pawn
void Chess::generatePawnMoves(const char *state, std::vector<Move>&moves, int row, int col, char color, std::string lastMove){
    int direction = (color == 'W') ? 1 : -1;
    int startRow = (color == 'W') ? 1 : 6;

    if (stateNotation(state, row + direction, col) == '0'){
        addMoveIfValid(state, moves, row, col, row + direction, col);

        if (row == startRow && stateNotation(state, row + 2 * direction, col) == '0'){
            addMoveIfValid(state, moves, row, col, row +2 * direction, col);
        }
    }

    for (int i = -1; i <= 1; i+=2){
        if (col + i >= 0 && col + i < 8){
            if (stateColor(state, row + direction, col + i) == oppositeColor(color)){
                addMoveIfValid(state, moves, row, col, row + direction, col + i);
            }
        }
    }

    if (lastMove.length()){
        char lastMovePiece = lastMove[0];
        int lastMoveStartRow = lastMove[3] - '0';
        int lastMoveEndRow = lastMove[6] - '0';
        int lastMoveStartCol = lastMove[2] - 'a';
        int lastMoveEndCol = lastMove[5] - 'a';

        if (color == 'W' && row == 4){
            if (lastMovePiece == 'p' && lastMoveStartRow == 7 && lastMoveEndRow == 5){
                if (lastMoveEndCol == col - 1 || lastMoveEndCol == col + 1){
                    addMoveIfValid(state, moves, row, col, row+1, lastMoveEndCol, true);
                    
                }
            }
        }
        else if (color == 'B' && row == 3){
            if (lastMovePiece == 'P' && lastMoveStartRow == 2 && lastMoveEndRow == 4){
                if (lastMoveEndCol == col +1 || lastMoveEndCol == col -1){
                    addMoveIfValid(state, moves, row, col, row - 1, lastMoveEndCol, true);
                }
            }
        }
    }
}

void Chess::generateLinearMoves(const char *state, std::vector<Move>&moves, int row, int col, const std::vector<std::pair<int,int>>&directions){
    for (auto& dir : directions){
        int currentRow = row + dir.first;
        int currentCol = col + dir.second;
        while (currentRow >= 0 && currentRow < 8 && currentCol >= 0 && currentCol < 8){
            if(pieceNotation(currentRow, currentCol) != '0'){
                addMoveIfValid(state, moves, row, col, currentRow, currentCol);
                break;
            }
            addMoveIfValid(state, moves, row, col, currentRow, currentCol);
            currentRow += dir.first;
            currentCol += dir.second;
        }
    }
}

void Chess::generateBishopMoves(const char *state, std::vector<Move>&moves, int row, int col){
    static const std::vector<std::pair<int, int>> directions = {{1, 1}, {1, -1}, {-1, 1}, 
    {-1, -1}};
    generateLinearMoves(state, moves, row, col, directions);
}

void Chess::generateRookMoves(const char *state, std::vector<Move>&moves, int row, int col){
    static const std::vector<std::pair<int, int>> directions = {{1, 0}, {-1, 0}, {0, 1}, 
    {0, -1}};
    generateLinearMoves(state, moves, row, col, directions);
}

void Chess::generateQueenMoves(const char *state, std::vector<Move>&moves, int row, int col){
    generateRookMoves(state, moves, row, col);
    generateBishopMoves(state, moves, row, col);
}

void Chess::generateKingMoves(const char *state, std::vector<Move>&moves, int row, int col, int castleStatus, char color){
    static const std::vector<std::pair<int, int>> directions = {{1, 0}, {-1, 0}, {0, 1}, 
    {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

    for (auto& dir : directions){
        int newRow = row + dir.first;
        int newCol = col + dir.second;
        if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8){
            addMoveIfValid(state, moves, row, col, newRow, newCol);
        }
    }
    //castling
    uint64_t _opponentMovesBitBoard = 0;
    int kingCastleStatus = (color == 'B') ? castleStatus&(BlackKingMoved) : castleStatus&(WhiteKingMoved);
    if (kingCastleStatus == 0){
        int kingRookcanCastle = (color == 'B') ? castleStatus&(BlackKingRookMoved) : castleStatus&(WhiteKingRookMoved);
        if (kingRookcanCastle == 0 && pieceNotation(row, col + 1) == '0' && pieceNotation(row, col + 2) == '0'){
            //check noty moving across into check
            uint64_t kingSideCastleBitBoard = 1ULL << (row * 8 + col + 1);
            uint64_t kingSideCastleBitBoard2 = 1ULL << (row * 8 + col + 2);
            if (((kingSideCastleBitBoard | kingSideCastleBitBoard2) & _opponentMovesBitBoard) == 0){
                addMoveIfValid(state, moves, row, col, row, col+2);
            }
        }
        int queenRookcanCastle = (color == 'B') ? castleStatus&(BlackQueenRookMoved) : castleStatus&(WhiteQueenRookMoved);
        if (queenRookcanCastle == 0 && pieceNotation(row, col - 1) == '0' && pieceNotation(row, col - 2) == '0'){
            //check noty moving across into check
            uint64_t kingSideCastleBitBoard = 1ULL << (row * 8 + col - 1);
            uint64_t kingSideCastleBitBoard2 = 1ULL << (row * 8 + col - 2);
            if (((kingSideCastleBitBoard | kingSideCastleBitBoard2) & _opponentMovesBitBoard) == 0){
                addMoveIfValid(state, moves, row, col, row, col-2);
            }
        }
    }
}

char Chess::oppositeColor(char color){
    return (color == 'W') ? 'B' : 'W';
}

void Chess::filterOutIllegalMoves(std::vector<Chess::Move>& moves, char color){
    char baseState[65];
    std::string copyState = std::string(stateString().c_str());

    int kingSquare = -1;
    for (int i = 0; i < 64; i++){
        if (copyState[i] == 'k' && color == 'B'){ kingSquare = i; break;}
        if(copyState[i] == 'K' && color == 'W') {kingSquare = i; break;}
    }
    for (auto it = moves.begin(); it != moves.end();){
        bool moveBad = false; 
        memcpy(&baseState[0],copyState.c_str(), 64);
        int srcSquare = notationToIndex(it->from);
        int dstSquare = notationToIndex(it->to);
        baseState[dstSquare] = baseState[srcSquare];
        baseState[srcSquare] = '0';
        // Handle the case in which the king is the piece that moved, and it may have left or remained in check
        int updatedKingSquare = kingSquare;
        if (baseState[dstSquare] == 'k' && color == 'B')
        {
            updatedKingSquare = dstSquare;
        }
        else if (baseState[dstSquare] == 'K' && color == 'W')
        {
            updatedKingSquare = dstSquare;
        }
        auto oppositeMoves = generateMoves(baseState, oppositeColor(color), _lastMove, false);
        for (auto enemyMoves : oppositeMoves) {
            int enemyDst = notationToIndex(enemyMoves.to);
            if (enemyDst == updatedKingSquare){
                moveBad = true;
                break;
            }
        }
        if (moveBad){
            it = moves.erase(it);
        }
        else {
            ++it;
        }
    }
}

std::vector<Chess::Move> Chess::generateMoves(const char *state, char color, std::string lastMove, bool filter){
    
    std::vector<Move> moves;

    for (int row = 0; row < 8; ++row){
        for (int col = 0; col < 8; ++col){
            char piece = state[row*8 + col];
            char pieceColor = isupper(piece) ? 'W' : 'B';
            if (piece != '0' && pieceColor == color){
                switch (toupper(piece)){
                    case 'N':
                        generateKnightMoves(state, moves, row, col);
                        break;
                    case 'P':
                        generatePawnMoves(state, moves, row, col, pieceColor, lastMove);
                        break;
                    case 'B':
                        generateBishopMoves(state, moves, row, col);
                        break;
                    case 'R':
                        generateRookMoves(state, moves, row, col);
                        break;
                    case 'Q':
                        generateQueenMoves(state, moves, row, col);
                        break;
                    case 'K':
                        generateKingMoves(state, moves, row, col, _castleStatus, color);
                        break;
                }
            }
        }
    }
    if (filter){
        filterOutIllegalMoves(moves, color);
    }
    return moves;
}

Player* Chess::checkForWinner()
{
    return nullptr;
}

bool Chess::checkForDraw()
{
    return false;
}

//
// state strings
//
std::string Chess::initialStateString()
{
    return stateString();
}

//
// this still needs to be tied into imguis init and shutdown
// we will read the state string and store it in each turn object
//
std::string Chess::stateString()
{
    std::string s;
    for (int y=0; y < 8; y++) {
        for (int x = 0; x < 8; x++){
            s += pieceNotation(y, x);
        }
    }
    return s;
}

//
// this still needs to be tied into imguis init and shutdown
// when the program starts it will load the current game from the imgui ini file and set the game state to the last saved state
//
void Chess::setStateString(const std::string &s)
{
    for (int y=0; y<8; y++) {
        for (int x = 0; x < 8; x++){
            int index = y*_gameOptions.rowX + x;
            int playerNumber = s[index] - '0';
            if (playerNumber){
                _grid[y][x].setBit(PieceForPlayer(playerNumber-1, Pawn));
            }
            else {
                _grid[y][x].setBit(nullptr);
            }
        }
        
    }
}
int Chess::notationToIndex(std::string& notation){
    int square = notation[0] - 'a';
    square += (notation[1] - '1') * 8;
    return square;
}

void Chess::updateAI() { 
    char baseState[65];
    const int infin = 99999999;
    int bestMoveScore = -infin;
    Move bestMove;
    std::string copyState = stateString();
   
    // Loop through all possible moves
    for (auto move : _moves) {
        memcpy(&baseState[0],copyState.c_str(), 64);
        int srcSquare = notationToIndex(move.from);
        int dstSquare = notationToIndex(move.to);
        baseState[dstSquare] = baseState[srcSquare];
        baseState[srcSquare] = '0';
        int bestValue = -negamax(baseState, 3, -infin, infin, 1);
        if(bestValue > bestMoveScore){
            bestMoveScore = bestValue;
            bestMove = move;
        }
    }
    if (bestMoveScore != -infin){
        int srcSquare = notationToIndex(bestMove.from);
        int dstSquare = notationToIndex(bestMove.to);
        BitHolder& src = getHolderAt(srcSquare&7, srcSquare/8);
        BitHolder& dst = getHolderAt(dstSquare&7, dstSquare/8);
        Bit* bit = src.bit();
        dst.dropBitAtPoint(bit, ImVec2(0, 0));
        src.setBit(nullptr);
        bitMovedFromTo(*bit, src, dst);
    }
}

static std::map<char, int> evaluateBoardScores = {
        {'P', 100}, {'p', -100},
        {'N', 200}, {'n', -200},
        {'B', 230}, {'b', -230},
        {'R', 400}, {'r', -400},
        {'Q', 900}, {'q', -900},
        {'K', 2000}, {'k', -2000},
        {'0', 0}
    };

int Chess::evaluateBoard(const char* state){
    int score = 0;
    for (int i = 0; i < 64; i++){
        score += evaluateBoardScores[state[i]];
    }
    return score;
}

//negamax function for out AI
int Chess::negamax(char* state, int depth, int alpha, int beta, int playerColor){
    
    if (depth == 0){
        int score = evaluateBoard(state);
        return playerColor * score;
    }
    const int infin = 99999999;
    int bestval = -infin;
    char color = (playerColor == 1 ? 'W' : 'B');
    auto negaMoves = generateMoves(state, color, state, true);
    for (auto move: negaMoves){
        int srcSquare = notationToIndex(move.from);
        int dstSquare = notationToIndex(move.to);
        char saveMove = state[dstSquare];
        state[dstSquare] = state[srcSquare];
        state[srcSquare] = '0';
        bestval = std::max(bestval, -negamax(state, depth-1, -beta, -alpha, -playerColor));
        state[srcSquare] = state[dstSquare];
        state[dstSquare] = saveMove;
        alpha = std::max(alpha, bestval);
        if (alpha >= beta){
            break;
        }
    }
    return bestval;
}