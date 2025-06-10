#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "architecture.h"

// GUI 遊戲狀態
typedef enum {
    GAME_STATE_CHOOSE_CHAR,
    GAME_STATE_HUMAN_TURN,
    GAME_STATE_BOT_TURN,
    GAME_STATE_GAME_OVER
} GameState;

// 主遊戲結構
typedef struct Game {
    game inner_game;
    GameState current_state;
    const char* message;
    float bot_action_timer;
    bool player_has_acted; // [新增] 追蹤玩家本回合是否已行動
} Game;

// 函式原型
void InitGame(Game* game);
void UpdateGame(Game* game, bool *should_close); 

#endif // GAME_LOGIC_H
