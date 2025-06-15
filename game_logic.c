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
void apply_card_effect(Game* game, int card_hand_index);
void apply_movement(Game* game, int direction);
void apply_buy_card(Game* game, int card_id);
void apply_focus_remove(Game* game, int choice);
void resolve_skill_and_basic(Game* game, int skill_idx, int basic_idx);
void initialize_shop_skill_piles(Game* game);
void resolve_sleeping_beauty_attack(Game* game, int chosen_hp_cost);
void apply_poison_damage(player* p, const Card* card);
void start_turn_pvp(Game* game);
void end_turn_pvp(Game* game);

bool ultra_unlocked[4] = {false};

// PVB 模式的結束回合函式
void end_turn(Game* game) {
    int player_id = game->inner_game.now_turn_player_id;
    player* p = &game->inner_game.players[player_id];

    game->thorns_attacks_left[player_id] = 0;
    game->thorns_damage_bonus[player_id] = 0;

    for (uint32_t i = 0; i < p->hand.SIZE; i++) {
        int32_t card_id = p->hand.array[i];
        const Card* card = get_card_info(card_id);
        if (card && card->type == STATUS) {
            apply_poison_damage(p, card);
        }
        pushbackVector(&p->graveyard, card_id);
    }
    clearVector(&p->hand);
    
    p->energy = 0;
    
    uint32_t total_cards = p->deck.SIZE + p->graveyard.SIZE;
    int target_hand_size = (p->character == SNOW_WHITE) ? 4 : 6;
    if (total_cards < (uint32_t)target_hand_size) {
        target_hand_size = total_cards;
    }
    
    while(p->hand.SIZE < (uint32_t)target_hand_size) {
        draw_card(p);
    }
    
    game->inner_game.now_turn_player_id = (player_id + 1) % 2;
    start_turn(game);
}

#define HAND_SCALE 0.8f 
void UpdateGame(Game* game, bool* should_close) {
    switch (game->current_state) {
        case GAME_STATE_CHOOSE_CHAR: {
            for(int i = 0; i < 10; i++) {
                int row = i / 5;
                float extra_y = (row == 1) ? 40.0f : 0.0f;
                Rectangle btn_bounds = {
                    200.0f + (i % 5) * 180.0f,
                    280.0f + row * 200.0f + extra_y,
                    160,
                    80
                };
                if (CheckCollisionPointRec(GetMousePosition(), btn_bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    init_player_deck(&game->inner_game.players[0], (CharacterType)i);
                    init_player_deck(&game->inner_game.players[1], (CharacterType)(rand() % 10));
                    initialize_shop_skill_piles(game);

                    game->inner_game.players[0].locate[0] = 6;
                    game->inner_game.players[1].locate[0] = 4;

                    player* p0 = &game->inner_game.players[0];
                    while(p0->hand.SIZE < 4) { draw_card(p0); }
                    player* p1 = &game->inner_game.players[1];
                    while(p1->hand.SIZE < 6) { draw_card(p1); }
                    
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
                    game->current_state = GAME_STATE_FOCUS_REMOVE;
                    game->message = "Focus: Remove a card";
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
            player* opponent = &game->inner_game.players[1];
            int distance = abs(human->locate[0] - opponent->locate[0]);

            int scaled_card_width = CARD_WIDTH * HAND_SCALE;
            int scaled_card_height = CARD_HEIGHT * HAND_SCALE;
            int spacing = 15 * HAND_SCALE;

            int hand_width = human->hand.SIZE * (scaled_card_width + spacing) - spacing;
            float hand_start_x = (GetScreenWidth() - hand_width) / 2.0f;
            float hand_y = GetScreenHeight() - scaled_card_height - 20;

            for (uint32_t i = 0; i < human->hand.SIZE; ++i) {
                Rectangle card_bounds = {
                    hand_start_x + i * (scaled_card_width + spacing),
                    hand_y,
                    scaled_card_width,
                    scaled_card_height
                };
                if (CheckCollisionPointRec(GetMousePosition(), card_bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    
                    const Card* card = get_card_info(human->hand.array[i]);
                    if (!card) continue;

                    bool can_play = true;
                    if (card->type == ATTACK) {
                        if (distance > 1) {
                            can_play = false;
                            game->message = "No target in range!";
                        }
                    } else if (card->type == SKILL) {
                        int skill_subtype = card->id % 10;
                        int range = 0;
                        bool range_check_needed = false;

                        if (human->character == RED_HOOD) {
                            if (skill_subtype == 1 || skill_subtype == 2 || skill_subtype == 3) {
                                range = card->level;
                                range_check_needed = true;
                            }
                        }
                        else if (human->character == SNOW_WHITE) {
                            if (skill_subtype == 1 || skill_subtype == 2) { 
                                range = 1; 
                                range_check_needed = true;
                            }
                        }
                        else if (human->character == SLEEPING_BEAUTY) {
                            if (skill_subtype == 1) { 
                                range = 1; 
                                range_check_needed = true;
                            }
                        }
                        if (range_check_needed && distance > range) {
                            can_play = false;
                            game->message = "No target in range!";
                        }
                    }else if (card->type == STATUS) {
                        can_play = true;
                    }

                    if (can_play) {
                        apply_card_effect(game, i);
                    }
                    
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
            Rectangle basic_tab = { GetScreenWidth() / 2.0f - 210, 150, 200, 40 };
            Rectangle skill_tab = { GetScreenWidth() / 2.0f + 10, 150, 200, 40 };
            if (CheckCollisionPointRec(GetMousePosition(), basic_tab) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                game->shop_page = 0;
            }
            if (CheckCollisionPointRec(GetMousePosition(), skill_tab) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                game->shop_page = 1;
            }

            if (game->shop_page == 0) {
                float column_width = 300; 
                float startX = (GetScreenWidth() - (3 * column_width)) / 2.0f;
                
                float top_bound = 210;
                float bottom_bound = GetScreenHeight() - 90;
                float total_area_height = bottom_bound - top_bound;
                float row_spacing = total_area_height / 3.0f;

                for (int type = 0; type < 3; type++) {
                    float row_center_y = top_bound + (row_spacing * type) + (row_spacing / 2.0f);
                    float card_start_y = row_center_y - (CARD_HEIGHT / 2.0f);

                    for (int level = 0; level < 3; level++) {
                        const vector* pile = &game->shop_piles[0][type][level];
                        if (pile->SIZE > 0) {
                            const Card* card = get_card_info(pile->array[0]);
                            if(card) {
                                float card_start_x = startX + (level * column_width);
                                Rectangle bounds = {card_start_x, card_start_y, CARD_WIDTH, CARD_HEIGHT};
                                if (CheckCollisionPointRec(GetMousePosition(), bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                                    apply_buy_card(game, card->id);
                                    return;
                                }
                            }
                        }
                    }
                }
            }
            
            if (game->shop_page == 1) {
                int chara = game->inner_game.players[0].character;

                if (chara == RED_HOOD) {
                    for (int type = 0; type < 3; ++type) {
                            const vector* pile = &game->shop_skill_piles[chara][type];
                            if (!pile || pile->SIZE == 0) continue;

                            for (uint32_t i = 0; i < pile->SIZE; ++i) {
                                const Card* card = get_card_info(pile->array[i]);
                                if (!card) continue;

                                float x = 200 + (i % 5) * (CARD_WIDTH + 30);
                                float y = 250 + type * 250 + (i / 5) * (CARD_HEIGHT + 20);
                                Rectangle bounds = { x, y, CARD_WIDTH, CARD_HEIGHT };

                                if (CheckCollisionPointRec(GetMousePosition(), bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                                    apply_buy_card(game, card->id);
                                    return;
                                }
                            }
                    }
                } else {
                    game->message = "技能商店尚未支援此角色！";
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
                game->current_state = GAME_STATE_HUMAN_TURN;
            }
            if (CheckCollisionPointRec(GetMousePosition(), rightBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                apply_movement(game, 1);
                game->current_state = GAME_STATE_HUMAN_TURN;
            }
            break;
        }
        case GAME_STATE_FOCUS_REMOVE: {
            player* p = &game->inner_game.players[0];
            int choice = 0;
            bool action_taken = false;

            for (uint32_t i = 0; i < p->hand.SIZE; i++) {
                Rectangle card_bounds = { 50 + i * (CARD_WIDTH + 15), 180, CARD_WIDTH, CARD_HEIGHT };
                if (CheckCollisionPointRec(GetMousePosition(), card_bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    choice = i + 1;
                    action_taken = true;
                    break;
                }
            }
            if (action_taken) {
                apply_focus_remove(game, choice);
                return;
            }

            for (uint32_t i = 0; i < p->graveyard.SIZE; i++) {
                Rectangle card_bounds = { 50 + i * (CARD_WIDTH + 15), 430, CARD_WIDTH, CARD_HEIGHT };
                if (CheckCollisionPointRec(GetMousePosition(), card_bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    choice = -(int)(i + 1);
                    action_taken = true;
                    break;
                }
            }
            if (action_taken) {
                apply_focus_remove(game, choice);
                return;
            }

            Rectangle cancel_btn = { GetScreenWidth() - 180.0f, GetScreenHeight() - 70.0f, 160, 50 };
            if (CheckCollisionPointRec(GetMousePosition(), cancel_btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                game->current_state = GAME_STATE_HUMAN_TURN;
                game->message = "Your Turn!";
            }
            break;
        }
        case GAME_STATE_AWAITING_BASIC_FOR_SKILL: {
            player* human = &game->inner_game.players[0];
            const Card* pending_skill_card = get_card_info(human->hand.array[game->pending_skill_card_index]);
            if (!pending_skill_card) break;

            CardType required_type;
            switch (pending_skill_card->id % 10) {
                case 1: required_type = ATTACK; break;
                case 2: required_type = DEFENSE; break;
                case 3: required_type = MOVE; break;
                default: required_type = GENERIC; break;
            }

            if (required_type == ATTACK) game->message = "Select an Attack card to use.";
            else if (required_type == DEFENSE) game->message = "Select a Defense card to use.";
            else if (required_type == MOVE) game->message = "Select a Move card to use.";

            int scaled_card_width = CARD_WIDTH * HAND_SCALE;
            int scaled_card_height = CARD_HEIGHT * HAND_SCALE;
            int spacing = 15 * HAND_SCALE;

            int hand_width = human->hand.SIZE * (scaled_card_width + spacing) - spacing;
            float hand_start_x = (GetScreenWidth() - hand_width) / 2.0f;
            float hand_y = GetScreenHeight() - scaled_card_height - 20;

            for (uint32_t i = 0; i < human->hand.SIZE; ++i) {
                if ((int)i == game->pending_skill_card_index) continue;
                const Card* card_to_check = get_card_info(human->hand.array[i]);
                if (!card_to_check) continue;

                if (card_to_check->type == required_type) {
                    Rectangle card_bounds = { hand_start_x + i * (scaled_card_width + spacing), hand_y, scaled_card_width, scaled_card_height };
                    if (CheckCollisionPointRec(GetMousePosition(), card_bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        resolve_skill_and_basic(game, game->pending_skill_card_index, i);
                        return;
                    }
                }
            }
            
            Rectangle cancel_btn = { GetScreenWidth() / 2.0f - 100, GetScreenHeight() / 2.0f + 50, 200, 50 };
            if (CheckCollisionPointRec(GetMousePosition(), cancel_btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                game->current_state = GAME_STATE_HUMAN_TURN;
                game->message = "Your Turn!";
                game->pending_skill_card_index = -1;
            }
            break;
        }
        case GAME_STATE_BOT_TURN: {
            game->bot_action_timer -= GetFrameTime();
            if (game->bot_action_timer <= 0) {
                int card_to_play = get_bot_action(&game->inner_game);
                if (card_to_play != -1) {
                    // Bot AI currently does not support the new skill mechanic.
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
        case GAME_STATE_SLEEPING_BEAUTY_CHOOSE_HP_COST: {
            const player* human = &game->inner_game.players[0];
            const Card* skill_card = get_card_info(human->hand.array[game->pending_skill_card_index]);
            if (!skill_card) { // 安全檢查
                game->current_state = GAME_STATE_HUMAN_TURN;
                break;
            }
            int max_hp_cost = skill_card->level;
            
            for (int i = 0; i <= max_hp_cost; i++) {
                Rectangle btn_bounds = { GetScreenWidth() / 2.0f - (max_hp_cost + 1) * 35.0f + i * 70.0f, GetScreenHeight() / 2.0f + 20, 60, 40 };
                 if (CheckCollisionPointRec(GetMousePosition(), btn_bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    resolve_sleeping_beauty_attack(game, i);
                    return;
                }
            }
            Rectangle cancel_btn = { GetScreenWidth() / 2.0f - 50, GetScreenHeight() / 2.0f + 80, 100, 40 };
            if (CheckCollisionPointRec(GetMousePosition(), cancel_btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                game->current_state = GAME_STATE_HUMAN_TURN;
                game->message = "Your Turn!";
                game->pending_skill_card_index = -1;
                game->pending_basic_card_index = -1;
            }
            break;
        }
        case GAME_STATE_SLEEPING_BEAUTY_CHOOSE_AWAKEN_COST:{
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                const player* p = &game->inner_game.players[game->inner_game.now_turn_player_id];
                const Card* skill_card = get_card_info(p->hand.array[game->pending_skill_card_index]);
                if (!skill_card) return;

                int max_cost = (p->sleepingBeauty.AWAKEN == 1) ? 3 : 0;
                if (p->sleepingBeauty.AWAKEN_TOKEN < (uint32_t)max_cost) {
                    max_cost = p->sleepingBeauty.AWAKEN_TOKEN;
                }

                float button_width = 100, button_height = 50, padding = 10;
                float total_width = (max_cost + 1) * (button_width + padding) - padding;
                float start_x = (GetScreenWidth() - total_width) / 2.0f;
                float button_y = GetScreenHeight() / 2.0f;

                for (int i = 0; i <= max_cost; ++i) {
                    Rectangle button_bounds = {start_x + i * (button_width + padding), button_y, button_width, button_height};
                    if (CheckCollisionPointRec(GetMousePosition(), button_bounds)) {
                        resolve_sleeping_beauty_defense(game, i);
                        break;
                    }
                }
            }
            break;
        }
        case GAME_STATE_SLEEPING_BEAUTY_CHOOSE_MOVE_AWAKEN_COST: {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                const player* p = &game->inner_game.players[game->inner_game.now_turn_player_id];
                int max_cost = 3;
                if (p->sleepingBeauty.AWAKEN_TOKEN < 3) {
                    max_cost = p->sleepingBeauty.AWAKEN_TOKEN;
                }
                float button_width = 100, button_height = 50, padding = 10;
                float total_width = (max_cost + 1) * (button_width + padding) - padding;
                float start_x = (GetScreenWidth() - total_width) / 2.0f;
                float button_y = GetScreenHeight() / 2.0f;
                for (int i = 0; i <= max_cost; ++i) {
                    Rectangle button_bounds = {start_x + i * (button_width + padding), button_y, button_width, button_height};
                    if (CheckCollisionPointRec(GetMousePosition(), button_bounds)) {
                        resolve_sleeping_beauty_move(game, i);
                        break;
                    }
                }
            }
            break;
        }
        default:
            break;
    }

    if (game->current_state != GAME_STATE_CHOOSE_CHAR && game->current_state != GAME_STATE_GAME_OVER) {
        Rectangle exit_btn = { 20, 20, 100, 40 };
        if (CheckCollisionPointRec(GetMousePosition(), exit_btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            InitGame(game);
            return;
        }
    }
    
    if (game->current_state != GAME_STATE_CHOOSE_CHAR && game->current_state != GAME_STATE_GAME_OVER) {
        if (game->inner_game.players[0].life <= 0) { game->message = "You Lose!"; game->current_state = GAME_STATE_GAME_OVER; }
        if (game->inner_game.players[1].life <= 0) { game->message = "You Win!"; game->current_state = GAME_STATE_GAME_OVER; }
    }
}

void UpdatePVPGame(Game* game, bool* should_close) {
    if (WindowShouldClose()) {
        *should_close = true;
        return;
    }

    switch (game->current_state) {
        case GAME_STATE_PVP_CHOOSE_CHAR_P1: {
            game->message = "Player 1: Select Your Hero";
            for(int i = 0; i < 10; i++) {
                int row = i / 5;
                float extra_y = (row == 1) ? 40.0f : 0.0f;
                Rectangle btn_bounds = { 200.0f + (i % 5) * 180.0f, 280.0f + row * 200.0f + extra_y, 160, 80 };
                
                if (CheckCollisionPointRec(GetMousePosition(), btn_bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    init_player_deck(&game->inner_game.players[0], (CharacterType)i);
                    game->p1_selected_char = i;
                    game->current_state = GAME_STATE_PVP_CHOOSE_CHAR_P2;
                    return;
                }
            }
            break;
        }
        case GAME_STATE_PVP_CHOOSE_CHAR_P2: {
            game->message = "Player 2: Select Your Hero";
            for(int i = 0; i < 10; i++) {
                if (i == game->p1_selected_char) continue;

                int row = i / 5;
                float extra_y = (row == 1) ? 40.0f : 0.0f;
                Rectangle btn_bounds = { 200.0f + (i % 5) * 180.0f, 280.0f + row * 200.0f + extra_y, 160, 80 };

                if (CheckCollisionPointRec(GetMousePosition(), btn_bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    init_player_deck(&game->inner_game.players[1], (CharacterType)i);
                    initialize_shop_skill_piles(game);
                    game->inner_game.players[0].locate[0] = 6;
                    game->inner_game.players[1].locate[0] = 4;
                    
                    player* p0 = &game->inner_game.players[0];
                    while(p0->hand.SIZE < 4) { draw_card(p0); }
                    player* p1 = &game->inner_game.players[1];
                    while(p1->hand.SIZE < 6) { draw_card(p1); }
                    
                    start_turn_pvp(game);
                    return;
                }
            }
            break;
        }

        case GAME_STATE_PLAYER_1_TURN:
        case GAME_STATE_PLAYER_2_TURN: {
            int current_player_id = game->inner_game.now_turn_player_id;
            player* current_player = &game->inner_game.players[current_player_id];

            Rectangle focus_btn = { GetScreenWidth() - 200.0f, GetScreenHeight() - 120.0f, 180, 50 };
            if (CheckCollisionPointRec(GetMousePosition(), focus_btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (!game->player_has_acted) {
                    game->current_state = GAME_STATE_FOCUS_REMOVE;
                    game->message = "Focus: Remove a card";
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
            
            Rectangle end_turn_btn = { GetScreenWidth() - 200.0f, GetScreenHeight() - 60.0f, 180, 50 };
            if (CheckCollisionPointRec(GetMousePosition(), end_turn_btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                end_turn_pvp(game);
                return;
            }

            int scaled_card_width = CARD_WIDTH * HAND_SCALE;
            int scaled_card_height = CARD_HEIGHT * HAND_SCALE;
            int spacing = 15 * HAND_SCALE;
            int hand_width = current_player->hand.SIZE * (scaled_card_width + spacing) - spacing;
            float hand_start_x = (GetScreenWidth() - hand_width) / 2.0f;
            float hand_y = GetScreenHeight() - scaled_card_height - 20;

            for (uint32_t i = 0; i < current_player->hand.SIZE; ++i) {
                Rectangle card_bounds = { hand_start_x + i * (scaled_card_width + spacing), hand_y, scaled_card_width, scaled_card_height };
                if (CheckCollisionPointRec(GetMousePosition(), card_bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    apply_card_effect(game, i);
                    break;
                }
            }
            break;
        }

        case GAME_STATE_SHOP: {
            int current_player_id = game->inner_game.now_turn_player_id;
            Rectangle basic_tab = { GetScreenWidth() / 2.0f - 210, 150, 200, 40 };
            Rectangle skill_tab = { GetScreenWidth() / 2.0f + 10, 150, 200, 40 };
            if (CheckCollisionPointRec(GetMousePosition(), basic_tab) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                game->shop_page = 0;
            }
            if (CheckCollisionPointRec(GetMousePosition(), skill_tab) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                game->shop_page = 1;
            }

            if (game->shop_page == 0) { 
                float column_width = 300; 
                float startX = (GetScreenWidth() - (3 * column_width)) / 2.0f;
                float top_bound = 210, bottom_bound = GetScreenHeight() - 90;
                float total_area_height = bottom_bound - top_bound;
                float row_spacing = total_area_height / 3.0f;
                for (int type = 0; type < 3; type++) {
                    float row_center_y = top_bound + (row_spacing * type) + (row_spacing / 2.0f);
                    float card_start_y = row_center_y - (CARD_HEIGHT / 2.0f);
                    for (int level = 0; level < 3; level++) {
                        const vector* pile = &game->shop_piles[current_player_id][type][level];
                        if (pile->SIZE > 0) {
                            const Card* card = get_card_info(pile->array[0]);
                            if(card) {
                                float card_start_x = startX + (level * column_width);
                                Rectangle bounds = {card_start_x, card_start_y, CARD_WIDTH, CARD_HEIGHT};
                                if (CheckCollisionPointRec(GetMousePosition(), bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                                    apply_buy_card(game, card->id);
                                    return;
                                }
                            }
                        }
                    }
                }
            } else { 
                int chara = game->inner_game.players[current_player_id].character;
                for (int type = 0; type < 3; ++type) {
                    const vector* pile = &game->shop_skill_piles[chara][type];
                    if (!pile || pile->SIZE == 0) continue;
                    for (uint32_t i = 0; i < pile->SIZE; ++i) {
                        const Card* card = get_card_info(pile->array[i]);
                        if (!card) continue;
                        float x = 200 + (i % 5) * (CARD_WIDTH + 30);
                        float y = 250 + type * 250 + (i / 5) * (CARD_HEIGHT + 20);
                        Rectangle bounds = { x, y, CARD_WIDTH, CARD_HEIGHT };
                        if (CheckCollisionPointRec(GetMousePosition(), bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                            apply_buy_card(game, card->id);
                            return;
                        }
                    }
                }
            }
            Rectangle close_btn = { GetScreenWidth() - 160, GetScreenHeight() - 70, 140, 50 };
            if(CheckCollisionPointRec(GetMousePosition(), close_btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                if (game->inner_game.now_turn_player_id == 0) {
                    game->current_state = GAME_STATE_PLAYER_1_TURN;
                } else {
                    game->current_state = GAME_STATE_PLAYER_2_TURN;
                }
            }
            break;
        }
        case GAME_STATE_FOCUS_REMOVE: {
            player* p = &game->inner_game.players[game->inner_game.now_turn_player_id];
            int choice = 0;
            bool action_taken = false;

            for (uint32_t i = 0; i < p->hand.SIZE; i++) {
                Rectangle card_bounds = { 50 + i * (CARD_WIDTH + 15), 180, CARD_WIDTH, CARD_HEIGHT };
                if (CheckCollisionPointRec(GetMousePosition(), card_bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    choice = i + 1;
                    action_taken = true;
                    break;
                }
            }
            if (action_taken) {
                 if (choice > 0) eraseVector(&p->hand, choice - 1);
                 p->energy = 0; 
                 game->inner_game.now_turn_player_id = (game->inner_game.now_turn_player_id + 1) % 2;
                 start_turn_pvp(game);
                 return;
            }

            for (uint32_t i = 0; i < p->graveyard.SIZE; i++) {
                Rectangle card_bounds = { 50 + i * (CARD_WIDTH + 15), 430, CARD_WIDTH, CARD_HEIGHT };
                if (CheckCollisionPointRec(GetMousePosition(), card_bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    choice = -(int)(i + 1);
                    action_taken = true;
                    break;
                }
            }
            if (action_taken) {
                 if (choice < 0) eraseVector(&p->graveyard, -choice - 1);
                 p->energy = 0;
                 game->inner_game.now_turn_player_id = (game->inner_game.now_turn_player_id + 1) % 2;
                 start_turn_pvp(game);
                 return;
            }

            Rectangle cancel_btn = { GetScreenWidth() - 180.0f, GetScreenHeight() - 70.0f, 160, 50 };
            if (CheckCollisionPointRec(GetMousePosition(), cancel_btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                 if (game->inner_game.now_turn_player_id == 0) {
                    game->current_state = GAME_STATE_PLAYER_1_TURN;
                } else {
                    game->current_state = GAME_STATE_PLAYER_2_TURN;
                }
            }
            break;
        }
        
        case GAME_STATE_CHOOSE_MOVE_DIRECTION: {
            Rectangle leftBtn = {480, 350, 120, 50};
            Rectangle rightBtn = {680, 350, 120, 50};
            if (CheckCollisionPointRec(GetMousePosition(), leftBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                apply_movement(game, -1);
                if (game->inner_game.now_turn_player_id == 0) {
                    game->current_state = GAME_STATE_PLAYER_1_TURN;
                } else {
                    game->current_state = GAME_STATE_PLAYER_2_TURN;
                }
            }
            if (CheckCollisionPointRec(GetMousePosition(), rightBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                apply_movement(game, 1);
                if (game->inner_game.now_turn_player_id == 0) {
                    game->current_state = GAME_STATE_PLAYER_1_TURN;
                } else {
                    game->current_state = GAME_STATE_PLAYER_2_TURN;
                }
            }
            break;
        }

        case GAME_STATE_GAME_OVER: {
             Rectangle back_btn = { (float)GetScreenWidth()/2 - 125, (float)GetScreenHeight()/2 + 40, 250, 50 };
            if (CheckCollisionPointRec(GetMousePosition(), back_btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                InitGame(game);
                game->current_state = GAME_STATE_PVP_CHOOSE_CHAR_P1;
            }
            break;
        }

        default:
            break;
    }
    
    if (game->current_state != GAME_STATE_PVP_CHOOSE_CHAR_P1 && game->current_state != GAME_STATE_PVP_CHOOSE_CHAR_P2) {
        if (game->inner_game.players[0].life <= 0) { game->message = "Player 2 Wins!"; game->current_state = GAME_STATE_GAME_OVER; } 
        else if (game->inner_game.players[1].life <= 0) { game->message = "Player 1 Wins!"; game->current_state = GAME_STATE_GAME_OVER; }
    }
}

void end_turn_pvp(Game* game) {
    int player_id = game->inner_game.now_turn_player_id;
    player* p = &game->inner_game.players[player_id];

    game->thorns_attacks_left[player_id] = 0;
    game->thorns_damage_bonus[player_id] = 0;

    for (uint32_t i = 0; i < p->hand.SIZE; i++) {
        int32_t card_id = p->hand.array[i];
        const Card* card = get_card_info(card_id);
        if (card && card->type == STATUS) {
            apply_poison_damage(p, card);
        }
        pushbackVector(&p->graveyard, card_id);
    }
    clearVector(&p->hand);
    
    p->energy = 0;
    
    uint32_t total_cards = p->deck.SIZE + p->graveyard.SIZE;
    int target_hand_size = (p->character == SNOW_WHITE) ? 4 : 6;
    if (total_cards < (uint32_t)target_hand_size) {
        target_hand_size = total_cards;
    }
    
    while(p->hand.SIZE < (uint32_t)target_hand_size) {
        draw_card(p);
    }
    
    game->inner_game.now_turn_player_id = (player_id + 1) % 2;
    start_turn_pvp(game);
}

void start_turn_pvp(Game* game) {
    game->turn_count++;
    int current_player_id = game->inner_game.now_turn_player_id;
    player* p_current = &game->inner_game.players[current_player_id];
    
    if (game->pending_retaliation_level[current_player_id] > 0 && p_current->defense > 0) {
        player* p_opponent = &game->inner_game.players[(current_player_id + 1) % 2];
        int level = game->pending_retaliation_level[current_player_id];
        int damage = 0, range = 0;
        switch(level) {
            case 1: damage = 2; range = 1; break;
            case 2: damage = 4; range = 2; break;
            case 3: damage = 6; range = 3; break;
        }
        if (damage > 0 && abs(p_current->locate[0] - p_opponent->locate[0]) <= range) {
            apply_damage(p_current, p_opponent, damage);
        }
    }
    
    game->pending_retaliation_level[current_player_id] = 0;
    p_current->defense = 0;
    game->player_has_acted = false;
    
    if (current_player_id == 0) {
        game->current_state = GAME_STATE_PLAYER_1_TURN;
        game->message = "Player 1's Turn!";
    } else {
        game->current_state = GAME_STATE_PLAYER_2_TURN;
        game->message = "Player 2's Turn!";
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
    game->message = "You moved!";
}

void init_player_deck(player* p, CharacterType character) {
    p->character = character; 
    p->deck = initVector(); 
    p->hand = initVector(); 
    p->graveyard = initVector(); 
    switch (character) {
        case 0: 
            p->maxlife = 30;
            p->maxdefense = 6;
            p->specialGate = 15;
            break;
        case 1: 
            p->maxlife = 34;
            p->maxdefense = 6;
            p->specialGate = 17;
            break;
        case 2: 
            p->maxlife = 42;
            p->maxdefense = 6;
            p->specialGate = 21;
            break;
        case 3: 
            p->maxlife = 32;
            p->maxdefense = 6;
            p->specialGate = 16;
            break;
        case 4: 
            p->maxlife = 34;
            p->maxdefense = 3;
            p->specialGate = 17;
            break;
        case 5: 
            p->maxlife = 32;
            p->maxdefense = 6;
            p->specialGate = 16;
            break;
        case 6: 
            p->maxlife = 36;
            p->maxdefense = 3;
            p->specialGate = 18;
            break;
        case 7: 
            p->maxlife = 36;
            p->maxdefense = 6;
            p->specialGate = 18;
            break;
        case 8: 
            p->maxlife = 40;
            p->maxdefense = 6;
            p->specialGate = 20;
            break;
        case 9: 
            p->maxlife = 36;
            p->maxdefense = 6;
            p->specialGate = 18;
            break;
    }

    p->life = p->maxlife;
    p->defense = 0;
    p->energy = 0;
    p->snowWhite.remindPosion = initVector();
    p->sleepingBeauty.AWAKEN_TOKEN = 0;
    p->sleepingBeauty.AWAKEN = 0;
    
    for(int k=0; k<3; ++k) pushbackVector(&p->deck, 101);
    for(int k=0; k<3; ++k) pushbackVector(&p->deck, 201);
    for(int k=0; k<3; ++k) pushbackVector(&p->deck, 301);

    switch(character) {
        case RED_HOOD:
            pushbackVector(&p->deck, 501);
            pushbackVector(&p->deck, 502);
            pushbackVector(&p->deck, 503);
            break;
        case SNOW_WHITE:
            pushbackVector(&p->deck, 511);
            pushbackVector(&p->deck, 512);
            pushbackVector(&p->deck, 513);

            for (int i = 0; i < 6; ++i) {
                pushbackVector(&p->snowWhite.remindPosion, 913);
            }
            for (int i = 0; i < 6; ++i) {
                pushbackVector(&p->snowWhite.remindPosion, 912);
            }
            for (int i = 0; i < 6; ++i) {
                pushbackVector(&p->snowWhite.remindPosion, 911);
            }
            break;
        case SLEEPING_BEAUTY:
            pushbackVector(&p->deck, 521);
            pushbackVector(&p->deck, 522);
            pushbackVector(&p->deck, 523);
            break;
        case ALICE:
            pushbackVector(&p->deck, 531);
            pushbackVector(&p->deck, 532);
            pushbackVector(&p->deck, 533);
            break;
        case MULAN:
            pushbackVector(&p->deck, 541);
            pushbackVector(&p->deck, 542);
            pushbackVector(&p->deck, 543);
            break;
        case KAGUYA:
            pushbackVector(&p->deck, 551);
            pushbackVector(&p->deck, 552);
            pushbackVector(&p->deck, 553);
            break;
        case MERMAID:
            pushbackVector(&p->deck, 561);
            pushbackVector(&p->deck, 562);
            pushbackVector(&p->deck, 563);
            break;
        case MATCH_GIRL:
            pushbackVector(&p->deck, 571);
            pushbackVector(&p->deck, 572);
            pushbackVector(&p->deck, 573);
            break;
        case DOROTHY:
            pushbackVector(&p->deck, 581);
            pushbackVector(&p->deck, 582);
            pushbackVector(&p->deck, 583);
            break;
        case SCHEHERAZADE:
            pushbackVector(&p->deck, 591);
            pushbackVector(&p->deck, 592);
            pushbackVector(&p->deck, 593);
            break;
    }
    shuffle_deck(&p->deck);
}

void start_turn(Game* game) {
    game->turn_count++;
    int current_player_id = game->inner_game.now_turn_player_id;
    player* p_current = &game->inner_game.players[current_player_id];
    
    if (game->cacheA_card_id != -1) {
        pushbackVector(&game->inner_game.players[0].hand, game->cacheA_card_id);
        game->cacheA_card_id = -1;
    }
    if (game->pending_retaliation_level[current_player_id] > 0 && p_current->defense > 0) {
        player* p_opponent = &game->inner_game.players[(current_player_id + 1) % 2];
        int level = game->pending_retaliation_level[current_player_id];
        int damage = 0;
        int range = 0;

        switch(level) {
            case 1: damage = 2; range = 1; break;
            case 2: damage = 4; range = 2; break;
            case 3: damage = 6; range = 3; break;
        }

        if (damage > 0) {
            int distance = abs(p_current->locate[0] - p_opponent->locate[0]);
            if (distance <= range) {
                apply_damage(p_current, p_opponent, damage);
            }
        }
    }
    
    game->pending_retaliation_level[current_player_id] = 0;
    p_current->defense = 0;
    
    game->player_has_acted = false;
    
    if (current_player_id == 0) {
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
    game->turn_count = 0;
    game->pending_skill_card_index = -1;
    game->pending_basic_card_index = -1;
    game->shop_page = 0;
    
    game->pending_retaliation_level[0] = 0;
    game->pending_retaliation_level[1] = 0;

    game->thorns_attacks_left[0] = 0;
    game->thorns_attacks_left[1] = 0;
    game->thorns_damage_bonus[0] = 0;
    game->thorns_damage_bonus[1] = 0;

    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 3; ++j) {
            game->shop_skill_piles[i][j] = initVector();
        }
    }
    
    for (int p_idx = 0; p_idx < 2; p_idx++) {
        for(int i=0; i<3; ++i) for(int j=0; j<3; ++j) game->shop_piles[p_idx][i][j] = initVector();

        for(int i=0; i<6; ++i) {
            pushbackVector(&game->shop_piles[p_idx][0][0], 101); 
            pushbackVector(&game->shop_piles[p_idx][1][0], 201); 
            pushbackVector(&game->shop_piles[p_idx][2][0], 301); 
        }
        for(int i=0; i<9; ++i) {
            pushbackVector(&game->shop_piles[p_idx][0][1], 102);
            pushbackVector(&game->shop_piles[p_idx][1][1], 202);
            pushbackVector(&game->shop_piles[p_idx][2][1], 302);
        }
        for(int i=0; i<9; ++i) {
            pushbackVector(&game->shop_piles[p_idx][0][2], 103);
            pushbackVector(&game->shop_piles[p_idx][1][2], 203);
            pushbackVector(&game->shop_piles[p_idx][2][2], 303);
        }
    }

    for (int i = 0; i < 2; ++i) {
        pushbackVector(&game->shop_skill_piles[RED_HOOD][0], 601);
        pushbackVector(&game->shop_skill_piles[RED_HOOD][1], 602);
        pushbackVector(&game->shop_skill_piles[RED_HOOD][2], 603);
    }
    for (int i = 0; i < 3; ++i) {
        pushbackVector(&game->shop_skill_piles[RED_HOOD][0], 701);
        pushbackVector(&game->shop_skill_piles[RED_HOOD][1], 702);
        pushbackVector(&game->shop_skill_piles[RED_HOOD][2], 703);
    }
    
    game->cacheA_enabled = false;
    game->cacheA_card_id = -1;
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

    if (card_hand_index < 0 || card_hand_index >= (int)attacker->hand.SIZE) return;
    const Card* card = get_card_info(attacker->hand.array[card_hand_index]);
    if (!card) return;

    CardType type = card->type;

    if (type == SKILL) {
        game->pending_skill_card_index = card_hand_index;
        game->current_state = GAME_STATE_AWAITING_BASIC_FOR_SKILL;
        return;
    }

    int32_t played_card_id = card->id;
    
    if (type == STATUS) {
        attacker->energy += 1;
        apply_poison_damage(attacker, card);
        game->message = TextFormat("Discarded %s, took damage, gained 1 energy.", card->name);
    }
    else if (type == ATTACK) {
        player* defender = &game->inner_game.players[(player_id + 1) % 2];
        
        int distance = abs(attacker->locate[0] - defender->locate[0]);
        int range = 1;

        if (distance <= range) {
            int final_damage = card->value;
            if (attacker->character == SLEEPING_BEAUTY && game->thorns_attacks_left[player_id] > 0) {
                final_damage += game->thorns_damage_bonus[player_id];
                game->thorns_attacks_left[player_id]--;
                game->message = TextFormat("Thorns bonus! +%d damage", game->thorns_damage_bonus[player_id]);
            }
            apply_damage(attacker, defender, final_damage);
        } else {
            game->message = "Out of range!";
        }
        attacker->energy += card->value;
    } 
    else if (type == DEFENSE || type == GENERIC) {
        attacker->energy += card->value;
        if (type == DEFENSE) {
            attacker->defense += card->value;
        }
    } 
    else if (type == MOVE) {
        attacker->energy += card->value;
        game->pending_move_distance = card->value;
        game->current_state = GAME_STATE_CHOOSE_MOVE_DIRECTION;
        game->message = "Choose a direction to move";
    }

    game->player_has_acted = true;
    
    pushbackVector(&attacker->graveyard, played_card_id);
    eraseVector(&attacker->hand, card_hand_index);
}

void apply_buy_card(Game* game, int card_id) {
    int current_player_id = game->inner_game.now_turn_player_id;
    player* buyer = &game->inner_game.players[current_player_id];
    
    const Card* card_to_buy = get_card_info(card_id);
    if (!card_to_buy) return;

    if (buyer->energy < card_to_buy->cost) {
        game->message = "Not enough energy!";
        return;
    }

    bool found_in_supply = false;
    
    if (card_to_buy->type == SKILL) {
        int chara = buyer->character;
        for (int type = 0; type < 3; ++type) {
            vector* pile = &game->shop_skill_piles[chara][type];
             for (int i = pile->SIZE - 1; i >= 0; --i) {
                if (pile->array[i] == card_id) {
                    removeVectorAt(pile, i);
                    found_in_supply = true;
                    goto BUY_DONE;
                }
            }
        }
    } else {
        for (int type = 0; type < 3; ++type) {
            for (int lvl = 0; lvl < 3; ++lvl) {
                vector* pile = &game->shop_piles[current_player_id][type][lvl];
                if (pile->SIZE > 0 && pile->array[0] == card_id) {
                    popbackVector(pile);
                    found_in_supply = true;
                    goto BUY_DONE;
                }
            }
        }
    }
    
BUY_DONE:
    if (!found_in_supply) {
        game->message = "This card is sold out!";
        return;
    }

    buyer->energy -= card_to_buy->cost;
    pushbackVector(&buyer->graveyard, card_id);
    game->message = TextFormat("Bought %s!", card_to_buy->name);
    game->player_has_acted = true;
}

void apply_focus_remove(Game* game, int choice) {
    player* p = &game->inner_game.players[game->inner_game.now_turn_player_id];
    if (choice > 0 && choice <= (int)p->hand.SIZE) {
        eraseVector(&p->hand, choice - 1);
        game->message = "Card removed from hand.";
    } else if (choice < 0 && -choice <= (int)p->graveyard.SIZE) {
        eraseVector(&p->graveyard, -choice - 1);
        game->message = "Card removed from graveyard.";
    } else {
        game->message = "Invalid selection.";
    }
    end_turn(game);
}

void resolve_skill_and_basic(Game* game, int skill_idx, int basic_idx) {
    int player_id = game->inner_game.now_turn_player_id;
    player* attacker = &game->inner_game.players[player_id];
    player* defender = &game->inner_game.players[(player_id + 1) % 2];
    
    const Card* skill_card = get_card_info(attacker->hand.array[skill_idx]);
    const Card* basic_card = get_card_info(attacker->hand.array[basic_idx]);

    if (!skill_card || !basic_card) return;

    if (attacker->character == RED_HOOD && skill_card->type == SKILL) {
        if (skill_card->id % 10 == 1) { 
            int base_damage = skill_card->value;
            int bonus_damage = basic_card->value;
            int total_damage = base_damage + bonus_damage;
            
            apply_damage(attacker, defender, total_damage);
            game->message = TextFormat("Used %s! Dealt %d damage!", skill_card->name, total_damage);
        
        } else if (skill_card->id % 10 == 2) { 
            int defense_gain = basic_card->value;
            int total_damage = skill_card->value;
            attacker->defense += defense_gain;
            game->pending_retaliation_level[player_id] = skill_card->level;
            game->message = TextFormat("Gained %d defense! Retaliation set!", defense_gain);

            apply_damage(attacker, defender, skill_card->value);
            game->message = TextFormat("Used %s! Dealt %d damage!", skill_card->name, total_damage);

        } else if (skill_card->id % 10 == 3) { 
            apply_damage(attacker, defender, skill_card->value);
            
            int knockback_dist = basic_card->value;
            int direction = (defender->locate[0] > attacker->locate[0]) ? 1 : -1;
            
            for (int i = 0; i < knockback_dist; i++) {
                int next_pos = defender->locate[0] + direction;
                if (next_pos < 0 || next_pos > 10) break;
                if (next_pos == attacker->locate[0]) break;
                defender->locate[0] = next_pos;
            }
            game->message = TextFormat("Dealt %d damage & knocked back!", skill_card->value);
        }
    } else if (attacker->character == SNOW_WHITE && skill_card->type == SKILL) {
        if (skill_card->id % 10 == 1) { 
            int base_damage = skill_card->value;
            int bonus_damage = basic_card->value;
            int total_damage = base_damage + bonus_damage;
            
            apply_damage(attacker, defender, total_damage);

            int cards_to_discard = skill_card->level;
            for (int i = 0; i < cards_to_discard; ++i) {
                if (defender->deck.SIZE > 0) {
                    pushbackVector(&defender->graveyard, defender->deck.array[defender->deck.SIZE - 1]);
                    popbackVector(&defender->deck);
                } else {
                    break;
                }
            }

            game->message = TextFormat("Dealt %d damage, opponent discards %d!", total_damage, cards_to_discard);
        }else if (skill_card->id % 10 == 2) { 
            int damage = skill_card->level;
            
            apply_damage(attacker, defender, damage);

            int cards_to_move = basic_card->value;
            int cards_moved = 0;
            for (int i = 0; i < cards_to_move; ++i) {
                if (attacker->snowWhite.remindPosion.SIZE > 0) {
                    int poison_card_id = attacker->snowWhite.remindPosion.array[attacker->snowWhite.remindPosion.SIZE - 1];
                    pushbackVector(&defender->graveyard, poison_card_id);
                    popbackVector(&attacker->snowWhite.remindPosion);
                    cards_moved++;
                } else {
                    break;
                }
            }
            game->message = TextFormat("Used %s, dealing %d damage!", skill_card->name, damage);
        }else if (skill_card->id % 10 == 3) { 
            int damage = skill_card->level;
            int range_bonus = skill_card->level - 1;
            int total_range = basic_card->value + range_bonus;
            int distance = abs(attacker->locate[0] - defender->locate[0]);
            int move_dist = distance - 1;
            
            if (distance <= total_range) {
                apply_damage(attacker, defender, damage);
                game->message = TextFormat("Used %s, dealing %d damage!", skill_card->name, damage);
            } else {
                game->message = "Target out of range, skill failed!";
                game->current_state = GAME_STATE_HUMAN_TURN;
                game->pending_skill_card_index = -1;
                return;
            }

            if (move_dist > 0) {
                int direction = (defender->locate[0] > attacker->locate[0]) ? 1 : -1;
                for (int i = 0; i < move_dist; i++) {
                    int next_pos = attacker->locate[0] + direction;
                    if (next_pos == defender->locate[0]) break;
                    attacker->locate[0] = next_pos;
                }
                game->message = TextFormat("Moved %d space(s) towards opponent!", move_dist);
            }
        }

    } else if(attacker->character == SLEEPING_BEAUTY && skill_card->type == SKILL) {
        game->pending_skill_card_index = skill_idx;
        game->pending_basic_card_index = basic_idx;
        if (skill_card->id % 10 == 1) { 
            game->current_state = GAME_STATE_SLEEPING_BEAUTY_CHOOSE_HP_COST;
            game->message = "選擇要消耗多少生命值以造成額外傷害";
            return; 
        }else if (skill_card->id % 10 == 2) { 
            if (attacker->sleepingBeauty.AWAKEN == 1 && attacker->sleepingBeauty.AWAKEN_TOKEN > 0) {
                game->current_state = GAME_STATE_SLEEPING_BEAUTY_CHOOSE_AWAKEN_COST;
                game->message = "選擇要消耗多少覺醒值";
                return;
            } else {
                resolve_sleeping_beauty_defense(game, 0);
                return;
            }
            return; 
        }else if (skill_card->id % 10 == 3) {
            if (attacker->sleepingBeauty.AWAKEN == 1 && attacker->sleepingBeauty.AWAKEN_TOKEN > 0) {
                game->current_state = GAME_STATE_SLEEPING_BEAUTY_CHOOSE_MOVE_AWAKEN_COST;
                game->message = "選擇要消耗多少覺醒值來強化傷害";
                return;
            } else {
                resolve_sleeping_beauty_move(game, 0);
                return;
            }
        }
    }else {
        if (skill_card->value > 0) {
            int total_damage = skill_card->value;
            if (attacker->character == SLEEPING_BEAUTY && game->thorns_attacks_left[player_id] > 0) {
                total_damage += game->thorns_damage_bonus[player_id];
                game->thorns_attacks_left[player_id]--;
            }
            apply_damage(attacker, defender, skill_card->value);
        }
        game->message = TextFormat("Used %s!", skill_card->name);
    }

    int32_t skill_id = skill_card->id;
    int32_t basic_id = basic_card->id;
    if (skill_idx > basic_idx) {
        eraseVector(&attacker->hand, skill_idx);
        eraseVector(&attacker->hand, basic_idx);
    } else {
        eraseVector(&attacker->hand, basic_idx);
        eraseVector(&attacker->hand, skill_idx);
    }
    pushbackVector(&attacker->graveyard, skill_id);
    pushbackVector(&attacker->graveyard, basic_id);

    game->player_has_acted = true;
    game->pending_skill_card_index = -1;
    game->current_state = GAME_STATE_HUMAN_TURN;
    
    if (attacker->character == RED_HOOD &&
        (skill_card->type == SKILL) &&
        (attacker->life > 0)) {

        int type_digit = skill_card->id % 10;
        if (type_digit == 1 || type_digit == 3) { 

            bool has_601_in_shop = false;
            vector* atk_shop = &game->shop_skill_piles[RED_HOOD][0];

            for (uint32_t i = 0; i < atk_shop->SIZE; ++i) {
                if (atk_shop->array[i] == 601) {
                    has_601_in_shop = true;
                    break;
                }
            }

            if (!has_601_in_shop) {
                game->current_state = GAME_STATE_OVERLOAD_CONFIRM;
                return;
            }
        }
    }

    if (attacker->character == RED_HOOD &&
        skill_card->type == SKILL &&
        attacker->life > 0) {

        int type_digit = skill_card->id % 10;
        if (type_digit == 1 || type_digit == 3) {
            bool no_lv3_left = true;
            vector* atk_shop = &game->shop_skill_piles[RED_HOOD][0];
            for (uint32_t i = 0; i < atk_shop->SIZE; ++i) {
                if (atk_shop->array[i] >= 700 && atk_shop->array[i] < 800) {
                    no_lv3_left = false;
                    break;
                }
            }
            if (no_lv3_left) {
                game->cacheA_enabled = true;
            }
        }
    }
}

void initialize_shop_skill_piles(Game* game) {
    for (int chara = 0; chara < 10; ++chara) {
        for (int type = 0; type < 3; ++type) {
            vector* pile = &game->shop_skill_piles[chara][type];
            clearVector(pile);

            int lv2 = 600 + chara * 10 + (type + 1);
            int lv3 = 700 + chara * 10 + (type + 1);

            pushbackVector(pile, lv2);
            pushbackVector(pile, lv2);
            pushbackVector(pile, lv3);
            pushbackVector(pile, lv3);
            pushbackVector(pile, lv3);
        }
    }
}

void apply_poison_damage(player* p, const Card* card) {
    if (!p || !card || card->type != STATUS) return;

    int damage = 0;
    if (card->id == 912) damage = 1;
    else if (card->id == 913) damage = 2;
    
    if (damage > 0) {
        apply_damage(NULL, p, damage);
    }
}

int apply_damage(player* attacker, player* defender, int base_damage) {
    if (!defender || defender->life <= 0) {
        return 0;
    }

    int life_before_damage = defender->life;
    int damage_to_apply = (base_damage < 0) ? 0 : base_damage;

    int absorbed_by_defense = (defender->defense >= damage_to_apply) ? damage_to_apply : defender->defense;
    defender->defense -= absorbed_by_defense;

    int remaining_damage = damage_to_apply - absorbed_by_defense;
    if (remaining_damage > 0) {
        if (defender->life <= remaining_damage) {
            defender->life = 0;
        } else {
            defender->life -= remaining_damage;
        }
    }

    int life_lost = life_before_damage - defender->life;
    if (life_lost > 0 && defender->character == SLEEPING_BEAUTY && defender->sleepingBeauty.AWAKEN == 0) {
        defender->sleepingBeauty.AWAKEN_TOKEN += life_lost;
        printf("DEBUG: Sleeping Beauty lost %d life and gained %d Awaken Tokens. Total: %d\n", 
               life_lost, life_lost, defender->sleepingBeauty.AWAKEN_TOKEN);

        if (defender->sleepingBeauty.AWAKEN == 0 && defender->sleepingBeauty.AWAKEN_TOKEN >= 6) {
            defender->sleepingBeauty.AWAKEN = 1;
            printf("DEBUG: Sleeping Beauty has AWAKENED!\n");
        }
    }

    printf("DEBUG: Damage applied. Attacker: %p, Defender: %p, Base Damage: %d, Final Life: %d\n", 
           (void*)attacker, (void*)defender, base_damage, defender->life);

    if (defender->life == 0) {
        printf("DEBUG: Defender has been defeated!\n");
    }

    return life_lost;
}

void resolve_sleeping_beauty_attack(Game* game, int chosen_hp_cost) {
    int player_id = game->inner_game.now_turn_player_id;
    player* attacker = &game->inner_game.players[player_id];
    player* defender = &game->inner_game.players[(player_id + 1) % 2];

    int skill_idx = game->pending_skill_card_index;
    int basic_idx = game->pending_basic_card_index;
    
    if (skill_idx < 0 || basic_idx < 0 || (uint32_t)skill_idx >= attacker->hand.SIZE || (uint32_t)basic_idx >= attacker->hand.SIZE) {
        game->current_state = GAME_STATE_HUMAN_TURN;
        game->message = "Error: Invalid card index.";
        return;
    }
    
    const Card* skill_card = get_card_info(attacker->hand.array[skill_idx]);
    const Card* basic_card = get_card_info(attacker->hand.array[basic_idx]);

    if (!skill_card || !basic_card) {
        game->current_state = GAME_STATE_HUMAN_TURN;
        return;
    }
    
    int base_damage = skill_card->level * basic_card->value;
    int health_cost = chosen_hp_cost;
    
    int life_lost_for_skill = 0;
    if (health_cost > 0) {
        int original_defense = attacker->defense;
        attacker->defense = 0;
        life_lost_for_skill = apply_damage(NULL, attacker, health_cost);
        attacker->defense = original_defense;
    }

    int extra_damage = life_lost_for_skill;
    int total_damage = base_damage + extra_damage;

    if (game->thorns_attacks_left[player_id] > 0) {
        total_damage += game->thorns_damage_bonus[player_id];
        game->thorns_attacks_left[player_id]--;
    }

    apply_damage(attacker, defender, total_damage);
    
    game->message = TextFormat("Lost %d HP for +%d bonus damage! Total: %d", life_lost_for_skill, extra_damage, total_damage);

    if (attacker->sleepingBeauty.AWAKEN_TOKEN == 0) {
        attacker->sleepingBeauty.AWAKEN = 0;
        printf("DEBUG: Sleeping Beauty returned to sleep state.\n");
    }

    int32_t skill_id = skill_card->id;
    int32_t basic_id = basic_card->id;
    if (skill_idx > basic_idx) {
        eraseVector(&attacker->hand, skill_idx);
        eraseVector(&attacker->hand, basic_idx);
    } else {
        eraseVector(&attacker->hand, basic_idx);
        eraseVector(&attacker->hand, skill_idx);
    }
    pushbackVector(&attacker->graveyard, skill_id);
    pushbackVector(&attacker->graveyard, basic_id);

    game->player_has_acted = true;
    game->pending_skill_card_index = -1;
    game->pending_basic_card_index = -1;
    game->current_state = GAME_STATE_HUMAN_TURN;
}

void resolve_sleeping_beauty_defense(Game* game, int chosen_awaken_cost) {
    int player_id = game->inner_game.now_turn_player_id;
    player* attacker = &game->inner_game.players[player_id];
    
    int skill_idx = game->pending_skill_card_index;
    int basic_idx = game->pending_basic_card_index;

    if (skill_idx < 0 || basic_idx < 0 || (uint32_t)skill_idx >= attacker->hand.SIZE || (uint32_t)basic_idx >= attacker->hand.SIZE) {
        game->current_state = GAME_STATE_HUMAN_TURN; return;
    }

    const Card* skill_card = get_card_info(attacker->hand.array[skill_idx]);
    const Card* basic_card = get_card_info(attacker->hand.array[basic_idx]);
    if (!skill_card || !basic_card) { game->current_state = GAME_STATE_HUMAN_TURN; return; }

    int bonus_level = skill_card->level;
    int num_attacks = basic_card->value;
    
    if (chosen_awaken_cost > 0) {
        attacker->sleepingBeauty.AWAKEN_TOKEN -= chosen_awaken_cost;
        num_attacks += chosen_awaken_cost;
        game->message = TextFormat("消耗%d覺醒值! 荊棘效果已強化!", chosen_awaken_cost);
    } else {
        game->message = "獲得荊棘效果!";
    }
    
    game->thorns_attacks_left[player_id] = num_attacks;
    game->thorns_damage_bonus[player_id] = bonus_level;

    int32_t skill_id = skill_card->id;
    int32_t basic_id = basic_card->id;
    if (skill_idx > basic_idx) { eraseVector(&attacker->hand, skill_idx); eraseVector(&attacker->hand, basic_idx); } 
    else { eraseVector(&attacker->hand, basic_idx); eraseVector(&attacker->hand, skill_idx); }
    pushbackVector(&attacker->graveyard, skill_id);
    pushbackVector(&attacker->graveyard, basic_id);

    game->player_has_acted = true;
    game->pending_skill_card_index = -1;
    game->pending_basic_card_index = -1;
    game->current_state = GAME_STATE_HUMAN_TURN;
}

void resolve_sleeping_beauty_move(Game* game, int chosen_awaken_cost) {
    int player_id = game->inner_game.now_turn_player_id;
    player* attacker = &game->inner_game.players[player_id];
    player* defender = &game->inner_game.players[(player_id + 1) % 2];
    
    int skill_idx = game->pending_skill_card_index;
    int basic_idx = game->pending_basic_card_index;
    if (skill_idx < 0 || basic_idx < 0) return;
    
    const Card* skill_card = get_card_info(attacker->hand.array[skill_idx]);
    const Card* basic_card = get_card_info(attacker->hand.array[basic_idx]);
    if (!skill_card || !basic_card) return;

    int range = skill_card->level + 1;
    int damage = basic_card->value;
    int distance = abs(attacker->locate[0] - defender->locate[0]);
    
    if (distance <= range) {
        if (chosen_awaken_cost > 0) {
            attacker->sleepingBeauty.AWAKEN_TOKEN -= chosen_awaken_cost;
            damage += chosen_awaken_cost;
        }
        
        if (game->thorns_attacks_left[player_id] > 0) {
            damage += game->thorns_damage_bonus[player_id];
            game->thorns_attacks_left[player_id]--;
        }
        
        apply_damage(attacker, defender, damage);
        
        int pull_distance = basic_card->value;
        int direction = (attacker->locate[0] > defender->locate[0]) ? 1 : -1;
        for (int i = 0; i < pull_distance; i++) {
            int next_pos = defender->locate[0] + direction;
            if (next_pos == attacker->locate[0]) break;
            defender->locate[0] = next_pos;
        }
        
        game->message = TextFormat("造成 %d 傷害並將對手拉近！", damage);
    } else {
        game->message = "目標超出範圍！";
    }

    int32_t skill_id = skill_card->id;
    int32_t basic_id = basic_card->id;
    if (skill_idx > basic_idx) { eraseVector(&attacker->hand, skill_idx); eraseVector(&attacker->hand, basic_idx); } 
    else { eraseVector(&attacker->hand, basic_idx); eraseVector(&attacker->hand, skill_idx); }
    pushbackVector(&attacker->graveyard, skill_id);
    pushbackVector(&attacker->graveyard, basic_id);

    game->player_has_acted = true;
    game->pending_skill_card_index = -1;
    game->pending_basic_card_index = -1;
    game->current_state = GAME_STATE_HUMAN_TURN;
}
