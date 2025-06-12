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


void end_turn(Game* game) {
    int player_id = game->inner_game.now_turn_player_id;
    player* p = &game->inner_game.players[player_id];
    
    // 1. 當前回合玩家棄置所有手牌
    for (uint32_t i = 0; i < p->hand.SIZE; i++) {
        pushbackVector(&p->graveyard, p->hand.array[i]);
    }
    clearVector(&p->hand);
    
    // 2. 重置能量和防禦
    p->energy = 0;
    p->defense = 0;
    
    // 3. [新防護機制] 計算總牌數並決定抽牌目標
    // 此時手牌已空，總牌數等於牌庫 + 棄牌堆
    uint32_t total_cards = p->deck.SIZE + p->graveyard.SIZE;
    int target_hand_size = 6;

    // 如果總牌數少於標準手牌上限，則只抽到總牌數的量
    if (total_cards < 6) {
        target_hand_size = total_cards;
    }
    
    // 4. 為下個回合抽出新的手牌
    while(p->hand.SIZE < (uint32_t)target_hand_size) {
        draw_card(p);
    }
    
    // 5. 交換玩家順序並開始新回合
    game->inner_game.now_turn_player_id = (player_id + 1) % 2;
    start_turn(game);
}

void UpdateGame(Game* game, bool* should_close) {
    switch (game->current_state) {
        case GAME_STATE_CHOOSE_CHAR: {
            for(int i = 0; i < 10; i++) {
                // [修正] 增加Y軸的間距 (120 -> 240)，以符合圖案加入後，第二排按鈕的實際位置
                Rectangle btn_bounds = {200.0f + (i % 5) * 180, 250.0f + (i / 5) * 240, 160, 80};
                
                if (CheckCollisionPointRec(GetMousePosition(), btn_bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    init_player_deck(&game->inner_game.players[0], (CharacterType)i);
                    init_player_deck(&game->inner_game.players[1], (CharacterType)(rand() % 10));
                    
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

            int hand_width = human->hand.SIZE * (CARD_WIDTH + 15) - 15;
            float hand_start_x = (GetScreenWidth() - hand_width) / 2.0f;
            float hand_y = GetScreenHeight() - CARD_HEIGHT - 20;

            for (uint32_t i = 0; i < human->hand.SIZE; ++i) {
                Rectangle card_bounds = { hand_start_x + i * (CARD_WIDTH + 15), hand_y, CARD_WIDTH, CARD_HEIGHT };
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
                        if (skill_subtype == 1) {
                            if (distance > 2) {
                                can_play = false;
                                game->message = "No target in range!";
                            }
                        }
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
                        const vector* pile = &game->shop_piles[type][level];
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

            int hand_width = human->hand.SIZE * (CARD_WIDTH + 15) - 15;
            float hand_start_x = (GetScreenWidth() - hand_width) / 2.0f;
            float hand_y = GetScreenHeight() - CARD_HEIGHT - 20;

            for (uint32_t i = 0; i < human->hand.SIZE; ++i) {
                if ((int)i == game->pending_skill_card_index) continue;
                const Card* card_to_check = get_card_info(human->hand.array[i]);
                if (!card_to_check) continue;

                if (card_to_check->type == required_type) {
                    Rectangle card_bounds = { hand_start_x + i * (CARD_WIDTH + 15), hand_y, CARD_WIDTH, CARD_HEIGHT };
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
    }
    if (game->current_state != GAME_STATE_CHOOSE_CHAR && game->current_state != GAME_STATE_GAME_OVER) {
        if (game->inner_game.players[0].life <= 0) { game->message = "You Lose!"; game->current_state = GAME_STATE_GAME_OVER; }
        if (game->inner_game.players[1].life <= 0) { game->message = "You Win!"; game->current_state = GAME_STATE_GAME_OVER; }
    }
}



// init_player_deck 函式 - 保持不變
void init_player_deck(player* p, CharacterType character) {
    p->character = character; 
    p->deck = initVector(); 
    p->hand = initVector(); 
    p->graveyard = initVector(); 
    p->life = 20; 
    p->defense = 0; 
    p->energy = 0;
    
    // Add basic cards
    for(int k=0; k<3; ++k) pushbackVector(&p->deck, 101); // Attack LV1
    for(int k=0; k<3; ++k) pushbackVector(&p->deck, 201); // Defense LV1
    for(int k=0; k<3; ++k) pushbackVector(&p->deck, 301); // Move LV1

    // Add one of each type of skill card based on character
    switch(character) {
        case RED_HOOD:
            pushbackVector(&p->deck, 501); // ATK Skill
            pushbackVector(&p->deck, 502); // DEF Skill
            pushbackVector(&p->deck, 503); // MOV Skill
            break;
        case SNOW_WHITE:
            pushbackVector(&p->deck, 511);
            pushbackVector(&p->deck, 512);
            pushbackVector(&p->deck, 513);
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

// start_turn 函式 - 保持不變
void start_turn(Game* game) {
    game->turn_count++;
    int player_id = game->inner_game.now_turn_player_id;
    player* p = &game->inner_game.players[player_id];
    
    p->defense = 0;
    game->player_has_acted = false;
    
    if (player_id == 0) {
        game->current_state = GAME_STATE_HUMAN_TURN;
        game->message = "Your Turn!";
    } else {
        game->current_state = GAME_STATE_BOT_TURN;
        game->message = "Opponent's Turn...";
        game->bot_action_timer = 1.0f;
    }
}

// [修改] InitGame 函式，初始化商店頁面
void InitGame(Game* game) {
    srand(time(NULL));
    memset(game, 0, sizeof(Game));
    game->current_state = GAME_STATE_CHOOSE_CHAR;
    game->message = "Select Your Hero";
    game->turn_count = 0;
    game->pending_skill_card_index = -1;
    game->shop_page = 0; // [新] 預設為基礎牌商店頁面

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

// shuffle_deck 函式 - 保持不變
void shuffle_deck(vector* deck) {
    if (deck->SIZE < 2) return;
    for (uint32_t i = 0; i < deck->SIZE - 1; ++i) {
        uint32_t j = i + rand() % (deck->SIZE - i);
        int32_t temp = deck->array[i];
        deck->array[i] = deck->array[j];
        deck->array[j] = temp;
    }
}

// draw_card 函式 - 保持不變
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

// apply_movement 函式 - 保持不變
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

    if (card_hand_index < 0 || card_hand_index >= (int)attacker->hand.SIZE) return;
    const Card* card = get_card_info(attacker->hand.array[card_hand_index]);
    if (!card) return;

    CardType type = card->type;

    if (type == SKILL) {
        game->pending_skill_card_index = card_hand_index;
        game->current_state = GAME_STATE_AWAITING_BASIC_FOR_SKILL;
        // The message will be updated in the AWAITING_BASIC state
        return;
    }

    int32_t played_card_id = card->id;
    
    // [修改] 攻擊基礎牌的處理邏輯
    if (type == ATTACK) {
        player* defender = &game->inner_game.players[(player_id + 1) % 2];
        
        // 計算距離並檢查射程
        int distance = abs(attacker->locate[0] - defender->locate[0]);
        int range = 1; // 攻擊基礎牌射程為 1

        if (distance <= range) {
            // 在射程內，造成傷害
            int damage = card->value;
            if(defender->defense >= damage) {
                defender->defense -= damage;
            } else { 
                int damage_left = damage - defender->defense;
                defender->defense = 0;
                if (defender->life <= damage_left) defender->life = 0;
                else defender->life -= damage_left;
            }
        } else {
            // 不在射程內，不造成傷害
            game->message = "Out of range!";
        }
        attacker->energy += card->value; // 無論是否在射程內，都獲得能量
    } 
    // 防禦與泛用牌的處理邏輯
    else if (type == DEFENSE || type == GENERIC) {
        attacker->energy += card->value;
        if (type == DEFENSE) {
            attacker->defense += card->value;
        }
    } 
    // 移動牌的處理邏輯
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

// apply_buy_card 函式 - 保持不變
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

// apply_focus_remove 函式 - 保持不變
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
    player* attacker = &game->inner_game.players[0];
    player* defender = &game->inner_game.players[1];
    
    const Card* skill_card = get_card_info(attacker->hand.array[skill_idx]);
    const Card* basic_card = get_card_info(attacker->hand.array[basic_idx]);

    if (!skill_card || !basic_card) return;

    // 結算技能效果 (此處以造成傷害為例)
    if (skill_card->value > 0) {
        int damage = skill_card->value;
        if(defender->defense >= damage) {
            defender->defense -= damage;
        } else { 
            int damage_left = damage - defender->defense;
            defender->defense = 0;
            if(defender->life <= damage_left) defender->life = 0;
            else defender->life -= damage_left;
        }
    }

    // 將兩張牌都移至棄牌堆
    int32_t skill_id = skill_card->id;
    int32_t basic_id = basic_card->id;

    // 為了避免索引錯亂，永遠先移除索引較大的那張牌
    if (skill_idx > basic_idx) {
        eraseVector(&attacker->hand, skill_idx);
        eraseVector(&attacker->hand, basic_idx);
    } else {
        eraseVector(&attacker->hand, basic_idx);
        eraseVector(&attacker->hand, skill_idx);
    }

    pushbackVector(&attacker->graveyard, skill_id);
    pushbackVector(&attacker->graveyard, basic_id);

    // 更新遊戲狀態
    game->player_has_acted = true;
    game->current_state = GAME_STATE_HUMAN_TURN;
    game->message = TextFormat("Used %s!", skill_card->name);
    game->pending_skill_card_index = -1; // 清除待處理的技能牌
}
