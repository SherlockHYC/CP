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
    GAME_STATE_CHOOSE_MOVE_DIRECTION 
} GameState;

// Main Game Struct
typedef struct Game {
    game inner_game;
    GameState current_state;
    const char* message;
    float bot_action_timer;
    bool player_has_acted;

    // Shop card piles: [Type][Level-1]
    // Type 0: Attack, 1: Defense, 2: Move
    vector shop_piles[3][3];

    // [FIX] Added the missing member back
    int pending_move_distance; 
} Game;

// Function Prototypes
void InitGame(Game* game);
void UpdateGame(Game* game, bool *should_close); 

#endif // GAME_LOGIC_H
