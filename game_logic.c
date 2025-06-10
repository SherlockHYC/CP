#include "game_logic.h"
#include "bot_ai.h"
#include "definitions.h"
#include "raylib.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

// Function Prototypes
void shuffle_deck(vector* deck);
void draw_card(player* p);
void start_turn(Game* game);
void init_player_deck(player* p, CharacterType character);
void apply_card_effect(Game* game, int card_hand_index);

void end_turn(Game* game) {
    player* p = &game->inner_game.players[game->inner_game.now_turn_player_id];
    p->energy = 0;
    p->defense = 0;
    
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
            player* human = &game->inner_game.players[0];
            int hand_width = human->hand.SIZE * (CARD_WIDTH + 15) - 15;
            float hand_start_x = (GetScreenWidth() - hand_width) / 2.0f;
            float hand_y = GetScreenHeight() - CARD_HEIGHT - 20;

            for (uint32_t i = 0; i < human->hand.SIZE; ++i) {
                Rectangle card_bounds = { hand_start_x + i * (CARD_WIDTH + 15), hand_y, CARD_WIDTH, CARD_HEIGHT };
                 if (CheckCollisionPointRec(GetMousePosition(), card_bounds)) {
                    card_bounds.y -= 20; // Hover effect
                }
                if (CheckCollisionPointRec(GetMousePosition(), card_bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    apply_card_effect(game, i);
                    break;
                }
            }
            
            // [MODIFIED] Update End Turn button coordinates to match the GUI
            Rectangle end_turn_btn = { GetScreenWidth() - 200.0f, GetScreenHeight() - 60.0f, 180, 50 };
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
    int player_id = game->inner_game.now_turn_player_id;
    player* p = &game->inner_game.players[player_id];
    
    p->defense = 0;
    game->player_has_acted = false;
    
    while(p->hand.SIZE < 6) {
        draw_card(p);
    }
    if (player_id == 0) {
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

void apply_card_effect(Game* game, int card_hand_index) {
    int player_id = game->inner_game.now_turn_player_id;
    player* attacker = &game->inner_game.players[player_id];
    player* defender = &game->inner_game.players[(player_id + 1) % 2];
    if (card_hand_index < 0 || card_hand_index >= (int)attacker->hand.SIZE) return;
    const Card* card = get_card_info(attacker->hand.array[card_hand_index]);
    if (!card) return;

    CardType type = card->type;

    if (type == ATTACK || type == DEFENSE || type == MOVE || type == GENERIC) {
        attacker->energy += card->value;
        if(type == ATTACK) {
             int damage = card->value;
             if(defender->defense >= damage) defender->defense -= damage;
             else { 
                int damage_left = damage - defender->defense;
                defender->defense = 0;
                if (defender->life <= damage_left) defender->life = 0;
                else defender->life -= damage_left;
             }
        } else if (type == DEFENSE) {
            attacker->defense += card->value;
        }
    } else if (type == SKILL) {
        if (attacker->energy < card->cost) {
            game->message = "Not enough energy for this skill!";
            return;
        }
        attacker->energy -= card->cost;
        if(card->value > 0) {
             int damage = card->value;
             if(defender->defense >= damage) defender->defense -= damage;
             else { 
                int damage_left = damage - defender->defense;
                defender->defense = 0;
                if(defender->life <= damage_left) defender->life = 0;
                else defender->life -= damage_left;
             }
        }
    }

    game->player_has_acted = true;
    pushbackVector(&attacker->graveyard, card->id);
    eraseVector(&attacker->hand, card_hand_index);
}
