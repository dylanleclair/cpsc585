// # Copyright (c) Dylan Leclair

#include <iostream>
#include <algorithm>
#include "Board.h"
#include "Move.h"
#include "Piece.h"
#include <raylib.h>
#include <unordered_map>
#include "ecs.h"
#include <random>

#define SCREEN_WIDTH (1280)
#define SCREEN_HEIGHT (720)

#define WINDOW_TITLE "ecs demo"
#define CHESS_ASSETS ASSETS_PATH "chess/512h/"

const uint64_t NUM_ENTITIES = 50000;
struct SampleData {

    SampleData() : origin({5.0f,5.0f}), color(RED), rotation(0.0f) {}
    SampleData(Rectangle r, float rotation, Color color) : origin({ r.x + (r.width / 2), r.y + (r.height / 2) }), rotation(rotation), color(color) {};

    float rotation;
    Vector2 origin;
    Color color;
};

struct PhysicsSystem : ecs::ISystem {

    virtual void Initialize() {}
    virtual void Teardown() {}
    virtual void Update(ecs::Scene &scene, float deltaTime)
    {
        for (Guid entityGuid : ecs::EntitiesInScene<Rectangle, SampleData>(scene))
        {
            SampleData &t = scene.GetComponent<SampleData>(entityGuid);
            Rectangle &r = scene.GetComponent<Rectangle>(entityGuid);
            r.y += 3.0f;
            t.rotation += 8.0f;
        }
    }
};



void RenderingSystem(ecs::Scene& scene, float deltaTime)
{
    for (Guid entityGuid : ecs::EntitiesInScene<Rectangle, SampleData>(scene))
    {
        SampleData& t = scene.GetComponent<SampleData>(entityGuid);
        Rectangle& r = scene.GetComponent<Rectangle>(entityGuid);
        Vector2 origin = Vector2{ 5.0f,5.0f };

        DrawRectanglePro(r, origin, t.rotation, t.color);
    }
}

struct TestItem
{
    Rectangle rect;
    SampleData sd;
};

void updateItems(std::vector<TestItem> &items)
{
    for (auto &item : items)
    {
        SampleData &t = item.sd;
        Rectangle &r = item.rect;

        // t.rotation += 8.0f;
        // r.y += 3.0f;

        item.sd.rotation += 8.0f;
        item.rect.y += 3.0f;

        Vector2 origin = Vector2{5.0f, 5.0f};

        DrawRectanglePro(r, origin, t.rotation, t.color);
    }
}



int main(void)
{

    std::vector<TestItem> objects{};
    
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

    PhysicsSystem physics{};

    ecs::Scene scene;

    std::default_random_engine generator;
    std::uniform_real_distribution<float> randPosition(0.0f, 1200.0f);
    std::uniform_real_distribution<float> randRotation(0.0f, 3.0f);
    std::uniform_real_distribution<float> randGravity(-10.0f, -1.0f);
    std::uniform_int_distribution<int> randColor(0,255);

    for (int i = 0; i < NUM_ENTITIES; i++)
    {

        ecs::Entity e = scene.CreateEntity();

        Rectangle r{ randPosition(generator), randPosition(generator), 10.0f, 10.0f};

        float rotation = randRotation(generator);

        Color color = { static_cast<unsigned char>(randColor(generator)),static_cast<unsigned char>(randColor(generator)),static_cast<unsigned char>(randColor(generator)), 255};
       
        scene.AddComponent(e.guid, r);
        scene.AddComponent(e.guid, SampleData(r, rotation, color));

        objects.push_back({r, SampleData(r, rotation, color)});
    }

    std::cout << "scene initialized\n";
    std::cout << "displaying " << NUM_ENTITIES << "entities.\n";

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

        physics.Update(scene, deltaTime);
        RenderingSystem(scene, deltaTime);

        // updateItems(objects);

        // DrawTextEx(ttf, "Hello world!", Vector2{20.0f, 100.0f}, (float)ttf.baseSize, 2, LIME);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

// float getDeltaTime() {};


// #include "ecs.h"
// int ecs_example()
// {
//     ecs::Scene scene;
//     ecs::Entity &e = scene.CreateEntity();

//     Rectangle rect{0.0f, 0.0f, 10.0f, 10.0f};

//     // add a component -> type is interpreted by compiler
//     scene.AddComponent(e.guid, rect);
//     // explicit typing of component
//     scene.AddComponent<Rectangle>(e.guid, rect);

//     while (true)
//     {
//         RenderingSystem(scene,getDeltaTime());
//     }
// }

// void RenderingSystem(ecs::Scene &scene, float deltaTime)
// {
//     // iterate over entities in scene by GUID, using component type to select components.
//     // only entities with ALL of the specified components are selected.
//     for (Guid entityGuid : ecs::EntitiesInScene<Rectangle, SampleData>(scene))
//     {
//         // get the component on the entity, using GUID to lookup.
//         Rectangle &r = scene.GetComponent<Rectangle>(entityGuid);
//         // sample function that will render the rectangle
//         DrawRectangleRec(r, RED);
//     }
// }
