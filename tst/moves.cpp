#include "gtest/gtest.h"
#include "Board.h"
#include "Piece.h"
#include <vector>

TEST(moves, queen) // also tests rook / bishop LOL
{

    std::vector<std::pair<int, int>> expected;

    std::vector<std::vector<Piece>> pieces =
        {
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},

            {Piece::BLACK_PAWN, Piece::BLACK_PAWN, Piece::BLACK_PAWN, Piece::BLACK_PAWN, Piece::BLACK_PAWN, Piece::BLACK_PAWN, Piece::BLACK_PAWN, Piece::BLACK_PAWN},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::WHITE_ROOK, Piece::WHITE_KNIGHT, Piece::WHITE_BISHOP, Piece::WHITE_QUEEN, Piece::WHITE_KING, Piece::WHITE_BISHOP, Piece::WHITE_KNIGHT, Piece::WHITE_ROOK},
        };

    // straight up
    expected.emplace_back(4, 3); // row 4, col 3
    expected.emplace_back(5, 3); // row 5, col 3
    expected.emplace_back(6, 3); // row 6, col 3

    // diagonal left
    expected.emplace_back(6, 2);
    expected.emplace_back(5, 1);
    expected.emplace_back(4, 0);
    // diagonal right
    expected.emplace_back(6, 4);
    expected.emplace_back(5, 5);
    expected.emplace_back(4, 6);

    Board b{pieces};

    b.setValidMoves(std::pair<int, int>{7, 3});
    const std::vector<Move> &moves = b.getValidMoves();

    for (auto &move : moves)
    {
        auto it = std::find(std::begin(expected), std::end(expected), move.m_dest);
        ASSERT_TRUE(it != std::end(expected));
    }

    // unexpected
    std::vector<std::pair<int, int>> unexpected;
    unexpected.push_back({3, 3}); // past row of pawns straight
    unexpected.push_back({3, 7}); // past row of pawns diagonally
    unexpected.push_back({7, 4}); // allied right
    unexpected.push_back({7, 2}); // allied left

    for (auto &move : moves)
    {
        auto it = std::find(std::begin(unexpected), std::end(unexpected), move.m_dest);
        ASSERT_TRUE(it == std::end(unexpected));
    }
}

TEST(moves, in_check)
{

    std::vector<std::vector<Piece>> pieces =
        {
            {Piece::BLACK_ROOK, Piece::EMPTY, Piece::WHITE_KING, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},

            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::BLACK_KING, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
        };

    Board b{pieces};
    ASSERT_TRUE(b.isInCheck(PlayerColor::White));
}

TEST(moves, not_in_check)
{

    std::vector<std::vector<Piece>> pieces =
        {
            {Piece::BLACK_ROOK, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::WHITE_KING, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},

            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
        };

    Board b{pieces};
    ASSERT_TRUE(!b.isInCheck(PlayerColor::White));
}

TEST(moves, can_king_move)
{
    // can the king move
    std::vector<std::vector<Piece>> pieces =
        {
            {Piece::BLACK_ROOK, Piece::EMPTY, Piece::BLACK_ROOK, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::WHITE_KING, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::BLACK_ROOK, Piece::EMPTY, Piece::BLACK_ROOK, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},

            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
        };
    Board b{pieces};
    ASSERT_TRUE(!b.canKingMove(PlayerColor::White));
}

// the problem is it can't know it's pinned...
// we need logic to simulate each move rip

TEST(moves, can_king_move_true)
{
    // can the king move
    std::vector<std::vector<Piece>> pieces =
        {
            {Piece::EMPTY, Piece::BLACK_ROOK, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::WHITE_KING, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},

            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::BLACK_ROOK, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
        };
    Board b{pieces};
    ASSERT_TRUE(b.canKingMove(PlayerColor::White));
}

TEST(moves, checkmate)
{
    // can the king move
    std::vector<std::vector<Piece>> pieces =
        {
            {Piece::BLACK_ROOK, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::BLACK_ROOK, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},

            {Piece::WHITE_KING, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
        };
    Board b{pieces};
    ASSERT_TRUE(b.isCheckmate(PlayerColor::Black));
}

TEST(moves, king_moves_next_to_king)
{
    // make sure that it's impossible for a king to move next to another king
    // use results to determine if flag still needed for king

    // can the king move
    std::vector<std::vector<Piece>> pieces =
        {
            {Piece::BLACK_KING, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::WHITE_KING, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},

            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
        };

    std::vector<std::pair<int, int>> expected;

    // straight up
    expected.emplace_back(2, 1); // right
    expected.emplace_back(3, 0); // down
    expected.emplace_back(3, 1); // down right

    Board b{pieces};

    b.setValidMoves(std::pair<int, int>{2, 0});
    const std::vector<Move> &moves = b.getValidMoves();

    for (auto &move : moves)
    {
        auto it = std::find(std::begin(expected), std::end(expected), move.m_dest);
        ASSERT_TRUE(it != std::end(expected));
    }

    // unexpected
    std::vector<std::pair<int, int>> unexpected;
    unexpected.push_back({1, 0}); // up
    unexpected.push_back({1, 1}); // up right

    for (auto &move : moves)
    {
        auto it = std::find(std::begin(unexpected), std::end(unexpected), move.m_dest);
        ASSERT_TRUE(it == std::end(unexpected));
    }
}

TEST(moves, king_castles_queenside)
{
    // test if the king can castle queenside
    std::vector<std::vector<Piece>> pieces =
        {
            {Piece::BLACK_KING, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::WHITE_KING, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},

            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
            {Piece::WHITE_ROOK, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY, Piece::WHITE_KING, Piece::EMPTY, Piece::EMPTY, Piece::EMPTY},
        };

    Board b{pieces};

    b.setValidMoves(std::pair<int, int>{7, 4});
    const std::vector<Move> &moves = b.getValidMoves();

    bool targetFound = false;
    // find the move !!
    for (auto it = moves.begin(); it != moves.end(); it++)
    {
        if (it->m_dest == std::pair<int, int>{7, 2}) // queenside castle
        {
            targetFound = true;
            b.move(*it);
            break; // idk what's causing this to be needed lol
        }
    }
    ASSERT_TRUE(targetFound);
    // assert that rook also moves
    ASSERT_TRUE(b.getBoard()[7][3] == Piece::WHITE_ROOK);
}