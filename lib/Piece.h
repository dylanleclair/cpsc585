// # Copyright (c) Dylan Leclair
#pragma once

enum Piece
{
    EMPTY,
    WHITE_PAWN,
    WHITE_ROOK,
    WHITE_KNIGHT,
    WHITE_BISHOP,
    WHITE_QUEEN,
    WHITE_KING,
    BLACK_PAWN,
    BLACK_ROOK,
    BLACK_KNIGHT,
    BLACK_BISHOP,
    BLACK_QUEEN,
    BLACK_KING,
    SQUARE_BLACK,
    SQUARE_WHITE
};

static const char getChar(Piece &p)
{
    switch (p)
    {
    // all the possible moves for the white pieces
    // find pawn moves
    case (Piece::BLACK_PAWN):
    case (Piece::WHITE_PAWN):
        // find pawn moves, add to vector
        return 'p';
    // find rook moves
    case (Piece::BLACK_ROOK):
    case (Piece::WHITE_ROOK):
        return 'r';
    // find knight moves
    case (Piece::BLACK_KNIGHT):
    case (Piece::WHITE_KNIGHT):
        return 'l';
    // find bishop moves
    case (Piece::BLACK_BISHOP):
    case (Piece::WHITE_BISHOP):
        return 'b';
        break;
    // find queen moves
    case (Piece::BLACK_QUEEN):
    case (Piece::WHITE_QUEEN):
        return 'q';
        break;
    // find king moves
    case (Piece::BLACK_KING):
    case (Piece::WHITE_KING):
        return 'k';
        break;
    default:
        return ' '; // not a piece -> no moves!!!
    }
}