#include "raylib.h"
#include "game_logic.h"
#include "gui.h"
#include <stdio.h>
#include <string.h>

// 全域變數
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

    const int screenWidth = 1280;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "Twisted Fables - GUI Edition");

    if (!IsWindowReady()) {
        printf("ERROR: Failed to initialize window.\n");
        return 1;
    }

    font = GetFontDefault();
    backgroundTexture = LoadTexture("background.png");
    
    SetTargetFPS(60);

    Game game;
    InitGame(&game);

    // [新增] 我們的自訂退出旗標
    bool should_exit = false; 
    
    // --- 遊戲主迴圈 ---
    // [修改] 迴圈現在會檢查 Raylib 的退出信號和我們自己的退出旗標
    while (!should_exit && !WindowShouldClose())
    {
        // 1. 更新遊戲狀態，並將退出旗標的指標傳入
        UpdateGame(&game, &should_exit);
        
        // 2. 繪圖
        BeginDrawing();
        ClearBackground(DARKGRAY);
        DrawGame(&game);
        EndDrawing();
    }

    // --- 清理資源並關閉視窗 ---
    UnloadTexture(backgroundTexture);
    CloseWindow();

    return 0;
}


void show_help() {
    printf("\n--- Twisted Fables GUI Edition 使用說明 ---\n\n");
    printf("這是一款 1v1 的卡牌對戰遊戲，您的目標是將對手的生命值降到 0。\n\n");
    printf("【執行模式】\n");
    printf("  ./TwistedFablesGUI         : (預設) 啟動玩家 vs. Bot 的圖形化遊戲。\n");
    printf("  ./TwistedFablesGUI --help  : 顯示此說明訊息。\n\n");
    printf("【遊戲玩法】\n");
    printf("  1. 遊戲開始時，使用滑鼠點擊選擇您想扮演的英雄角色。\n");
    printf("  2. 在您的回合中，您可以點擊手牌來使用它 (前提是能量足夠)。\n");
    printf("  3. 點擊畫面右下角的「結束回合」按鈕來結束您的行動，輪到對手出牌。\n");
    printf("  4. 透過購買、使用技能和攻擊牌來擊敗您的對手！\n\n");
    printf("祝您遊戲愉快！\n\n");
}
