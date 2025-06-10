#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "architecture.h"

// GUI Game States
typedef enum {
    GAME_STATE_CHOOSE_CHAR,
    GAME_STATE_HUMAN_TURN,
    GAME_STATE_CHOOSE_MOVE_DIRECTION, // New state for choosing move direction
    GAME_STATE_BOT_TURN,
    GAME_STATE_GAME_OVER
} GameState;

// Main Game Struct
typedef struct Game {
    game inner_game;
    GameState current_state;
    const char* message;
    float bot_action_timer;
    bool player_has_acted;
    int pending_move_distance; // New: stores the distance for the pending move
} Game;

// Function Prototypes
void InitGame(Game* game);
void UpdateGame(Game* game, bool *should_close); 

#endif // GAME_LOGIC_H
