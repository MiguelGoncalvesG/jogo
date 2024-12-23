#include "raylib.h"
#define ScreenWidth 1200
#define ScreenHeight 600



int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(ScreenWidth, ScreenHeight, "INF MAN - Menu");
    InitAudioDevice();
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    //load
    Texture2D menu = LoadTexture ("bin/Debug/resources/imagem_menu.png");
    Sound menusound = LoadSound ("bin/Debug/resources/menusound.wav");
    //Sound gamesound = LoadSound ("bin/Debug/resources/gamesound.wav");

    //
    //Definindo a cor do background
    Color backgroundColor = (Color)
    {
        53, 173, 208, 255
    };
    //--------------------------------------------------------------------------------------

    Rectangle ranking = {ScreenWidth/2 - 100, ScreenHeight/2 + 70, 200, 40};
    Rectangle sair = {ScreenWidth/2 - 100, ScreenHeight/2 +20, 200, 40};
    Rectangle iniciar = {ScreenWidth/2 - 100, ScreenHeight/2 - 30, 200, 40};

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        if (!IsSoundPlaying(menusound))
        {
            PlaySound(menusound);
        }

        BeginDrawing();

        ClearBackground(backgroundColor);
        //Desenhandoi botoes do menu
        DrawRectangleRec(ranking, CheckCollisionPointRec(GetMousePosition(), ranking) ? DARKGRAY : GRAY);
        DrawRectangleRec(sair, CheckCollisionPointRec(GetMousePosition(), sair) ? DARKGRAY : GRAY);
        DrawRectangleRec(iniciar, CheckCollisionPointRec(GetMousePosition(), iniciar) ? DARKGRAY : GRAY);

        DrawText("Jogar", iniciar.x + (iniciar.width / 2 - MeasureText("Jogar", 20) / 2), iniciar.y + 10, 20, WHITE);
        DrawText("sair", sair.x + (iniciar.width / 2 - MeasureText("sair", 20) / 2), sair.y + 10, 20, WHITE);
        DrawText("ranking", ranking.x + (iniciar.width / 2 - MeasureText("ranking", 20) / 2), ranking.y + 10, 20, WHITE);

        DrawText("INF MAN", ScreenWidth / 2 - MeasureText("INF MAN", 50) / 2, ScreenHeight / 5, 50, YELLOW);

        //
        DrawTexture(menu, ScreenWidth/15, ScreenHeight/4, WHITE);

        EndDrawing();

    }

    //unload
    UnloadSound(menusound);
    UnloadTexture (menu);
    CloseAudioDevice();
    CloseWindow();


    return 0;
}

