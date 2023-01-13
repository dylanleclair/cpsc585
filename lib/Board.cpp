// # Copyright (c) Dylan Leclair
#include "Board.h"
#include "PlayerColor.h"
#include "Move.h"

#define IN_RANGE(num) (0 <= num && num < 8)
#define IS_ON_BOARD(row, col) (IN_RANGE(row) && IN_RANGE(col))
#define IS_WHITE(row, col) (Piece::WHITE_PAWN <= m_board[row][col] && m_board[row][col] <= Piece::WHITE_KING)
#define IS_EMPTY(row, col) (Piece::EMPTY == m_board[row][col])
#define BREAK_IF_TARGET_COLOR(dest, target) \
    if (dest == target)                     \
        break;

PlayerColor Board::getColor(int row, int col)
{
    if (!IS_ON_BOARD(row,col)) return PlayerColor::None;

    if (!IS_EMPTY(row, col))
    {
        return IS_WHITE(row, col) ? PlayerColor::White : PlayerColor::Black;
    }
    return PlayerColor::None;
}

void Board::addRookMoves(std::vector<Move> &moves, const PlayerColor &playerToMove, const std::pair<int, int> &position)
{
    PlayerColor targetColor = playerToMove == PlayerColor::White ? PlayerColor::Black : PlayerColor::White;

    int row = position.first + 1;
    int col = position.second;

    PlayerColor color = getColor(row, col);
    // check down
    while (IS_ON_BOARD(row,col) && !(color == playerToMove))
    {
        // decrement row
        moves.emplace_back(playerToMove, m_board[position.first][position.second], m_board[row][col], position, row, col);
        BREAK_IF_TARGET_COLOR(color, targetColor);
        row++;
        color = getColor(row, col);
    }
    // check up
    row = position.first - 1;
    color = getColor(row, col);
    while (IS_ON_BOARD(row,col) && !(color == playerToMove))
    {
        moves.emplace_back(playerToMove, m_board[position.first][position.second], m_board[row][col], position, row, col);
        BREAK_IF_TARGET_COLOR(color, targetColor);
        row--;
        color = getColor(row, col);
    }
    row = position.first;
    // col = position.second - 1;
    col = position.second -1;
    color = getColor(row, col);
    // check left
    while (IS_ON_BOARD(row,col) && !(color == playerToMove))
    {
        moves.emplace_back(playerToMove, m_board[position.first][position.second], m_board[row][col], position, row, col);
        BREAK_IF_TARGET_COLOR(color, targetColor);
        col--;
        color = getColor(row, col);
    }
    col = position.second + 1;
    color = getColor(row, col);
    // check right
    while (IS_ON_BOARD(row,col) && !(color == playerToMove))
    {
        // if color at dest is target color, break
        moves.emplace_back(playerToMove, m_board[position.first][position.second], m_board[row][col], position, row, col);
        BREAK_IF_TARGET_COLOR(color, targetColor);
        col++;
        color = getColor(row, col);
    }
}

void Board::addBishopMoves(std::vector<Move> &moves, const PlayerColor &playerToMove, const std::pair<int, int> &position)
{
    int row = position.first - 1;
    int col = position.second + 1;

    PlayerColor targetColor = playerToMove == PlayerColor::White ? PlayerColor::Black : PlayerColor::White;

    PlayerColor color = getColor(row, col);
    // check NE (row decrements, column increments)
    while (IS_ON_BOARD(row, col) && !(color == playerToMove))
    {
        // decrement row
        moves.emplace_back(playerToMove, m_board[position.first][position.second], m_board[row][col], position, row, col);
        BREAK_IF_TARGET_COLOR(color, targetColor);
        row--;
        col++;
        color = getColor(row, col);
    }
    // check NW (row decrements, column decrements)
    row = position.first - 1;
    col = position.second - 1;
    color = getColor(row, col);
    while (IS_ON_BOARD(row, col) && !(color == playerToMove))
    {
        moves.emplace_back(playerToMove, m_board[position.first][position.second], m_board[row][col], position, row, col);
        BREAK_IF_TARGET_COLOR(color, targetColor);
        row--;
        col--;
        color = getColor(row, col);
    }
    row = position.first + 1;
    col = position.second + 1;
    color = getColor(row, col);
    // check SE (row increments, column increments)
    while (IS_ON_BOARD(row, col) && !(color == playerToMove))
    {
        moves.emplace_back(playerToMove, m_board[position.first][position.second], m_board[row][col], position, row, col);
        BREAK_IF_TARGET_COLOR(color, targetColor);
        row++;
        col++;
        color = getColor(row, col);
    }
    row = position.first + 1;
    col = position.second - 1;
    color = getColor(row, col);
    // check SW (row increments, column decrements)
    while (IS_ON_BOARD(row, col) && !(color == playerToMove))
    {
        moves.emplace_back(playerToMove, m_board[position.first][position.second], m_board[row][col], position, row, col);
        BREAK_IF_TARGET_COLOR(color, targetColor);
        row++;
        col--;
        color = getColor(row, col);
    }
}

void Board::addPawnMoves(std::vector<Move> &moves, const PlayerColor &playerToMove, const std::pair<int, int> &position)
{
    /* TODO implement in passing */
    /* TODO implement 2 move if haven't moved yet*/

    int rowOffset = playerToMove == PlayerColor::White ? -1 : 1;

    

    PlayerColor targetColor = playerToMove == PlayerColor::White ? PlayerColor::Black : PlayerColor::White;


    /* home row */
    int row = position.first + (rowOffset * 2); // target row
    if ((position.first == 1 || position.first == 6) && IS_EMPTY(row, position.second) && IS_ON_BOARD(row,position.second))
    {
        moves.emplace_back(playerToMove, m_board[position.first][position.second], m_board[row][position.second], position, row, position.second);
    }

    row = position.first + rowOffset;
    /* moving up/down */
    if (IS_EMPTY(row, position.second) && IS_ON_BOARD(row,position.second))
    {
        moves.emplace_back(playerToMove, m_board[position.first][position.second], m_board[row][position.second], position, row, position.second);
    }



    int col = position.second + 1;
    /* taking pieces */
    if ((getColor(row, col) == targetColor) && IS_ON_BOARD(row, col))
    {
        // push back
        moves.emplace_back(playerToMove, m_board[position.first][position.second], m_board[row][col], position, row, col);
    }
    col = position.second - 1;
    if ((getColor(row, col) == targetColor) && IS_ON_BOARD(row, col))
    {
        moves.emplace_back(playerToMove, m_board[position.first][position.second], m_board[row][col], position, row, col);
    }
}

// offset constants
static const std::vector<std::pair<int, int>> knightOffsets{{2, 1}, {2, -1}, {-2, 1}, {-2, -1}, {1, 2}, {1, -2}, {-1, 2}, {-1, -2}};

void Board::addKnightMoves(std::vector<Move> &moves, const PlayerColor &playerToMove, const std::pair<int, int> &position)
{
    for (auto &x : knightOffsets)
    {
        int row = position.first + x.first;
        int col = position.second + x.second;
        if (IS_ON_BOARD(row, col) && !(getColor(row, col) == playerToMove))
        {
            moves.emplace_back(playerToMove, m_board[position.first][position.second], m_board[row][col], position, row, col);
        }
    }
}

static const std::vector<std::pair<int, int>> kingOffsets{{0, 1}, {0, -1}, {1, 0}, {-1, 0}, {1, 1}, {-1, 1}, {1, -1}, {-1, -1}};

void Board::addKingMoves(std::vector<Move> &moves, const PlayerColor &playerToMove, const std::pair<int, int> &position)
{
    std::vector<Move> options;
    // all around the position, as long as it's not the same color
    for (auto &x : kingOffsets)
    {
        int row = position.first + x.first;
        int col = position.second + x.second;
        if (IS_ON_BOARD(row, col) && !(getColor(row, col) == playerToMove))
        {
            options.emplace_back(playerToMove, m_board[position.first][position.second], m_board[row][col], position, row, col);
        }
    }

    /* king castling */
    Piece king = (playerToMove == PlayerColor::White) ? Piece::WHITE_KING : Piece::BLACK_KING;
    Piece rook = (playerToMove == PlayerColor::White) ? Piece::WHITE_ROOK : Piece::BLACK_ROOK;

    bool isKingsideAllowed{false};
    bool isQueensideAllowed{false};

    int row = (playerToMove == PlayerColor::White) ? 7 : 0 ;

    if ((IS_EMPTY(row,1) && IS_EMPTY(row,2) && IS_EMPTY(row,3)) && (m_board[row][0] == rook))
    {
        isQueensideAllowed = true;
    }

    if (IS_EMPTY(row,5) && IS_EMPTY(row,6) && (m_board[row][7] == rook))
    {
        isKingsideAllowed = true;
    }


    if ((isKingsideAllowed || isQueensideAllowed) && !isInCheck(playerToMove))
    {
        // first and foremost, check if king has moved
        // check if either rook has moved
        // we can assume the position! saves a tiny bit of work lol

        for (const auto& move : m_previousMoves)
        {
            if (move.m_piece == king)
            {
                isKingsideAllowed = false;
                isQueensideAllowed = false;
                break;
            }
            if (move.m_piece == rook)
            {
                if (move.m_start.second == 0)
                {
                    isQueensideAllowed = false;
                }
                if (move.m_start.second == 7)
                {
                    isKingsideAllowed = false;
                }
            }
            
        }
        if (isKingsideAllowed)
        {
            if (!isUnderAttack(playerToMove,{row,5}))
            {
                options.emplace_back(playerToMove, m_board[position.first][position.second], m_board[row][6], position, row, 6, true);
            }
        }

        if (isQueensideAllowed)
        {
            // make sure all the crossing squares are safe
            if (!isUnderAttack(playerToMove,{row,3}))
            {
                options.emplace_back(playerToMove, m_board[position.first][position.second], m_board[row][2], position, row, 2, true);
            }
        }

        // check crossing squares of allowed sides

        // criteria:
        // - neither king nor rook have moved yet
        // - king is not in check
        // - king does not cross over a square attacked by enemy piece // does not end up in check
    }




    /* unfortunately this is rather expensive */
    Board copy = (*this);
    for (auto &move : options)
    {
        copy.move(move);
        if (!copy.isInCheck(playerToMove))
        {
            // the move is legal
            moves.emplace_back(move);
        }
        copy.undo();
    }
}


bool Board::isUnderAttack(PlayerColor playerUnderAttack, std::pair<int,int> square)
{

    PlayerColor targetColor = playerUnderAttack == PlayerColor::White ? PlayerColor::Black : PlayerColor::White;

    std::vector<Move> opponentMoves;

    for (int i =0; IN_RANGE(i); i++)
    {
        for (int j=0; IN_RANGE(i); i++)
        {
            
            if (getColor(i, j) == targetColor) // if opponent's piece
            {
                // add the moves to the list of targetMoves
                getMoves(opponentMoves, targetColor, {i, j}, false); // may need to change flag
            }

        }
    }

    // need to filter results to see if any match the position passed as argument
    // if any of opponents moves can target the king, return true
    for (const auto &move : opponentMoves)
    {
            if (move.m_dest == square)
            {
                return true;
            }            
    }
    return false;
}

void Board::getMoves(std::vector<Move> &moves, const PlayerColor playerToMove, std::pair<int, int> position, bool includeKing)
{
    const Piece piece = m_board[position.first][position.second];
    // there's two ways out of this mess:
    // - encode the color of the piece in the piece enum
    // - switch to polymorphic piece representation?
    if (getColor(position.first, position.second) != playerToMove)
        return;

    switch (piece)
    {
    // all the possible moves for the white pieces
    case (Piece::EMPTY):
        return; // not a piece -> no moves!!!
    // find pawn moves
    case (Piece::BLACK_PAWN):
    case (Piece::WHITE_PAWN):
        // find pawn moves, add to vector
        addPawnMoves(moves, playerToMove, position);
        break;
    // find rook moves
    case (Piece::BLACK_ROOK):
    case (Piece::WHITE_ROOK):
        addRookMoves(moves, playerToMove, position);
        break;
    // find knight moves
    case (Piece::BLACK_KNIGHT):
    case (Piece::WHITE_KNIGHT):
        addKnightMoves(moves, playerToMove, position);
        break;
    // find bishop moves
    case (Piece::BLACK_BISHOP):
    case (Piece::WHITE_BISHOP):
        addBishopMoves(moves, playerToMove, position);
        break;
    // find queen moves
    case (Piece::BLACK_QUEEN):
    case (Piece::WHITE_QUEEN):
        addBishopMoves(moves, playerToMove, position);
        addRookMoves(moves, playerToMove, position);
        break;
    // find king moves
    case (Piece::BLACK_KING):
    case (Piece::WHITE_KING):
        if (includeKing)
            addKingMoves(moves, playerToMove, position);
        break;
    }
}

// how to calculate if a player is in check
// - calculate opponents moves
// - if one of target pieces is the king, the player is in check
// - only moves that result in king no longer being under attack is playable

// every time a move is made, calculate both players possible moves
// when a player selects a piece, just show the subset

// we need some sort of api to simulate making a move
// copy constructor for board (maybe a lighter version of the board class)
// from the copy we can calculate possible moves

void Board::setValidMoves(std::pair<int, int> position)
{
    m_availableMoves.clear();
    if (!IS_ON_BOARD(position.first, position.second))
    {
        std::cout << "Error at setValidMoves: Invalid position."  << std::endl;
        return;
    }
    const Piece piece = m_board[position.first][position.second];
    const PlayerColor l_playerToMove = getPlayerToMove();

    getMoves(m_availableMoves, l_playerToMove, position, true);

    // filters out moves that place player in check
        /* unfortunately this is rather expensive */
    Board copy = (*this);
    for (int i = 0; i < m_availableMoves.size();)
    {
        copy.move(m_availableMoves[i]);
        if (copy.isInCheck(l_playerToMove))
        {
            // the move is legal
            m_availableMoves.erase(std::begin(m_availableMoves) + i);
        } else {
            i++;
        }
        copy.undo();
    }

}

const PlayerColor Board::getPlayerToMove() const
{
    return m_previousMoves.size() % 2 == 0 ? PlayerColor::White : PlayerColor::Black;
}

bool Board::isInCheck(PlayerColor playerToMove)
{
    PlayerColor targetColor = playerToMove == PlayerColor::White ? PlayerColor::Black : PlayerColor::White;

    Piece king = (playerToMove == PlayerColor::White) ? Piece::WHITE_KING : Piece::BLACK_KING;
    Piece opponentKing = (playerToMove == PlayerColor::White) ? Piece::BLACK_KING : Piece::WHITE_KING;

    std::pair<int, int> position{0,0};
    std::vector<Move> opponentMoves;
    // union together all of the moves that belong to target color's pieces
    for (int i = 0; IN_RANGE(i); i++)
    {
        for (int j = 0; IN_RANGE(j); j++)
        {
            if (m_board[i][j] == king)
            {
                position = {i, j};
            }

            if (m_board[i][j] == opponentKing) // restrict king from moving into opponent kings area
            {
                for (auto &offset : kingOffsets)
                {
                    if (IS_ON_BOARD(i + offset.first, j + offset.second))
                        opponentMoves.emplace_back(targetColor, m_board[i][j], m_board[i + offset.first][j + offset.second], position, i + offset.first, j + offset.second);
                }
            }

            if (getColor(i, j) == targetColor) // if opponent's piece
            {
                // add the moves to the list of targetMoves
                getMoves(opponentMoves, targetColor, {i, j}, false); // may need to change flag
            }
        }
    }

    // if any of opponents moves can target the king, return true
    for (auto &move : opponentMoves)
    {
        if (move.m_dest == position)
        {
            return true;
        }
    }
    return false;
}

bool Board::canKingMove(PlayerColor playerToMove)
{

    std::vector<Move> moves;

    Piece king = (playerToMove == PlayerColor::White) ? Piece::WHITE_KING : Piece::BLACK_KING;

    std::pair<int, int> position;
    // union together all of the moves that belong to target color's pieces
    for (int i = 0; IN_RANGE(i); i++)
    {
        for (int j = 0; IN_RANGE(j); j++)
        {
            if (m_board[i][j] == king)
            {
                getMoves(moves, playerToMove, {i, j}, true);
            }
        }
    }

    return (moves.size() == 0) ? false : true;
}

void Board::move(Move move)
{
    std::pair<int, int> dest = move.m_dest;
    std::pair<int, int> start = move.m_start;

    m_board[dest.first][dest.second] = m_board[start.first][start.second];
    m_board[start.first][start.second] = Piece::EMPTY;
    if (move.m_castling)
    {
        // the above move is the king
        // now we move rook

        // check if queen side
        if (move.m_dest.second == 2)
        {
            m_board[move.m_dest.first][3] = m_board[start.first][0];
            m_board[dest.first][0] = Piece::EMPTY;
        }
        // check if king side
        if (move.m_dest.second == 6)
        {
            m_board[dest.first][5] = m_board[start.first][7];
            m_board[start.first][7] = Piece::EMPTY;

        }
    }
    m_previousMoves.push_back(move);
    m_availableMoves.clear();
    deselect();
}

void Board::undo()
{
    if (m_previousMoves.size() == 0)
    {
        return;
    }
    Move move = *(std::prev(std::end(m_previousMoves)));
    std::pair<int, int> dest = move.m_dest;
    std::pair<int, int> start = move.m_start;
    m_board[dest.first][dest.second] = move.m_takes;
    m_board[start.first][start.second] = move.m_piece;
    m_previousMoves.pop_back();
}

bool Board::isCheckmate(PlayerColor winner)
{
    PlayerColor opponentColor = winner == PlayerColor::White ? PlayerColor::Black : PlayerColor::White;
    return (!canKingMove(opponentColor)) && isInCheck(opponentColor);
}

void Board::select(int row, int col) 
{
    if (!IS_ON_BOARD(row,col)) return;

    // set selection data
    m_selection = {{row,col}, true};
    std::cout << "selection: " <<  m_selection.m_selection.first << " " << m_selection.m_selection.second << "exists: " << m_selection.m_exists << std::endl;
    setValidMoves({row,col});
}

void Board::deselect() {
    m_selection = DEFAULT_SELECTION;
    m_availableMoves.clear();
} 