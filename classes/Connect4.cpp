#include "Connect4.h"

Connect4::Connect4(int AI) : Game()
{
    _grid = new Grid(7,6);
    AI_NUMBER = AI;
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

    if (gameHasAI() && AI_NUMBER != -1) {
        setAIPlayer(AI_NUMBER == 0 ? YELLOW_PLAYER : RED_PLAYER);
    }

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

// Converts board into bitboard
// Bitboard will represent the bottom row as the top bits
// Also makes the bitboard have an extra row at the end to serve
//  as sentinal bits for checkWinShift to prevent false positives
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

/*
    Writing this down so that I can hopefully understand how this works

    checWinShift will check four separate win conditions:
        Vertical, Horizontal, Diagonal Left (\), and Diagonal Right (/)
    
    It will first shift everything once in that direction and doing an & comparison, leaving 3 bits if there were four in a row
        then it will shift two in that direction and do a & comparison so that only 1 bit remains in that row of four.

    If any bits remain after a bit shift by one step into bit shift by two steps in a direction, then that means there
        is a four in a row.

    If there are no four in a row sequences, the & comparisons will result in 0.

    Also the uint64_bb is "upside down", as in if there was a single yellow piece in the center of the bottom row, the board would read:
        0001000
        0000000
        0000000
        0000000
        0000000
        0000000
*/
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

int AICheckForWinner(const std::string& state, int player) {
    const char* board = state.c_str();
    uint64_t playerBB = board_to_bitboard(board, (player + 1) + '0');
    uint64_t enemyBB = board_to_bitboard(board, (2 - player) + '0');
    if (checkWinShift(playerBB)) { return 1000; }
    if (checkWinShift(enemyBB)) { return -1000; }
    return 0;
}

int AIGetTargetRow(const std::string& state, int col) {
    for(int row = 5; row >= 0; row--) {
        int index = (5 - row) * 7 + col;
        if (state[index] == '0') {
            return row;
        }
    }
    return -1;
}

int Connect4::negamax(std::string& state, int depth, int alpha, int beta, int player) {
    int curVal = AICheckForWinner(state, player);
    if (curVal) { 
        return curVal - depth;
    }
    if (depth == 0) {
        return 0;
    }
    if (state.find('0') == std::string::npos) return 0; // Check for draw


    int bestScore = -1000;
    int order[7] = {3, 4, 2, 5, 1, 6, 0};
    for (int i = 0; i < 7; i++) {
        int col = order[i];
        int row = AIGetTargetRow(state, col);
        if (row == -1) { continue; }

        int index = (5 - row) * 7 + col;

        state[index] = player == 0 ? '1' : '2';

        // Check if current player can win next move
        int winCheck = AICheckForWinner(state, player);
        if (winCheck > 0) {
            state[index] ='0';
            return 1000000;
        }

        int score = -negamax(state, depth - 1, -beta, -alpha, 1 - player);
        state[index] = '0';

        if (score > bestScore) {
            bestScore = score;
        }
        if (score > alpha) {
            alpha = score;
        }
        if (alpha >= beta) {
            break;
        }
    }

    return bestScore;
}

void Connect4::updateAI() {
    if (!gameHasAI()) return;

    int humanPlayer = AI_NUMBER == 0 ? RED_PLAYER : YELLOW_PLAYER;
    int aiPlayer = AI_NUMBER == 0 ? YELLOW_PLAYER : RED_PLAYER;
    int bestScore = -1000;
    int bestMove = -1;
    std::string state = stateString();

    int order[7] = {3, 4, 2, 5, 1, 6, 0};
    for (int i = 0; i < 7; i++) {
        int col = order[i];
        int targetRow = getTargetRow(col);
        if (targetRow == -1) continue;

        int index = (5 - targetRow) * 7 + col;
        state[index] = AI_NUMBER == 0 ? '1' : '2';

        int score = -negamax(state, 8, -10000, 10000, 1 - aiPlayer);

        state[index] = '0';

        if (score > bestScore) {
            bestScore = score;
            bestMove = col;
        }
    }
    
    if (bestMove != -1) {
        int row = getTargetRow(bestMove);
        ChessSquare* square = _grid->getSquare(bestMove, row);
        if (square) {
            actionForEmptyHolder(*square);
        }
    }
    else {
        for (int option : order) {
            int potentialRow = getTargetRow(option);
            if (potentialRow != -1) {
                ChessSquare* square = _grid->getSquare(option, potentialRow);
                if (square) {
                    actionForEmptyHolder(*square);
                    break;
                }
            }
        }
    }

    return;
}