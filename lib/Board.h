// # Copyright (c) Dylan Leclair
#pragma once

#include "Move.h"
#include "PlayerColor.h"
#include "Piece.h"

#include <vector>
#include <iostream>
#include <algorithm>

#define DEFAULT_SELECTION {{0,0},false}

struct Selection
{
    std::pair<int,int> m_selection;
    bool m_exists;
};

class Board
{
public:
    PlayerColor Board::getColor(int row, int col);
    const std::vector<Move> &getValidMoves() const { return m_availableMoves; }
    void setValidMoves(std::pair<int, int> position);
    const PlayerColor getPlayerToMove() const;
    void move(Move move);
    void undo();
    const std::vector<std::vector<Piece>> &getBoard() { return m_board; }
    bool isInCheck(PlayerColor playerToMove);
    bool canKingMove(PlayerColor color);
    bool isCheckmate(PlayerColor winner);

    Selection m_selection = DEFAULT_SELECTION;

    // selection logic
    void select(int row, int col);
    void deselect();

    const void printBoard()
    {

        const std::string letters = "abcdefgh";

        for (auto &letter : letters)
        {
            std::cout << letter << " ";
        }
        std::cout << std::endl
                  << std::endl;
        for (int row = 0; row < 8; row++)
        {
            for (auto &letter : m_board[row])
            {
                std::cout << getChar(letter) << " ";
            }
            std::cout << " " << row + 1 << std::endl;
        }
    }

    Board()
    {

        m_board[0][0] = Piece::BLACK_ROOK;
        m_board[0][1] = Piece::BLACK_KNIGHT;
        m_board[0][2] = Piece::BLACK_BISHOP;
        m_board[0][3] = Piece::BLACK_QUEEN;
        m_board[0][4] = Piece::BLACK_KING;
        m_board[0][5] = Piece::BLACK_BISHOP;
        m_board[0][6] = Piece::BLACK_KNIGHT;
        m_board[0][7] = Piece::BLACK_ROOK;

        for (auto &piece : m_board[1])
        {
            piece = Piece::BLACK_PAWN;
        }

        for (auto &piece : m_board[6])
        {
            piece = Piece::WHITE_PAWN;
        }

        m_board[7][0] = Piece::WHITE_ROOK;
        m_board[7][1] = Piece::WHITE_KNIGHT;
        m_board[7][2] = Piece::WHITE_BISHOP;
        m_board[7][3] = Piece::WHITE_QUEEN;
        m_board[7][4] = Piece::WHITE_KING;
        m_board[7][5] = Piece::WHITE_BISHOP;
        m_board[7][6] = Piece::WHITE_KNIGHT;
        m_board[7][7] = Piece::WHITE_ROOK;
    }

    Board(std::vector<std::vector<Piece>> pieces)
    {
        this->m_board = pieces;
    }

    Board(const Board &b)
    {
        // copy all fields
        for (auto move : b.m_previousMoves)
        {
            m_previousMoves.push_back(move);
        }
        this->m_board = b.m_board;
    }

private:
    void addRookMoves(std::vector<Move> &moves, const PlayerColor &playerToMove, const std::pair<int, int> &position);
    void addBishopMoves(std::vector<Move> &moves, const PlayerColor &playerToMove, const std::pair<int, int> &position);
    void addPawnMoves(std::vector<Move> &moves, const PlayerColor &playerToMove, const std::pair<int, int> &position);
    void addKnightMoves(std::vector<Move> &moves, const PlayerColor &playerToMove, const std::pair<int, int> &position);
    void addKingMoves(std::vector<Move> &moves, const PlayerColor &playerToMove, const std::pair<int, int> &position);
    void getMoves(std::vector<Move> &moves, const PlayerColor playerToMove, std::pair<int, int> position, bool includeKing);

    // consider refactoring to shared data structure for moves under attack 
    // since it's a common and expensive operation
    bool isUnderAttack(PlayerColor playerUnderAttack, std::pair<int,int> square);

    // Piece m_board[8][8]{Piece::EMPTY};
    std::vector<std::vector<Piece>> m_board = std::vector<std::vector<Piece>>(
        8, std::vector<Piece>(8, Piece::EMPTY));
    std::vector<Move> m_previousMoves;
    // instead (in addition to?) of a vector of previous moves, we should use a map with each piece.
    // ordered map !!!
    // ordered_map<Piece,std::vector<Moves>>
    // or just
    // std::vector<Move>[16]
    std::vector<Move> m_availableMoves;
};
