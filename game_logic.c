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
void apply_movement(Game* game, int direction);
void apply_buy_card(Game* game, int card_id);

// [MODIFIED] This function now correctly discards the hand at the end of a turn.
void end_turn(Game* game) {
    int player_id = game->inner_game.now_turn_player_id;
    player* p = &game->inner_game.players[player_id];

    // 1. Move all cards from the current hand to the graveyard.
    for (uint32_t i = 0; i < p->hand.SIZE; i++) {
        pushbackVector(&p->graveyard, p->hand.array[i]);
    }
    
    // 2. Clear the hand vector.
    clearVector(&p->hand);

    // 3. Reset turn-based stats.
    p->energy = 0;
    p->defense = 0;
    
    // 4. Switch to the next player and start their turn.
    game->inner_game.now_turn_player_id = (player_id + 1) % 2;
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
                    
                    game->inner_game.players[0].locate[0] = 6;
                    game->inner_game.players[1].locate[0] = 4;

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
            Rectangle focus_btn = { GetScreenWidth() - 200.0f, GetScreenHeight() - 120.0f, 180, 50 };
            if (CheckCollisionPointRec(GetMousePosition(), focus_btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (!game->player_has_acted) {
                    game->message = "You chose to Focus!";
                    end_turn(game);
                    return;
                } else {
                    game->message = "Focus must be your first action!";
                }
            }
            
            Rectangle shop_btn = { GetScreenWidth() - 200.0f, GetScreenHeight() - 180.0f, 180, 50 };
            if (CheckCollisionPointRec(GetMousePosition(), shop_btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                game->current_state = GAME_STATE_SHOP;
                game->message = "Shop";
                return;
            }

            player* human = &game->inner_game.players[0];
            int hand_width = human->hand.SIZE * (CARD_WIDTH + 15) - 15;
            float hand_start_x = (GetScreenWidth() - hand_width) / 2.0f;
            float hand_y = GetScreenHeight() - CARD_HEIGHT - 20;

            for (uint32_t i = 0; i < human->hand.SIZE; ++i) {
                Rectangle card_bounds = { hand_start_x + i * (CARD_WIDTH + 15), hand_y, CARD_WIDTH, CARD_HEIGHT };
                 if (CheckCollisionPointRec(GetMousePosition(), card_bounds)) {
                    card_bounds.y -= 20;
                }
                if (CheckCollisionPointRec(GetMousePosition(), card_bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    apply_card_effect(game, i);
                    break;
                }
            }
            
            Rectangle end_turn_btn = { GetScreenWidth() - 200.0f, GetScreenHeight() - 60.0f, 180, 50 };
            if (CheckCollisionPointRec(GetMousePosition(), end_turn_btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                end_turn(game);
            }
            break;
        }
        case GAME_STATE_SHOP: {
            float startY = 120;
            float startX = 100;
            float card_gap_x = CARD_WIDTH + 20;
            float card_gap_y = CARD_HEIGHT + 40;

            for (int type = 0; type < 3; type++) {
                for (int level = 0; level < 3; level++) {
                    const vector* pile = &game->shop_piles[type][level];
                    if (pile->SIZE > 0) {
                        const Card* card = get_card_info(pile->array[0]);
                        if(card) {
                            Rectangle bounds = {startX + level * card_gap_x, startY + type * card_gap_y, CARD_WIDTH, CARD_HEIGHT};
                            if (CheckCollisionPointRec(GetMousePosition(), bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                                apply_buy_card(game, card->id);
                                return;
                            }
                        }
                    }
                }
            }
            
            Rectangle close_btn = { GetScreenWidth() - 160, GetScreenHeight() - 70, 140, 50 };
            if(CheckCollisionPointRec(GetMousePosition(), close_btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                game->current_state = GAME_STATE_HUMAN_TURN;
                game->message = "Your Turn!";
            }
            break;
        }
        case GAME_STATE_CHOOSE_MOVE_DIRECTION: {
            Rectangle leftBtn = {480, 350, 120, 50};
            Rectangle rightBtn = {680, 350, 120, 50};
            if (CheckCollisionPointRec(GetMousePosition(), leftBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                apply_movement(game, -1);
            }
            if (CheckCollisionPointRec(GetMousePosition(), rightBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                apply_movement(game, 1);
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
    p->character = character; 
    p->deck = initVector(); 
    p->hand = initVector(); 
    p->graveyard = initVector(); 
    p->life = 20; 
    p->defense = 0; 
    p->energy = 0;
    
    for(int k=0; k<3; ++k) pushbackVector(&p->deck, 101);
    for(int k=0; k<3; ++k) pushbackVector(&p->deck, 201);
    for(int k=0; k<3; ++k) pushbackVector(&p->deck, 301);

    switch(character) {
        case RED_HOOD: for(int k=0; k<3; ++k) pushbackVector(&p->deck, 501); break;
        case SNOW_WHITE: for(int k=0; k<3; ++k) pushbackVector(&p->deck, 502); break;
        case SLEEPING_BEAUTY: for(int k=0; k<3; ++k) pushbackVector(&p->deck, 503); break;
        case ALICE: for(int k=0; k<3; ++k) pushbackVector(&p->deck, 504); break;
        case MULAN: for(int k=0; k<3; ++k) pushbackVector(&p->deck, 505); break;
        case KAGUYA: for(int k=0; k<3; ++k) pushbackVector(&p->deck, 506); break;
        case MERMAID: for(int k=0; k<3; ++k) pushbackVector(&p->deck, 507); break;
        case MATCH_GIRL: for(int k=0; k<3; ++k) pushbackVector(&p->deck, 508); break;
        case DOROTHY: for(int k=0; k<3; ++k) pushbackVector(&p->deck, 509); break;
        case SCHEHERAZADE: for(int k=0; k<3; ++k) pushbackVector(&p->deck, 510); break;
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

    for(int i=0; i<3; ++i) for(int j=0; j<3; ++j) game->shop_piles[i][j] = initVector();

    for(int i=0; i<6; ++i) {
        pushbackVector(&game->shop_piles[0][0], 101); 
        pushbackVector(&game->shop_piles[1][0], 201); 
        pushbackVector(&game->shop_piles[2][0], 301); 
    }
    for(int i=0; i<9; ++i) {
        pushbackVector(&game->shop_piles[0][1], 102);
        pushbackVector(&game->shop_piles[1][1], 202);
        pushbackVector(&game->shop_piles[2][1], 302);
    }
    for(int i=0; i<9; ++i) {
        pushbackVector(&game->shop_piles[0][2], 103);
        pushbackVector(&game->shop_piles[1][2], 203);
        pushbackVector(&game->shop_piles[2][2], 303);
    }
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

void apply_movement(Game* game, int direction) {
    int player_id = game->inner_game.now_turn_player_id;
    player* mover = &game->inner_game.players[player_id];
    player* opponent = &game->inner_game.players[(player_id + 1) % 2];
    
    int current_pos = mover->locate[0];
    int opponent_pos = opponent->locate[0];
    int move_dist = game->pending_move_distance;

    int target_pos = current_pos + (direction * move_dist);

    if (target_pos < 0) target_pos = 0;
    if (target_pos > 10) target_pos = 10;

    if (target_pos == opponent_pos) {
        target_pos -= direction;
    }

    mover->locate[0] = target_pos;
    game->current_state = GAME_STATE_HUMAN_TURN;
    game->message = "You moved!";
}

void apply_card_effect(Game* game, int card_hand_index) {
    int player_id = game->inner_game.now_turn_player_id;
    player* attacker = &game->inner_game.players[player_id];
    player* defender = &game->inner_game.players[(player_id + 1) % 2];
    if (card_hand_index < 0 || card_hand_index >= (int)attacker->hand.SIZE) return;
    const Card* card = get_card_info(attacker->hand.array[card_hand_index]);
    if (!card) return;

    int32_t played_card_id = card->id;
    CardType type = card->type;

    if (type == ATTACK || type == DEFENSE || type == GENERIC) {
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
    } else if (type == MOVE) {
        attacker->energy += card->value;
        game->pending_move_distance = card->value;
        game->current_state = GAME_STATE_CHOOSE_MOVE_DIRECTION;
        game->message = "Choose a direction to move";
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
    
    pushbackVector(&attacker->graveyard, played_card_id);
    eraseVector(&attacker->hand, card_hand_index);
}

void apply_buy_card(Game* game, int card_id) {
    player* buyer = &game->inner_game.players[game->inner_game.now_turn_player_id];
    const Card* card_to_buy = get_card_info(card_id);

    if (!card_to_buy) return;

    if (buyer->energy < card_to_buy->cost) {
        game->message = "Not enough energy!";
        return;
    }

    bool found_in_supply = false;
    for(int type=0; type < 3; ++type) {
        for(int lvl=0; lvl<3; ++lvl) {
            vector* pile = &game->shop_piles[type][lvl];
            if(pile->SIZE > 0 && pile->array[0] == card_id) {
                popbackVector(pile);
                found_in_supply = true;
                break;
            }
        }
        if(found_in_supply) break;
    }
    
    if(!found_in_supply) {
        game->message = "This card is sold out!";
        return;
    }

    buyer->energy -= card_to_buy->cost;
    pushbackVector(&buyer->graveyard, card_id);
    game->message = TextFormat("Bought %s!", card_to_buy->name);
    game->player_has_acted = true;
}
