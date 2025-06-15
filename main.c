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

// Global variables
Font font;
Texture2D backgroundTexture;
Texture2D character_images[10];
Music dorothyBGM;


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

    // ✅ 在這裡載入中文點陣字型（建議放在 assets/fonts）
    // 分配足夠空間來存放中英文 + 數字 + 標點（20902 + 26*2 + 10 + 標點）
    int total = 20902 + 26 + 26 + 10 + 8 + 17;
    int *codepoints = malloc(sizeof(int) * total);
    int idx = 0;

    // 英文大寫 A-Z
    for (int i = 0x41; i <= 0x5A; ++i) codepoints[idx++] = i;

    // 英文小寫 a-z
    for (int i = 0x61; i <= 0x7A; ++i) codepoints[idx++] = i;

    // 數字 0-9
    for (int i = 0x30; i <= 0x39; ++i) codepoints[idx++] = i;

    // 基本標點（空格、冒號、句號、驚嘆號）
    codepoints[idx++] = 0x20; // space
    codepoints[idx++] = 0x3A; // :
    codepoints[idx++] = 0x2E; // .
    codepoints[idx++] = 0x21; // !
    codepoints[idx++] = 0x2B; // +
    codepoints[idx++] = 0x2D; // -
    codepoints[idx++] = 0x5B; // [
    codepoints[idx++] = 0x5D; // ]
    codepoints[idx++] = 0x28; // (
    codepoints[idx++] = 0x29; // )
    codepoints[idx++] = 0xFF0C; // ，
    codepoints[idx++] = 0x2F; // / 
    codepoints[idx++] = 0x22; // "
    codepoints[idx++] = 0x3C; // <
    codepoints[idx++] = 0x3E; // >
    codepoints[idx++] = 0x2C; // , 半形逗號 ✅
    codepoints[idx++] = 0x2264; // ≤ 小於等於符號
    codepoints[idx++] = 0x2265; // ≥ 大於等於符號
    codepoints[idx++] = 0x3D; // = 等號 ✅
    codepoints[idx++] = 0x2264; // ≤
    codepoints[idx++] = 0x2265; // ≥
    codepoints[idx++] = 0x2192; // → 右箭頭符號
    codepoints[idx++] = 0x3001; // 、頓號 ✅
    codepoints[idx++] = 0x3F;   // 半形問號 ?
    codepoints[idx++] = 0xFF1F; // 全形問號 ？

    // 中文區段：0x4E00 ~ 0x9FFF
    for (int i = 0; i < 20902; ++i)
        codepoints[idx++] = 0x4E00 + i;

    // ✅ 載入字體
    font = LoadFontEx("assets/fonts/Cubic_11.ttf", 32, codepoints, total);
    SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);

    // ❗完成後釋放記憶體（避免記憶體洩漏）
    free(codepoints);


    //font = LoadFont("assets/fonts/Cubic_11.ttf");
    //font = LoadFont("assets/fonts/NotoSansTC-Black.ttf");
    //font = GetFontDefault();
    backgroundTexture = LoadTexture("background.png");

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

    // --- [FIX] Allocate Game struct on the heap to prevent stack smashing ---
    Game *game = malloc(sizeof(Game));
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
        UpdateGame(game, &should_exit);

        BeginDrawing();
        ClearBackground(DARKGRAY);
        DrawGame(game, character_images);
        EndDrawing();

        CharacterType ch = game->inner_game.players[0].character;

        // 若回到選角畫面，停止所有 BGM
        if (game->current_state == GAME_STATE_CHOOSE_CHAR) {
            if (IsMusicStreamPlaying(dorothyBGM)) StopMusicStream(dorothyBGM);
            if (currentOtherBGMIndex != -1 && IsMusicStreamPlaying(otherBGMs[currentOtherBGMIndex])) {
                StopMusicStream(otherBGMs[currentOtherBGMIndex]);
                currentOtherBGMIndex = -1;
            }
        }
        // 桃樂絲播放專屬 BGM
        else if (ch == DOROTHY) {
            if (!IsMusicStreamPlaying(dorothyBGM)) {
                PlayMusicStream(dorothyBGM);
            }
            UpdateMusicStream(dorothyBGM);
        }
        // 其他角色隨機播放 one of 5 BGM
        else {
            if (currentOtherBGMIndex == -1 || !IsMusicStreamPlaying(otherBGMs[currentOtherBGMIndex])) {
                currentOtherBGMIndex = rand() % OTHER_BGM_COUNT;
                PlayMusicStream(otherBGMs[currentOtherBGMIndex]);
            }
            UpdateMusicStream(otherBGMs[currentOtherBGMIndex]);
        }

        // ✅ 每幀更新音樂緩衝
        UpdateMusicStream(dorothyBGM);
    }

    // --- Cleanup ---
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            freeVector(&game->shop_piles[i][j]);

    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 3; ++j)
            freeVector(&game->shop_skill_piles[i][j]);

    UnloadMusicStream(dorothyBGM);
    for (int i = 0; i < OTHER_BGM_COUNT; ++i) {
        UnloadMusicStream(otherBGMs[i]);
    }
    CloseAudioDevice();

    free(game); // Free the allocated memory for the game struct
    UnloadTexture(backgroundTexture);
    UnloadFont(font);
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
