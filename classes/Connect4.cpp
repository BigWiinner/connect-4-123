#include "Connect4.h"

Connect4::Connect4() : Game()
{
    _grid = new Grid(7,6);
}

Connect4::~Connect4()
{
    delete _grid;
}

Bit* Connect4::createPiece(Player* player) {
    Bit* bit = new Bit();
    bit->LoadTextureFromFile(player == getPlayerAt(YELLOW_PLAYER) ? "yellow.png" : "red.png");
    bit->setOwner(player);
    return bit;
}

void Connect4::setUpBoard()
{
    setNumberOfPlayers(2);
    _gameOptions.rowX = 7;
    _gameOptions.rowY = 6;
    _grid->initializeSquares(80, "square.png");
    move = 0;

//    if (gameHasAI()) {
//        setAIPlayer(YELLOW_PLAYER);
//    }

    startGame();
}

bool Connect4::actionForEmptyHolder(BitHolder &holder) {
    if (holder.bit()) { return false; }

    ChessSquare* square = static_cast<ChessSquare*>(&holder);
    if (!square) 
    {
        return false; 
    }
    int col = square->getColumn();
    if (col == -1) 
    {
        return false;
    }
    if (isColumnFull(col)) 
    {
        return false;
    }

    int targetRow = getTargetRow(col);
    if (targetRow == -1)
    {
        return false;
    }

    Player* currentPlayer = getCurrentPlayer();
    Bit* bit = createPiece(currentPlayer);
    if (bit) {
        ChessSquare* targetSquare = _grid->getSquare(col, targetRow);
        bit->setPosition(targetSquare->getPosition());
        targetSquare->setBit(bit);
        endTurn();
        move++;
        return true;
    }
    return false;
}

bool Connect4::isColumnFull(int col) {
    ChessSquare* topSquare = _grid->getSquare(col, 0);
    return topSquare && topSquare->bit() != nullptr;
}

int Connect4::getTargetRow(int col) {
    for(int row = 5; row >= 0; row--) {
        ChessSquare* square = _grid->getSquare(col, row);
        if (square && !square->bit()) {
            return row;
        }
    }
    return -1;
}

inline uint64_t bb_bit(int col, int row) {
    return uint64_t(1) << (col * 7 + row);
}

inline uint64_t board_to_bitboard(const char board[], char player) {
    uint64_t bb = 0;
    for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 7; col++) {
            if (board[row * 7 + col] == player) {
                bb |= bb_bit(col, row);
            }
        }
    }
    return bb;
}

bool checkWinShift(uint64_t bb) {
    uint64_t t;

    t = bb & (bb >> 1);
    if (t & (t >> 2)) { return true; }

    t = bb & (bb >> 7);
    if (t & (t >> 14)) { return true; }

    t = bb & (bb >> 8);
    if (t & (t >> 16)) { return true; }

    t = bb & (bb >> 6);
    if (t & (t >> 12)) { return true; }

    return false;
}

Player* Connect4::checkForWinner() {
    std::string state = stateString();
    const char* board = state.c_str();
    uint64_t firstPlayerBB = board_to_bitboard(board, '1');
    uint64_t secondPlayerBB = board_to_bitboard(board, '2');
    if (checkWinShift(firstPlayerBB)) { return getPlayerAt(0); }
    if (checkWinShift(secondPlayerBB)) { return getPlayerAt(1); }
    return nullptr;
}

bool Connect4::checkForDraw() {
    return move == 41;
}

std::string Connect4::initialStateString() {
    return "000000000000000000000000000000000000000000";
}

std::string Connect4::stateString() {
    std::string state = "000000000000000000000000000000000000000000";
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        Bit* bit = square->bit();
        if (bit) {
            int index = (5 - y) * 7 + x;
            state[index] = bit->getOwner()->playerNumber() + 1 + '0';
        }
    });
    return state;
}

void Connect4::setStateString(const std::string &s) {
    if (s.length() != 42) return;

    _grid->setStateString(s);

    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        int index = (5 - y) * 7 + x;
        int playerNumber = s[index] - '0';
        if (playerNumber) {
            square->setBit(createPiece(getCurrentPlayer()));
        } else {
            square->setBit(nullptr);
        }
    });
    return;
}

bool Connect4::canBitMoveFrom(Bit &bit, BitHolder &src) {
    return false;
}

bool Connect4::canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst) {
    return false;
}

void Connect4::stopGame() {
    _grid->forEachSquare([](ChessSquare* square, int x, int y) {
        square->destroyBit();
    });
}

void Connect4::bitMovedFromTo(Bit &bit, BitHolder &src, BitHolder &dst) {
    return;
}

void Connect4::updateAI() {
    return;
}