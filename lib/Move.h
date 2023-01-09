// # Copyright (c) Dylan Leclair
#pragma once

#include "PlayerColor.h"
#include "Piece.h"
#include <algorithm>

struct Move
{
    Move(PlayerColor color, Piece p, Piece takes, std::pair<int, int> start_pos, int dest_row, int dest_col)
    {
        this->m_player = color;
        this->m_piece = p;
        this->m_start = start_pos;
        this->m_dest = std::pair<int, int>{dest_row, dest_col};
        this->m_takes = takes;
    }

    Move(PlayerColor color, Piece p, Piece takes, std::pair<int, int> start_pos, int dest_row, int dest_col, bool castling)
    {
        this->m_player = color;
        this->m_piece = p;
        this->m_start = start_pos;
        this->m_dest = std::pair<int, int>{dest_row, dest_col};
        this->m_takes = takes;
        this->m_castling = castling;
    }

    PlayerColor m_player;
    Piece m_piece;
    Piece m_takes;
    std::pair<int, int> m_start;
    std::pair<int, int> m_dest;
    bool m_castling{false};
};
