#include "raylib.h"
#include "game_logic.h"
#include "gui.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h> // Required for malloc and free

// Global variables
Font font;
Texture2D backgroundTexture;

void show_help();

int main(int argc, char *argv[])
{
    if (argc > 1) {
        if (strcmp(argv[1], "--help") == 0) {
            show_help();
            return 0;
        }
    }

    // --- Window Initialization ---
    const int screenWidth = 1280;
    const int screenHeight = 800;
    InitWindow(screenWidth, screenHeight, "Twisted Fables - GUI Edition");

    if (!IsWindowReady()) {
        printf("ERROR: Failed to initialize window.\n");
        return 1;
    }

    font = GetFontDefault();
    backgroundTexture = LoadTexture("background.png");
    
    SetTargetFPS(60);

    // --- [FIX] Allocate Game struct on the heap to prevent stack overflow ---
    Game *game = (Game*)malloc(sizeof(Game));
    if (game == NULL) {
        printf("FATAL ERROR: Failed to allocate memory for the game.\n");
        CloseWindow();
        return 1;
    }
    
    InitGame(game);

    bool should_exit = false; 
    
    // --- Game Loop ---
    while (!should_exit && !WindowShouldClose())
    {
        // 1. Update game state (passing the pointer to the game struct)
        UpdateGame(game, &should_exit);
        
        // 2. Draw everything
        BeginDrawing();
        ClearBackground(DARKGRAY);
        DrawGame(game); // Pass the pointer
        EndDrawing();
    }

    // --- Cleanup ---
    free(game); // Free the allocated memory for the game struct
    UnloadTexture(backgroundTexture);
    CloseWindow();

    return 0;
}


void show_help() {
    printf("\n--- Twisted Fables GUI Edition ---\n\n");
    printf("A 1v1 card battler where the goal is to reduce your opponent's HP to 0.\n\n");
    printf("== HOW TO RUN ==\n");
    printf("  ./TwistedFablesGUI         : (Default) Starts the Player vs. Bot game.\n");
    printf("  ./TwistedFablesGUI --help  : Shows this help message.\n\n");
    printf("== HOW TO PLAY ==\n");
    printf("  1. Select your hero using the mouse.\n");
    printf("  2. On your turn, click cards in your hand to play them.\n");
    printf("  3. Click the 'End Turn' button when you are finished with your actions.\n\n");
    printf("Have fun!\n\n");
}
