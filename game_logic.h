#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "architecture.h"
#include <stdbool.h>

// (Notice) 將反擊效果的結構改為一個簡單的陣列，用索引代表玩家
// int8_t pending_retaliation_level[2]; // [0] for player 0, [1] for player 1

#define HAND_SCALE 0.8f  // 玩家手牌縮放比例
#define CARD_SCALE 0.8f
// GUI Game States
typedef enum {
    GAME_STATE_CHOOSE_CHAR,
    GAME_STATE_PVP_CHOOSE_CHAR_P1,
    GAME_STATE_PVP_CHOOSE_CHAR_P2,
    GAME_STATE_PLAYER_1_TURN,
    GAME_STATE_PLAYER_2_TURN,
    GAME_STATE_HUMAN_TURN,
    GAME_STATE_BOT_TURN,
    GAME_STATE_GAME_OVER,
    GAME_STATE_SHOP, 
    GAME_STATE_CHOOSE_MOVE_DIRECTION,
    GAME_STATE_FOCUS_REMOVE,
    GAME_STATE_AWAITING_BASIC_FOR_SKILL,
    GAME_STATE_PASSIVE_INFO,
    GAME_STATE_BATTLE,
    GAME_STATE_ULTRA,
    GAME_STATE_OVERLOAD_CONFIRM,
    GAME_STATE_OVERLOAD_SELECT,
    GAME_STATE_SLEEPING_BEAUTY_CHOOSE_HP_COST,
    GAME_STATE_CACHE_SELECT,
    GAME_STATE_SLEEPING_BEAUTY_CHOOSE_AWAKEN_COST,
    GAME_STATE_SLEEPING_BEAUTY_CHOOSE_MOVE_AWAKEN_COST
} GameState;

// Main Game Struct
typedef struct Game {
    game inner_game;
    GameState current_state;
    const char* message;
    float bot_action_timer;
    bool player_has_acted;
    int p1_selected_char; 

    // --- [修正] 確保只有一個 shop_piles 的宣告 ---
    vector shop_piles[2][3][3]; // [player_id][type][level]
    // ---------------------------------------------
    
    vector shop_skill_piles[10][3];
    int pending_move_distance; 
    int turn_count;
    int pending_skill_card_index;
    int pending_basic_card_index;

    int8_t pending_retaliation_level[2];
    int thorns_attacks_left[2];
    int thorns_damage_bonus[2];
    
    int shop_page;
    bool cacheA_enabled;
    int cacheA_card_id;
    
    bool show_scream_image;
    float scream_timer;
    int scream_image_index;
        
} Game;

// (函式原型不變)
void InitGame(Game* game);
void UpdateGame(Game* game, bool *should_close); 
void UpdatePVPGame(Game* game, bool *should_close);
void resolve_sleeping_beauty_defense(Game* game, int chosen_awaken_cost);
void resolve_sleeping_beauty_move(Game* game, int chosen_awaken_cost);
int apply_damage(player* attacker, player* defender, int base_damage);


#endif // GAME_LOGIC_H
