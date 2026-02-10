#include "Connect4.h"
#include <iostream>

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

//    if (gameHasAI()) {
//        setAIPlayer(AI_PLAYER);
//    }

    startGame();
}

bool Connect4::actionForEmptyHolder(BitHolder &holder) {
    if (holder.bit()) { return false; }

    Player* currentPlayer = getCurrentPlayer();
    Bit* bit = createPiece(currentPlayer);
    if (bit) {
        ImVec2 pos = holder.getPosition(); // TODO make piece fall to bottom of board
        bit->setPosition(pos);
        holder.setBit(bit);
        endTurn();
        return true;
    }
    return false;
}

Player* Connect4::checkForWinner() {
    return nullptr;
}

bool Connect4::checkForDraw() {
    return false;
}

std::string Connect4::initialStateString() {
    return "000000000000000000000000000000000000000000";
}

std::string Connect4::stateString() {
    return "000000000000000000000000000000000000000000";
}

void Connect4::setStateString(const std::string &s) {
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