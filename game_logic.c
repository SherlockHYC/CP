#include "game_logic.h"
#include "bot_ai.h"
#include "definitions.h"
#include "raylib.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

// 函式原型
void apply_card_effect(Game* game, int card_hand_index);
void end_turn(Game* game);
void start_turn(Game* game);
void init_player_deck(player* p, CharacterType character);
void draw_card(player* p);
void shuffle_deck(vector* deck);


// [修改] UpdateGame 不再回傳值，而是透過指標來設定退出請求
void UpdateGame(Game* game, bool* should_close) {
    // 根據遊戲狀態處理更新與輸入
    switch (game->current_state) {
        case GAME_STATE_CHOOSE_CHAR: {
            // 處理角色選擇按鈕
            for(int i = 0; i < 10; i++) {
                Rectangle btn_bounds = {200.0f + (i % 5) * 180, 250.0f + (i / 5) * 120, 160, 80};
                if (CheckCollisionPointRec(GetMousePosition(), btn_bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    init_player_deck(&game->inner_game.players[0], (CharacterType)i);
                    init_player_deck(&game->inner_game.players[1], (CharacterType)(rand() % 10));
                    game->inner_game.now_turn_player_id = 0;
                    start_turn(game);
                    return; // 動作已處理，立刻結束本幀的更新
                }
            }
            // 處理退出按鈕
            Rectangle exit_btn = { GetScreenWidth() - 180.0f, GetScreenHeight() - 70.0f, 160, 50 };
            if (CheckCollisionPointRec(GetMousePosition(), exit_btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                printf("[DEBUG] Exit button clicked. Setting quit flag...\n");
                *should_close = true; // [修改] 直接設定退出旗標
                return; 
            }
            break;
        }
        case GAME_STATE_HUMAN_TURN: {
            // 處理手牌和結束回合按鈕的邏輯
            player* human = &game->inner_game.players[0];
            for (uint32_t i = 0; i < human->hand.SIZE; ++i) {
                Rectangle card_bounds = { 300 + i * 140.0f, 520, 120, 150 };
                if (CheckCollisionPointRec(GetMousePosition(), card_bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    apply_card_effect(game, i);
                }
            }
            Rectangle end_turn_btn = { 1100, 650, 160, 50 };
            if (CheckCollisionPointRec(GetMousePosition(), end_turn_btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                end_turn(game);
            }
            break;
        }
        case GAME_STATE_BOT_TURN: {
            // Bot 邏輯
            game->bot_action_timer -= GetFrameTime();
            if (game->bot_action_timer <= 0) {
                int card_to_play = get_bot_action(&game->inner_game);
                if (card_to_play != -1) {
                    apply_card_effect(game, card_to_play);
                }
                end_turn(game);
            }
            break;
        }
        case GAME_STATE_GAME_OVER: {
            // "回到主選單"按鈕邏輯
            Rectangle back_btn = { (float)GetScreenWidth()/2 - 125, (float)GetScreenHeight()/2 + 40, 250, 50 };
            if (CheckCollisionPointRec(GetMousePosition(), back_btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                InitGame(game);
            }
            break;
        }
    }

    // 只有在遊戲進行中才檢查勝利/失敗條件
    if (game->current_state != GAME_STATE_CHOOSE_CHAR && game->current_state != GAME_STATE_GAME_OVER) {
        if (game->inner_game.players[0].life <= 0) { game->message = "You Lose!"; game->current_state = GAME_STATE_GAME_OVER; }
        if (game->inner_game.players[1].life <= 0) { game->message = "You Win!"; game->current_state = GAME_STATE_GAME_OVER; }
    }
}

// --- 以下是其他函式的實作 (內容不變) ---
void apply_card_effect(Game* game, int card_hand_index) {
     player* attacker = &game->inner_game.players[game->inner_game.now_turn_player_id];
     player* defender = &game->inner_game.players[(game->inner_game.now_turn_player_id + 1) % 2];

    if (card_hand_index < 0 || card_hand_index >= attacker->hand.SIZE) return;
    
    const Card* card = get_card_info(attacker->hand.array[card_hand_index]);
    if (!card) return;
    if (attacker->energy <= 0) { game->message = "Not enough energy!"; return; }

    attacker->energy--;
    
    if (card->type == ATTACK || card->type == SKILL) {
        int damage = card->value;
        if(defender->defense >= damage) defender->defense -= damage;
        else { defender->life -= (damage - defender->defense); defender->defense = 0; }
    }
    
    pushbackVector(&attacker->graveyard, card->id);
    eraseVector(&attacker->hand, card_hand_index);
}

void end_turn(Game* game) {
    game->inner_game.now_turn_player_id = (game->inner_game.now_turn_player_id + 1) % 2;
    start_turn(game);
}

void init_player_deck(player* p, CharacterType character) {
    p->character = character; p->deck = initVector(); p->hand = initVector(); p->graveyard = initVector(); p->life = 20; p->defense = 0; p->energy = 0;
    for(int k=0; k<7; ++k) pushbackVector(&p->deck, 101);
    for(int k=0; k<3; ++k) pushbackVector(&p->deck, 401);
    switch(character) {
        case RED_HOOD: for(int k=0; k<2; ++k) pushbackVector(&p->deck, 501); break;
        case SNOW_WHITE: for(int k=0; k<2; ++k) pushbackVector(&p->deck, 502); break;
        case SLEEPING_BEAUTY: for(int k=0; k<2; ++k) pushbackVector(&p->deck, 503); break;
        case ALICE: for(int k=0; k<2; ++k) pushbackVector(&p->deck, 504); break;
        case MULAN: for(int k=0; k<2; ++k) pushbackVector(&p->deck, 505); break;
        case KAGUYA: for(int k=0; k<2; ++k) pushbackVector(&p->deck, 506); break;
        case MERMAID: for(int k=0; k<2; ++k) pushbackVector(&p->deck, 507); break;
        case MATCH_GIRL: for(int k=0; k<2; ++k) pushbackVector(&p->deck, 508); break;
        case DOROTHY: for(int k=0; k<2; ++k) pushbackVector(&p->deck, 509); break;
        case SCHEHERAZADE: for(int k=0; k<2; ++k) pushbackVector(&p->deck, 510); break;
    }
    shuffle_deck(&p->deck);
}

void start_turn(Game* game) {
    player* p = &game->inner_game.players[game->inner_game.now_turn_player_id];
    p->energy = 3;
    while(p->hand.SIZE < 5) {
        draw_card(p);
    }
    if (game->inner_game.now_turn_player_id == 0) {
        game->current_state = GAME_STATE_HUMAN_TURN;
        game->message = "Your Turn!";
    } else {
        game->current_state = GAME_STATE_BOT_TURN;
        game->message = "Opponent's Turn...";
        game->bot_action_timer = 1.0f;
    }
}

void InitGame(Game* game) {
    srand(time(NULL));
    memset(game, 0, sizeof(Game));
    game->current_state = GAME_STATE_CHOOSE_CHAR;
    game->message = "Select Your Hero";
}

void shuffle_deck(vector* deck) {
    if (deck->SIZE < 2) return;
    for (uint32_t i = 0; i < deck->SIZE - 1; ++i) {
        uint32_t j = i + rand() % (deck->SIZE - i);
        int32_t temp = deck->array[i];
        deck->array[i] = deck->array[j];
        deck->array[j] = temp;
    }
}

void draw_card(player* p) {
    if (p->deck.SIZE == 0) {
        if (p->graveyard.SIZE == 0) return;
        p->deck = p->graveyard;
        p->graveyard = initVector();
        shuffle_deck(&p->deck);
    }
    if (p->hand.SIZE < 10) {
        pushbackVector(&p->hand, p->deck.array[p->deck.SIZE - 1]);
        popbackVector(&p->deck);
    }
}
