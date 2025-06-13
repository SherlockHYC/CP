#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "architecture.h"
#include <stdbool.h>

// GUI Game States
typedef enum {
    GAME_STATE_CHOOSE_CHAR,
    GAME_STATE_HUMAN_TURN,
    GAME_STATE_BOT_TURN,
    GAME_STATE_GAME_OVER,
    GAME_STATE_SHOP, 
    GAME_STATE_CHOOSE_MOVE_DIRECTION,
    GAME_STATE_FOCUS_REMOVE,
    GAME_STATE_AWAITING_BASIC_FOR_SKILL,
    GAME_STATE_CHOOSE_KNOCKBACK_DIRECTION // [新增] 選擇擊退方向的狀態
} GameState;

// Main Game Struct
typedef struct Game {
    game inner_game;
    GameState current_state;
    const char* message;
    float bot_action_timer;
    bool player_has_acted;

    vector shop_piles[3][3];
    vector shop_skill_piles[10][3];  // [新增] 技能牌商店：10角色 * 3技能類型（每種有LV2/LV3）
    int pending_move_distance; 
    int turn_count;
    int pending_skill_card_index;

    // [新] 新增一個變數來記錄當前的商店頁面 (0: 基礎牌, 1: 技能牌)
    int shop_page;
} Game;

// Function Prototypes
void InitGame(Game* game);
void UpdateGame(Game* game, bool *should_close); 

#endif // GAME_LOGIC_H
