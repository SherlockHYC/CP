#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "architecture.h"
#include <stdbool.h>

// (Notice) 將反擊效果的結構改為一個簡單的陣列，用索引代表玩家
// int8_t pending_retaliation_level[2]; // [0] for player 0, [1] for player 1


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

    // ✅ 新增的狀態
    GAME_STATE_PASSIVE_INFO,
    GAME_STATE_BATTLE,
    GAME_STATE_ULTRA,

    GAME_STATE_SLEEPING_BEAUTY_CHOOSE_HP_COST
} GameState;

// Main Game Struct
typedef struct Game {
    game inner_game;
    GameState current_state;
    const char* message;
    float bot_action_timer;
    bool player_has_acted;

    vector shop_piles[3][3];
    vector shop_skill_piles[10][3];
    int pending_move_distance; 
    int turn_count;
    int pending_skill_card_index;
    int pending_basic_card_index;

    int8_t pending_retaliation_level[2];


    // [新] 新增一個變數來記錄當前的商店頁面 (0: 基礎牌, 1: 技能牌)
    int shop_page;
} Game;

// Function Prototypes
void InitGame(Game* game);
void UpdateGame(Game* game, bool *should_close); 

int apply_damage(player* attacker, player* defender, int base_damage);


#endif // GAME_LOGIC_H
