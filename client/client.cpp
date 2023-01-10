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
#include <random>




#define SCREEN_WIDTH (1800)
#define SCREEN_HEIGHT (1200)

#define WINDOW_TITLE "ecs demo"
#define CHESS_ASSETS ASSETS_PATH "chess/512h/"

struct SampleData {

    SampleData(Rectangle r, float rotation, Color color) : origin({ r.x + (r.width / 2), r.y + (r.height / 2) }), rotation(rotation), color(color) {};

    float rotation;
    Vector2 origin;
    Color color;
};


void PhysicsSystem(ecs::Scene& scene, float deltaTime)
{
    for (Guid entityGuid : ecs::EntitiesInScene<Rectangle, SampleData> (scene))
    {
        SampleData& t = scene.GetComponent<SampleData>(entityGuid);
        Rectangle& r = scene.GetComponent<Rectangle>(entityGuid);
        t.rotation += 8.0f;
        r.y += 3.0f;
    }
}

void Renderer(ecs::Scene& scene, float deltaTime)
{
    for (Guid entityGuid : ecs::EntitiesInScene<Rectangle, SampleData>(scene))
    {
        SampleData& t = scene.GetComponent<SampleData>(entityGuid);
        Rectangle& r = scene.GetComponent<Rectangle>(entityGuid);

        Vector2 origin = Vector2{ 5.0f,5.0f };

        DrawRectanglePro(r, origin, t.rotation, t.color);
    }
}

int main(void)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    // SetConfigFlags(FLAG_FULLSCREEN_MODE);
    //  Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SetWindowIcon(LoadImage(CHESS_ASSETS "b_king_png_512px.png"));
    SetTargetFPS(60);

    Font ttf = LoadFontEx(ASSETS_PATH "JetBrainsMono.ttf", 50, 0, 250);

    int screenWidth{SCREEN_WIDTH};
    int screenHeight{SCREEN_HEIGHT};


    ecs::Scene scene;


    std::default_random_engine generator;
    std::uniform_real_distribution<float> randPosition(0.0f, 1200.0f);
    std::uniform_real_distribution<float> randRotation(0.0f, 3.0f);
    std::uniform_real_distribution<float> randGravity(-10.0f, -1.0f);
    std::uniform_int_distribution<int> randColor(0,255);

    for (int i = 0; i < 1000; i++)
    {

        ecs::Entity& e = scene.CreateEntity();

        Rectangle r{ randPosition(generator), randPosition(generator), 10.0f, 10.0f};

        float rotation = randRotation(generator);

        Color color = { static_cast<unsigned char>(randColor(generator)),static_cast<unsigned char>(randColor(generator)),static_cast<unsigned char>(randColor(generator)), 255};
       
        scene.AddComponent(e.id, r);
        scene.AddComponent(e.id, SampleData(r, rotation, color));
    }

    std::cout << "scene initialized\n";

    //--------------------------------------------------------------------------------------
    // Main game loop
    while (!WindowShouldClose())
    {

        float deltaTime = GetFrameTime();

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

                int unit_size{ 0 };
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
            }
        }

        PhysicsSystem(scene, deltaTime);
        Renderer(scene, deltaTime);

        // DrawTextEx(ttf, "Hello world!", Vector2{20.0f, 100.0f}, (float)ttf.baseSize, 2, LIME);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
