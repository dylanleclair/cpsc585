// # Copyright (c) Dylan Leclair
#pragma once

#include <iostream>
#include <algorithm>
#include "Board.h"
#include "Move.h"
#include "Piece.h"
#include <raylib.h>
#include <unordered_map>
#include "ecs.h"




#define SCREEN_WIDTH (1800)
#define SCREEN_HEIGHT (1200)

#define WINDOW_TITLE " [caskaydia] chess"
#define CHESS_ASSETS ASSETS_PATH "chess/512h/"

// draws a texture, scaling it to fit a rect of desired size
void drawTextureRect(Texture t, Rectangle dest, Color tint)
{
    // grab full texture (maybe use atlas eventually)
    Rectangle src;
    src.x = 0.0f;
    src.y = 0.0f;
    src.width = (float)t.width;
    src.height = (float)t.height;

    // origin for scaling / rotation
    // Vector2 origin = {dest.width / 2, dest.height / 2};
    Vector2 origin = {0, 0};
    DrawTexturePro(t, src, dest, origin, 0, tint);
}

// Wrapper class for board that manages drawing it
class BoardGUI
{
public:
    BoardGUI(int x, int y, int size)
    {
        this->m_x = x;
        this->m_y = y;
        this->m_size = size;
        this->m_board = Board{};
        this->m_cellSize = size / 8;

        // white pieces
        m_textures[Piece::WHITE_PAWN] = LoadTexture(CHESS_ASSETS "w_pawn_png_512px.png");
        m_textures[Piece::WHITE_ROOK] = LoadTexture(CHESS_ASSETS "w_rook_png_512px.png");
        m_textures[Piece::WHITE_KNIGHT] = LoadTexture(CHESS_ASSETS "w_knight_png_512px.png");
        m_textures[Piece::WHITE_BISHOP] = LoadTexture(CHESS_ASSETS "w_bishop_png_512px.png");
        m_textures[Piece::WHITE_QUEEN] = LoadTexture(CHESS_ASSETS "w_queen_png_512px.png");
        m_textures[Piece::WHITE_KING] = LoadTexture(CHESS_ASSETS "w_king_png_512px.png");

        // black pieces
        m_textures[Piece::BLACK_PAWN] = LoadTexture(CHESS_ASSETS "b_pawn_png_512px.png");
        m_textures[Piece::BLACK_ROOK] = LoadTexture(CHESS_ASSETS "b_rook_png_512px.png");
        m_textures[Piece::BLACK_KNIGHT] = LoadTexture(CHESS_ASSETS "b_knight_png_512px.png");
        m_textures[Piece::BLACK_BISHOP] = LoadTexture(CHESS_ASSETS "b_bishop_png_512px.png");
        m_textures[Piece::BLACK_QUEEN] = LoadTexture(CHESS_ASSETS "b_queen_png_512px.png");
        m_textures[Piece::BLACK_KING] = LoadTexture(CHESS_ASSETS "b_king_png_512px.png");
    }

    void select(float x, float y)
    {

        


        int row = static_cast<int>(std::floor(y / m_cellSize));
        int col = static_cast<int>(std::floor(x / m_cellSize));
        
        // 
        std::pair<int,int> selection = m_board.m_selection.m_selection;
        


        std::vector<Move> availableMoves = m_board.getValidMoves();
        std::cout << availableMoves.size();
        for (int i = 0 ; i < availableMoves.size(); i++)
        {
            if (availableMoves[i].m_dest == std::pair<int,int>{row,col})
            {
                // make the move !
                m_board.move(availableMoves[i]);
                std::cout << "lul";
                return;
            }
        }
        
        // if move not in selected && does not match the player's color, deselect & exit
        if (m_board.getColor(row,col) != m_board.getPlayerToMove())
        {
            // if move not in available moves
            m_board.deselect();
            return;
        }

        std::pair<int,int> currentSelection = m_board.m_selection.m_selection;
        if (m_board.m_selection.m_exists && (currentSelection == std::pair<int,int>{row,col} || m_board.getBoard()[currentSelection.first][currentSelection.second] == Piece::EMPTY)) m_board.deselect();
        else {
            m_board.select(row,col);
        }
    }

    // draws the board at current position and size
    void draw()
    {
        // draws the game board
        int cell_size = m_size / 8;
        int fill = ((cell_size * 8) % m_size) / 2;

        // starting pos is gucci
        // now we just draw ze assets :D

        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                int xOff = m_x + (j * cell_size);
                int yOff = m_y + (i * cell_size);
                // draw the board

                bool fill = (j % 2) == 0 ? true : false;
                if (i % 2 == 0)
                    fill = !fill;
                if (fill)
                    DrawRectangleRec({(float)xOff, (float)yOff, (float)cell_size, (float)cell_size}, GetColor(0x3a9163FF));


                // draw the pieces (tinting selected piece if one exists)
                if (m_board.m_selection.m_exists && m_board.m_selection.m_selection == std::pair<int, int>{i, j})
                {
                    // tint the selected piece
                    drawTextureRect(m_textures[m_board.getBoard()[i][j]], {(float)xOff + (cell_size * 0.1f), (float)yOff + (cell_size * 0.1f), (float)cell_size * 0.8f, (float)cell_size * 0.8f}, {255, 255, 255, 125});
                }
                else
                {
                    // draw the piece on the board
                    drawTextureRect(m_textures[m_board.getBoard()[i][j]], {(float)xOff + (cell_size * 0.1f), (float)yOff + (cell_size * 0.1f), (float)cell_size * 0.8f, (float)cell_size * 0.8f}, WHITE);
                }
            }
        }

        const std::vector<Move>& moves = m_board.getValidMoves();
        for (auto& move : moves)
        {
            int xOff = m_x + (move.m_dest.second * cell_size);
            int yOff = m_y + (move.m_dest.first * cell_size);
            // highlight any potential movement squares
            // DrawRectangleRec({(float)xOff + (cell_size * 0.25f), (float)yOff + (cell_size * 0.25f), (float)cell_size * 0.5f, (float)cell_size * 0.5f}, {0,0,0,60});
            DrawCircle(xOff + static_cast<int>(cell_size * 0.5f), yOff + static_cast<int>(cell_size * 0.5f),cell_size * 0.2f, {0,0,0,60});
        }

    }

    void setSize(int size)
    {
        this->m_size = size;
    }

private:
    Board m_board;
    int m_x;
    int m_y;
    int m_size;
    int m_cellSize;

    std::unordered_map<int, Texture2D> m_textures;
};



struct DummyData {
    int hello0{0};
    int hello1{1};
};

struct DummyDataAlternate {
    int why{1};
    int oh{0};
};

int main(void)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    // SetConfigFlags(FLAG_FULLSCREEN_MODE);
    //  Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SetWindowIcon(LoadImage(CHESS_ASSETS "b_king_png_512px.png"));
    SetTargetFPS(144);

    std::unordered_map<int, Texture2D> chessTextures;
    chessTextures[Piece::SQUARE_BLACK] = LoadTexture(CHESS_ASSETS "dark_png_512px.png");
    chessTextures[Piece::SQUARE_WHITE] = LoadTexture(CHESS_ASSETS "light_png_512px.png");

    // white pieces
    chessTextures[Piece::WHITE_PAWN] = LoadTexture(CHESS_ASSETS "w_pawn_png_512px.png");
    chessTextures[Piece::WHITE_ROOK] = LoadTexture(CHESS_ASSETS "w_rook_png_512px.png");
    chessTextures[Piece::WHITE_KNIGHT] = LoadTexture(CHESS_ASSETS "w_knight_png_512px.png");
    chessTextures[Piece::WHITE_BISHOP] = LoadTexture(CHESS_ASSETS "w_bishop_png_512px.png");
    chessTextures[Piece::WHITE_QUEEN] = LoadTexture(CHESS_ASSETS "w_queen_png_512px.png");
    chessTextures[Piece::WHITE_KING] = LoadTexture(CHESS_ASSETS "w_king_png_512px.png");

    // black pieces
    chessTextures[Piece::BLACK_PAWN] = LoadTexture(CHESS_ASSETS "b_pawn_png_512px.png");
    chessTextures[Piece::BLACK_ROOK] = LoadTexture(CHESS_ASSETS "b_rook_png_512px.png");
    chessTextures[Piece::BLACK_KNIGHT] = LoadTexture(CHESS_ASSETS "b_knight_png_512px.png");
    chessTextures[Piece::BLACK_BISHOP] = LoadTexture(CHESS_ASSETS "b_bishop_png_512px.png");
    chessTextures[Piece::BLACK_QUEEN] = LoadTexture(CHESS_ASSETS "b_queen_png_512px.png");
    chessTextures[Piece::BLACK_KING] = LoadTexture(CHESS_ASSETS "b_king_png_512px.png");

    Font ttf = LoadFontEx(ASSETS_PATH "JetBrainsMono.ttf", 50, 0, 250);

    Vector2 mousePos;
    int screenWidth{SCREEN_WIDTH};
    int screenHeight{SCREEN_HEIGHT};

    BoardGUI board{0, 0, screenHeight};
    //--------------------------------------------------------------------------------------
    // Main game loop
    while (!WindowShouldClose())
    {

        BeginDrawing();

        ClearBackground(RAYWHITE);

        // add bars to edges of window to fit with aspect ratio?
        { // window resizing
            int height = GetScreenHeight();
            int width = GetScreenWidth();

            if (screenWidth != width || screenHeight != height)
            {
                screenWidth = width;
                screenHeight = height;

                int unit_size{0};
                if (width > height)
                {
                    unit_size = width / 3;
                }
                else
                {
                    unit_size = height / 2;
                }

                screenWidth = 3 * unit_size;
                screenHeight = 2 * unit_size;
                SetWindowSize(screenWidth, screenHeight);
                board.setSize(screenHeight);
            }
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            mousePos = GetMousePosition();
            board.select(mousePos.x,mousePos.y);
        }

        // draw the board
        board.draw();


        std::cout << "new component id: " << ecs::GetComponentGuid<DummyData>();
        std::cout << "new component id: " << ecs::GetComponentGuid<DummyDataAlternate>();
        std::cout << "new component id: " << ecs::GetComponentGuid<DummyData>();

        // DrawTextEx(ttf, "Hello world!", Vector2{20.0f, 100.0f}, (float)ttf.baseSize, 2, LIME);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
