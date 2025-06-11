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
    GAME_STATE_AWAITING_BASIC_FOR_SKILL // [NEW] State for when a skill card is played and waits for a basic card.
} GameState;

// Main Game Struct
typedef struct Game {
    game inner_game;
    GameState current_state;
    const char* message;
    float bot_action_timer;
    bool player_has_acted;

    vector shop_piles[3][3];
    int pending_move_distance; 
    int turn_count;
    int pending_skill_card_index; // [NEW] Stores the hand index of the selected skill card.
} Game;

// Function Prototypes
void InitGame(Game* game);
void UpdateGame(Game* game, bool *should_close); 

#endif // GAME_LOGIC_H
