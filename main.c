#include "raylib.h"
#include "game_logic.h"
#include "gui.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h> // Required for malloc and free
#include "definitions.h" 
#include <time.h>  // 為了 srand 與 rand

#define OTHER_BGM_COUNT 5
Music otherBGMs[OTHER_BGM_COUNT];
int currentOtherBGMIndex = -1;

typedef enum {
    MODE_PVB, 
    MODE_PVP
} AppMode;


// Global variables
Font font;
Texture2D backgroundTexture;
Texture2D character_images[10];
Music dorothyBGM;
Texture2D screamImages[3];
Texture2D scream_images[3];
void show_help();

int main(int argc, char *argv[]) {
    AppMode selected_mode = MODE_PVB; 

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            show_help();
            return 0;
        }
        if (strcmp(argv[i], "--pvp") == 0) {
            selected_mode = MODE_PVP;
        }
        if (strcmp(argv[i], "--pvb") == 0) {
            selected_mode = MODE_PVB;
        }
    }

    const int screenWidth = 1280;
    const int screenHeight = 800;
    InitWindow(screenWidth, screenHeight, "Twisted Fables - GUI Edition");

    InitAudioDevice(); // 初始化音訊設備
    dorothyBGM = LoadMusicStream("assets/njhsq-7qkal.ogg"); // 改成你的檔案路徑
    otherBGMs[0] = LoadMusicStream("assets/ke5yr-inyej.ogg");
    otherBGMs[1] = LoadMusicStream("assets/kj2rz-39yf5.ogg");
    otherBGMs[2] = LoadMusicStream("assets/s3a3k-0acm5.ogg");
    otherBGMs[3] = LoadMusicStream("assets/azan5-7pbco.ogg");
    otherBGMs[4] = LoadMusicStream("assets/xv3pl-okfys.ogg");

    // 初始化亂數種子
    srand(time(NULL));



    if (!IsWindowReady()) {
        printf("ERROR: Failed to initialize window.\n");
        return 1;
    }

    // Load resources (font, textures, etc.)
    int total = 20902 + 26 + 26 + 10 + 8 + 17;
    int *codepoints = malloc(sizeof(int) * total);
    int idx = 0;
    for (int i = 0x41; i <= 0x5A; ++i) codepoints[idx++] = i;
    for (int i = 0x61; i <= 0x7A; ++i) codepoints[idx++] = i;
    for (int i = 0x30; i <= 0x39; ++i) codepoints[idx++] = i;
    codepoints[idx++] = 0x20; codepoints[idx++] = 0x3A; codepoints[idx++] = 0x2E; codepoints[idx++] = 0x21;
    codepoints[idx++] = 0x2B; codepoints[idx++] = 0x2D; codepoints[idx++] = 0x5B; codepoints[idx++] = 0x5D;
    codepoints[idx++] = 0x28; codepoints[idx++] = 0x29; codepoints[idx++] = 0xFF0C; codepoints[idx++] = 0x2F;
    codepoints[idx++] = 0x22; codepoints[idx++] = 0x3C; codepoints[idx++] = 0x3E; codepoints[idx++] = 0x2C;
    codepoints[idx++] = 0x2264; codepoints[idx++] = 0x2265; codepoints[idx++] = 0x3D; codepoints[idx++] = 0x2264;
    codepoints[idx++] = 0x2265; codepoints[idx++] = 0x2192; codepoints[idx++] = 0x3001; codepoints[idx++] = 0x3F;
    codepoints[idx++] = 0xFF1F;
    for (int i = 0; i < 20902; ++i) codepoints[idx++] = 0x4E00 + i;
    font = LoadFontEx("assets/fonts/Cubic_11.ttf", 32, codepoints, total);
    SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);
    free(codepoints);

    backgroundTexture = LoadTexture("background.png");


    screamImages[0] = LoadTexture("assets/scream.png");
    screamImages[1] = LoadTexture("assets/scream1.png");
    screamImages[2] = LoadTexture("assets/scream2.png");
    scream_images[0] = LoadTexture("assets/scream1.png");
    scream_images[1] = LoadTexture("assets/scream2.png");
    scream_images[2] = LoadTexture("assets/scream3.png");

    character_images[0] = LoadTexture("assets/redhood.png");
    character_images[1] = LoadTexture("assets/snowwhite.png");
    character_images[2] = LoadTexture("assets/sleeping.png");
    character_images[3] = LoadTexture("assets/alice.png");
    character_images[4] = LoadTexture("assets/mulan.png");
    character_images[5] = LoadTexture("assets/kaguya.png");
    character_images[6] = LoadTexture("assets/mermaid.png");
    character_images[7] = LoadTexture("assets/matchgirl.png");
    character_images[8] = LoadTexture("assets/dorothy.png");
    character_images[9] = LoadTexture("assets/scheherazade.png");

    SetTargetFPS(60);

    Game *game = malloc(sizeof(Game));
    if (game == NULL) {
        printf("FATAL ERROR: Failed to allocate memory for the game.\n");
        CloseWindow();
        return 1;
    }
    

    InitGame(game);

    if (selected_mode == MODE_PVP) {
        game->current_state = GAME_STATE_PVP_CHOOSE_CHAR_P1;
    }

    bool should_exit = false;

    while (!should_exit && !WindowShouldClose()) {
        if (selected_mode == MODE_PVB) {
            UpdateGame(game, &should_exit);

            BeginDrawing();
            ClearBackground(DARKGRAY);
            DrawGame(game, character_images);
            EndDrawing();

            CharacterType ch = game->inner_game.players[0].character;

            if (game->current_state == GAME_STATE_CHOOSE_CHAR) {
                if (IsMusicStreamPlaying(dorothyBGM)) StopMusicStream(dorothyBGM);
                if (currentOtherBGMIndex != -1 && IsMusicStreamPlaying(otherBGMs[currentOtherBGMIndex])) {
                    StopMusicStream(otherBGMs[currentOtherBGMIndex]);
                    currentOtherBGMIndex = -1;
                }
            } else if (ch == DOROTHY) {
                if (!IsMusicStreamPlaying(dorothyBGM)) {
                    PlayMusicStream(dorothyBGM);
                }
                UpdateMusicStream(dorothyBGM);
            } else {
                if (currentOtherBGMIndex == -1 || !IsMusicStreamPlaying(otherBGMs[currentOtherBGMIndex])) {
                    currentOtherBGMIndex = rand() % OTHER_BGM_COUNT;
                    PlayMusicStream(otherBGMs[currentOtherBGMIndex]);
                }
                UpdateMusicStream(otherBGMs[currentOtherBGMIndex]);
            }

            UpdateMusicStream(dorothyBGM);
        } else {
            UpdatePVPGame(game, &should_exit);

            BeginDrawing();
            ClearBackground(DARKGRAY);
            DrawGame(game, character_images);
            EndDrawing();
        }
    }
        
    // --- [修正] 正確的清理邏輯 ---
    // 無論是 PVP 還是 PVB，遊戲結束時都需要釋放資源
    // 首先清理基礎商店牌庫
    for (int p_idx = 0; p_idx < 2; ++p_idx) { // 遍歷兩位玩家的商店
        for (int type = 0; type < 3; ++type) {
            for (int lvl = 0; lvl < 3; ++lvl) {
                freeVector(&game->shop_piles[p_idx][type][lvl]);
            }
        }
    }

    // 接著清理技能商店牌庫
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 3; ++j) {
            freeVector(&game->shop_skill_piles[i][j]);
        }
    }
    // ------------------------------------

    free(game); 
    UnloadTexture(backgroundTexture);
    for(int i = 0; i < 10; i++) {
        UnloadTexture(character_images[i]);
    }
    UnloadFont(font);
    CloseWindow();
    
    return 0;
}


void show_help() {
    printf("\n--- Twisted Fables GUI Edition ---\n\n");
    printf("A 1v1 card battler where the goal is to reduce your opponent's HP to 0.\n\n");
    printf("== HOW TO RUN ==\n");
    printf("  ./TwistedFablesGUI         : (Default) Starts the Player vs. Bot game.\n");
    printf("  ./TwistedFablesGUI --pvb   : Starts the Player vs. Bot game.\n");
    printf("  ./TwistedFablesGUI --pvp   : Starts the Player vs. Player mode.\n");
    printf("  ./TwistedFablesGUI --help  : Shows this help message.\n\n");
    printf("== HOW TO PLAY ==\n");
    printf("  1. Select your hero using the mouse.\n");
    printf("  2. On your turn, click cards in your hand to play them.\n");
    printf("  3. Click the 'End Turn' button when you are finished with your actions.\n\n");
    printf("Have fun!\n\n");
}
