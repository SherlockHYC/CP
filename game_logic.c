#include "game_logic.h"
#include "bot_ai.h"
#include "definitions.h"
#include "raylib.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

// 函式原型
void shuffle_deck(vector* deck);
void draw_card(player* p);
void start_turn(Game* game);
void init_player_deck(player* p, CharacterType character);

// [修改] 重構傷害計算邏輯以修正血量溢位 Bug
void apply_card_effect(Game* game, int card_hand_index) {
    player* attacker = &game->inner_game.players[game->inner_game.now_turn_player_id];
    player* defender = &game->inner_game.players[(game->inner_game.now_turn_player_id + 1) % 2];

    if (card_hand_index < 0 || card_hand_index >= (int)attacker->hand.SIZE) return;
    
    const Card* card = get_card_info(attacker->hand.array[card_hand_index]);
    if (!card) return;
    if (attacker->energy <= 0) { game->message = "能量不足!"; return; }

    attacker->energy--;
    game->player_has_acted = true;
    
    // --- 處理卡牌效果 ---
    // 處理防禦牌
    if (card->type == DEFENSE) {
        attacker->defense += card->value;
    } 
    // 處理所有造成傷害的牌 (攻擊牌與技能牌)
    else if (card->type == ATTACK || (card->type == SKILL && card->value > 0)) {
        int damage_to_deal = card->value;
        
        // 1. 先用防禦抵銷傷害
        if (defender->defense > 0) {
            if (defender->defense >= damage_to_deal) {
                defender->defense -= damage_to_deal;
                damage_to_deal = 0; // 傷害被完全抵擋
            } else {
                damage_to_deal -= defender->defense;
                defender->defense = 0;
            }
        }
        
        // 2. 如果還有剩餘傷害，再安全地扣除生命值
        if (damage_to_deal > 0) {
            // [修正] 這是防止血量溢位的關鍵檢查
            if (defender->life <= damage_to_deal) {
                defender->life = 0; // 如果傷害大於等於目前生命，直接設為0
            } else {
                defender->life -= damage_to_deal; // 否則才執行減法
            }
        }
    }
    
    // 將使用過的卡牌移至棄牌堆
    pushbackVector(&attacker->graveyard, card->id);
    eraseVector(&attacker->hand, card_hand_index);
}

void end_turn(Game* game) {
    game->inner_game.players[game->inner_game.now_turn_player_id].defense = 0;
    game->inner_game.now_turn_player_id = (game->inner_game.now_turn_player_id + 1) % 2;
    start_turn(game);
}

void UpdateGame(Game* game, bool* should_close) {
    switch (game->current_state) {
        case GAME_STATE_CHOOSE_CHAR: {
            for(int i = 0; i < 10; i++) {
                Rectangle btn_bounds = {200.0f + (i % 5) * 180, 250.0f + (i / 5) * 120, 160, 80};
                if (CheckCollisionPointRec(GetMousePosition(), btn_bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    init_player_deck(&game->inner_game.players[0], (CharacterType)i);
                    init_player_deck(&game->inner_game.players[1], (CharacterType)(rand() % 10));
                    start_turn(game);
                    return;
                }
            }
            Rectangle exit_btn = { GetScreenWidth() - 180.0f, GetScreenHeight() - 70.0f, 160, 50 };
            if (CheckCollisionPointRec(GetMousePosition(), exit_btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                *should_close = true;
                return; 
            }
            break;
        }
        case GAME_STATE_HUMAN_TURN: {
            Rectangle focus_btn = { GetScreenWidth() - 180.0f, GetScreenHeight() - 130.0f, 160, 50 };
            if (CheckCollisionPointRec(GetMousePosition(), focus_btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (!game->player_has_acted) {
                    game->message = "你選擇了專注！";
                    end_turn(game);
                    return;
                } else {
                    game->message = "專注只能是第一個行動！";
                }
            }

            player* human = &game->inner_game.players[0];
            const int CARD_WIDTH = 120;
            const int CARD_HEIGHT = 170;
            int hand_width = human->hand.SIZE * (CARD_WIDTH + 10) - 10;
            float hand_start_x = (GetScreenWidth() - hand_width) / 2.0f;
            for (uint32_t i = 0; i < human->hand.SIZE; ++i) {
                Rectangle card_bounds = { hand_start_x + i * (CARD_WIDTH + 10), GetScreenHeight() - CARD_HEIGHT - 20, CARD_WIDTH, CARD_HEIGHT };
                if (CheckCollisionPointRec(GetMousePosition(), card_bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    apply_card_effect(game, i);
                    break; 
                }
            }

            Rectangle end_turn_btn = { GetScreenWidth() - 180.0f, GetScreenHeight() - 70.0f, 160, 50 };
            if (CheckCollisionPointRec(GetMousePosition(), end_turn_btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                end_turn(game);
            }
            break;
        }
        case GAME_STATE_BOT_TURN: {
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
            Rectangle back_btn = { (float)GetScreenWidth()/2 - 125, (float)GetScreenHeight()/2 + 40, 250, 50 };
            if (CheckCollisionPointRec(GetMousePosition(), back_btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                InitGame(game);
            }
            break;
        }
    }
    if (game->current_state != GAME_STATE_CHOOSE_CHAR && game->current_state != GAME_STATE_GAME_OVER) {
        if (game->inner_game.players[0].life <= 0) { game->message = "You Lose!"; game->current_state = GAME_STATE_GAME_OVER; }
        if (game->inner_game.players[1].life <= 0) { game->message = "You Win!"; game->current_state = GAME_STATE_GAME_OVER; }
    }
}

void init_player_deck(player* p, CharacterType character) {
    p->character = character; p->deck = initVector(); p->hand = initVector(); p->graveyard = initVector(); p->life = 20; p->defense = 0; p->energy = 0;
    
    for(int k=0; k<5; ++k) pushbackVector(&p->deck, 101);
    for(int k=0; k<2; ++k) pushbackVector(&p->deck, 201);
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
    p->defense = 0;
    game->player_has_acted = false;
    while(p->hand.SIZE < 5) {
        draw_card(p);
    }
    if (game->inner_game.now_turn_player_id == 0) {
        game->current_state = GAME_STATE_HUMAN_TURN;
        game->message = "你的回合！";
    } else {
        game->current_state = GAME_STATE_BOT_TURN;
        game->message = "對手回合...";
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
