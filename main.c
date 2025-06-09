#include "raylib.h"
#include "game_logic.h"
#include "gui.h"
#include <stdio.h>
#include <string.h>

// 全域變數，用來存放載入的字型
Font font;

void show_help();

int main(int argc, char *argv[])
{
    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        show_help();
        return 0;
    }

    // 初始化視窗
    const int screenWidth = 1280;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "Twisted Fables - GUI Edition");

    // 載入中文字型檔 (請確保檔案與執行檔在同一目錄下)
    font = LoadFontEx("NotoSansTC-Regular.otf", 32, 0, 0);
    SetTargetFPS(60);

    Game game;
    InitGame(&game);
    
    // 遊戲主迴圈
    while (!WindowShouldClose())
    {
        UpdateGame(&game);

        BeginDrawing();
        ClearBackground(DARKGRAY);
        DrawGame(&game);
        EndDrawing();
    }

    // 清理資源
    UnloadFont(font); // 卸載字型
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
