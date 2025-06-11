#include "gui.h"
#include "definitions.h"
#include "raylib.h"

// 外部變數與函式
extern const char* character_names[];
extern Font font;
extern Texture2D backgroundTexture;

// 函式原型
void DrawShop(const Game* game);
void DrawFocusSelection(const Game* game);
void DrawBattleInterface(const Game* game);
void DrawGameBoard(const Game* game);
void DrawCharSelection(void);
void DrawPlayerInfo(const Game* game, bool is_human);
void DrawCard(const Card* card, Rectangle bounds, bool is_hovered, bool is_opponent_card);
void DrawSkillPairingOverlay(const Game* game);

// =================================================================
//                      繪製函式
// =================================================================

void DrawCard(const Card* card, Rectangle bounds, bool is_hovered, bool is_opponent_card) {
    if (is_opponent_card) {
        DrawRectangleRounded(bounds, 0.08f, 10, DARKBLUE);
        DrawRectangleRoundedLinesEx(bounds, 0.08f, 10, 4, BLUE);
        return;
    }
    if (is_hovered) {
        bounds.y -= 20;
    }
    DrawRectangleRounded(bounds, 0.08f, 10, RAYWHITE);
    DrawRectangleRoundedLinesEx(bounds, 0.08f, 10, is_hovered ? 5 : 3, is_hovered ? GOLD : BLACK);
    if (card) {
        DrawTextEx(font, card->name, (Vector2){ bounds.x + 10, bounds.y + 15 }, 18, 1, BLACK);
        
        // --- 卡牌類型標籤 ---
        if (card->type == ATTACK) {
            DrawTextEx(font, TextFormat("Attack: %d", card->value), (Vector2){ bounds.x + 15, bounds.y + 50 }, 16, 1, RED);
        } else if (card->type == DEFENSE) {
            DrawTextEx(font, TextFormat("Defense: %d", card->value), (Vector2){ bounds.x + 15, bounds.y + 50 }, 16, 1, DARKGREEN);
        } else if (card->type == MOVE) {
            DrawTextEx(font, TextFormat("Move: %d", card->value), (Vector2){ bounds.x + 15, bounds.y + 50 }, 16, 1, PURPLE);
        } else if (card->type == SKILL) {
            // [修改] 根據卡牌ID的尾數判斷技能類型
            const char* skill_type_text = "Skill";
            int subtype = card->id % 10;
            switch (subtype) {
                case 1: skill_type_text = "[ATK] Skill"; break;
                case 2: skill_type_text = "[DEF] Skill"; break;
                case 3: skill_type_text = "[MOV] Skill"; break;
            }
            DrawTextEx(font, skill_type_text, (Vector2){ bounds.x + 15, bounds.y + 50 }, 16, 1, BLUE);
        }
        
        // --- 能量獲取標籤 ---
        if (card->type == ATTACK || card->type == DEFENSE || card->type == MOVE || card->type == GENERIC) {
             DrawTextEx(font, TextFormat("Energy Gain: +%d", card->value), (Vector2){ bounds.x + 15, bounds.y + CARD_HEIGHT - 35 }, 14, 1, SKYBLUE);
        }
    }
}

// DrawPlayerInfo 函式 - 保持不變
void DrawPlayerInfo(const Game* game, bool is_human) {
    int player_idx = is_human ? 0 : 1;
    const player* p = &game->inner_game.players[player_idx];
    if(p->character >= 10) return;

    int x_pos = is_human ? 30 : GetScreenWidth() - 330;
    int y_pos = is_human ? GetScreenHeight() - 110 : 30;
    
    const char* role_name = character_names[p->character];
    
    Rectangle info_box = {(float)x_pos, (float)y_pos, 300, 80};
    DrawRectangleRec(info_box, Fade(BLACK, 0.7f));
    DrawRectangleLinesEx(info_box, 2, is_human ? SKYBLUE : RED);
    
    DrawTextEx(font, role_name, (Vector2){ (float)x_pos + 15, (float)y_pos + 10}, 24, 1, WHITE);
    DrawTextEx(font, TextFormat("HP: %d", p->life), (Vector2){ (float)x_pos + 15, (float)y_pos + 45 }, 20, 1, LIME);
    DrawTextEx(font, TextFormat("Defense: %d", p->defense), (Vector2){ (float)x_pos + 110, (float)y_pos + 45 }, 20, 1, GRAY);
    DrawTextEx(font, TextFormat("Energy: %d", p->energy), (Vector2){ (float)x_pos + 210, (float)y_pos + 45 }, 20, 1, SKYBLUE);
}

// DrawCharSelection 函式 - 保持不變
void DrawCharSelection() {
    DrawTexture(backgroundTexture, 0, 0, WHITE);
    DrawTextEx(font, "Select Your Hero", (Vector2){ (float)GetScreenWidth()/2 - MeasureTextEx(font, "Select Your Hero", 60, 2).x/2, 100 }, 60, 2, WHITE);
    for(int i = 0; i < 10; i++) {
        Rectangle btn_bounds = {200.0f + (i % 5) * 180, 250.0f + (i / 5) * 120, 160, 80};
        bool hover = CheckCollisionPointRec(GetMousePosition(), btn_bounds);
        DrawRectangleRec(btn_bounds, hover ? SKYBLUE : LIGHTGRAY);
        DrawRectangleLinesEx(btn_bounds, 2, BLACK);
        DrawTextEx(font, character_names[i], (Vector2){ btn_bounds.x + 20, btn_bounds.y + 30 }, 20, 1, BLACK);
    }
    Rectangle exit_btn = { GetScreenWidth() - 180.0f, GetScreenHeight() - 70.0f, 160, 50 };
    bool btn_hover = CheckCollisionPointRec(GetMousePosition(), exit_btn);
    DrawRectangleRec(exit_btn, btn_hover ? RED : MAROON);
    DrawTextEx(font, "Exit Game", (Vector2){ exit_btn.x + 40, exit_btn.y + 15 }, 20, 1, WHITE);
}

// DrawGameBoard 函式 - 保持不變
void DrawGameBoard(const Game* game) {
    float screenWidth = GetScreenWidth();
    float screenHeight = GetScreenHeight();

    const int gameplay_slots = 11;
    const float slot_w = 80.0f;
    const float slot_h = 110.0f;
    const float spacing = 15.0f;
    float gameplay_area_w = gameplay_slots * (slot_w + spacing) - spacing;
    float start_x = (screenWidth - gameplay_area_w) / 2.0f;
    
    float screen_center_y = screenHeight / 2.0f;
    float board_offset_y = 40.0f;
    float mid_y = screen_center_y - (slot_h / 2.0f) + board_offset_y;

    const float circle_radius = 35.0f;
    const float vertical_gap = 95.0f;
    float top_row_y = screen_center_y - vertical_gap + board_offset_y;
    float bottom_row_y = screen_center_y + vertical_gap + board_offset_y;

    for (int i = 0; i < gameplay_slots; i++) {
        float current_x = start_x + i * (slot_w + spacing);
        float circle_center_x = current_x + (slot_w / 2.0f);
        Color color = (i == 2 || i == 8) ? Fade(LIME, 0.7f) : Fade(DARKGRAY, 0.7f);
        float thickness = (i == 5) ? 5.0f : 3.0f;

        Vector2 top_center = { circle_center_x, top_row_y };
        DrawCircleV(top_center, circle_radius, color);
        DrawCircleLinesV(top_center, circle_radius, BLACK);
        if(i == 5) DrawRing(top_center, circle_radius - thickness, circle_radius, 0, 360, 36, BLACK);

        Vector2 bottom_center = { circle_center_x, bottom_row_y };
        DrawCircleV(bottom_center, circle_radius, color);
        DrawCircleLinesV(bottom_center, circle_radius, BLACK);
        if(i == 5) DrawRing(bottom_center, circle_radius - thickness, circle_radius, 0, 360, 36, BLACK);
        
        for(int p_idx = 0; p_idx < 2; p_idx++){
            if(game->inner_game.players[p_idx].locate[0] == i) {
                Color token_color = (p_idx == 0) ? BLUE : RED;
                DrawCircleV(bottom_center, circle_radius * 0.6f, token_color);
                DrawCircleLines(bottom_center.x, bottom_center.y, circle_radius * 0.6f, BLACK);
            }
        }
    }
    
    for (int i = 0; i < gameplay_slots; i++) {
        float current_x = start_x + i * (slot_w + spacing);
        Rectangle card_slot = {current_x, mid_y, slot_w, slot_h};
        DrawRectangleRounded(card_slot, 0.1f, 10, Fade(BLACK, 0.6f));
        DrawRectangleRoundedLinesEx(card_slot, 0.1f, 10, 2, LIME);
    }
    
    Rectangle deck_rect = { start_x - slot_w - spacing * 3, mid_y, slot_w, slot_h };
    DrawRectangleRounded(deck_rect, 0.1f, 10, Fade(DARKBLUE, 0.8f));
    DrawRectangleRoundedLinesEx(deck_rect, 0.1f, 10, 2, SKYBLUE);
    DrawTextEx(font, "Deck", (Vector2){deck_rect.x + 20, deck_rect.y - 25}, 20, 1, WHITE);
    DrawTextEx(font, TextFormat("%d", game->inner_game.players[0].deck.SIZE), (Vector2){deck_rect.x + 30, deck_rect.y + 40}, 24, 1, WHITE);

    Rectangle discard_rect = { start_x + gameplay_area_w + spacing * 3, mid_y, slot_w, slot_h };
    DrawRectangleRounded(discard_rect, 0.1f, 10, Fade(MAROON, 0.8f));
    DrawRectangleRoundedLinesEx(discard_rect, 0.1f, 10, 2, RED);
    DrawTextEx(font, "Discard", (Vector2){discard_rect.x + 5, discard_rect.y - 25}, 20, 1, WHITE);
    DrawTextEx(font, TextFormat("%d", game->inner_game.players[0].graveyard.SIZE), (Vector2){discard_rect.x + 30, discard_rect.y + 40}, 24, 1, WHITE);
}

// DrawBattleInterface 函式 - 保持不變
void DrawBattleInterface(const Game* game) {
    const player* human = &game->inner_game.players[0];
    int hand_width = human->hand.SIZE * (CARD_WIDTH + 15) - 15;
    float hand_start_x = (GetScreenWidth() - hand_width) / 2.0f;
    float hand_y = GetScreenHeight() - CARD_HEIGHT - 20;
    
    for (uint32_t i = 0; i < human->hand.SIZE; ++i) {
        Rectangle card_bounds = { hand_start_x + i * (CARD_WIDTH + 15), hand_y, CARD_WIDTH, CARD_HEIGHT };
        bool is_hovered = CheckCollisionPointRec(GetMousePosition(), card_bounds);
        const Card* card_info = get_card_info(human->hand.array[i]);
        DrawCard(card_info, card_bounds, is_hovered, false);
    }
    
    const player* bot = &game->inner_game.players[1];
    int bot_hand_width = bot->hand.SIZE * (CARD_WIDTH/1.5f + 10) - 10;
    float bot_hand_start_x = (GetScreenWidth() - bot_hand_width) / 2.0f;
    for (uint32_t i = 0; i < bot->hand.SIZE; ++i) {
        Rectangle bot_card = {bot_hand_start_x + i * (CARD_WIDTH/1.5f + 10), 80, CARD_WIDTH/1.5f, CARD_HEIGHT/1.5f};
        DrawRectangleRounded(bot_card, 0.08f, 10, DARKBLUE);
        DrawRectangleRoundedLinesEx(bot_card, 0.08f, 10, 4, BLUE);
    }
    
    Rectangle end_turn_btn = { GetScreenWidth() - 200.0f, GetScreenHeight() - 60.0f, 180, 50 };
    bool et_hover = CheckCollisionPointRec(GetMousePosition(), end_turn_btn);
    DrawRectangleRec(end_turn_btn, et_hover ? LIME : GREEN);
    DrawTextEx(font, "End Turn", (Vector2){ end_turn_btn.x + 50, end_turn_btn.y + 15 }, 20, 1, BLACK);

    Rectangle focus_btn = { GetScreenWidth() - 200.0f, GetScreenHeight() - 120.0f, 180, 50 };
    bool focus_hover = CheckCollisionPointRec(GetMousePosition(), focus_btn);
    DrawRectangleRec(focus_btn, game->player_has_acted ? GRAY : (focus_hover ? YELLOW : GOLD));
    DrawTextEx(font, "Focus", (Vector2){ focus_btn.x + 60, focus_btn.y + 15 }, 20, 1, BLACK);

    Rectangle shop_btn = { GetScreenWidth() - 200.0f, GetScreenHeight() - 180.0f, 180, 50 };
    bool shop_hover = CheckCollisionPointRec(GetMousePosition(), shop_btn);
    DrawRectangleRec(shop_btn, shop_hover ? SKYBLUE : BLUE);
    DrawTextEx(font, "Shop", (Vector2){ shop_btn.x + 65, shop_btn.y + 15 }, 20, 1, WHITE);

    const char* turn_text = "";
    if (game->inner_game.now_turn_player_id == 0) {
        turn_text = "Your Turn";
    } else {
        turn_text = "Opponent's Turn";
    }
    Vector2 message_size = MeasureTextEx(font, turn_text, 40, 2);
    DrawTextEx(font, turn_text, (Vector2){ (GetScreenWidth() - message_size.x)/2, GetScreenHeight() / 2.0f }, 40, 2, WHITE);
}

// DrawFocusSelection 函式 - 保持不變
void DrawFocusSelection(const Game* game) {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.85f));
    DrawTextEx(font, "Focus: Remove a Card", (Vector2){ (float)GetScreenWidth()/2 - MeasureTextEx(font, "Focus: Remove a Card", 40, 2).x/2, 50 }, 40, 2, WHITE);
    DrawTextEx(font, "Select a card from your Hand or Graveyard to permanently remove it.", (Vector2){ (float)GetScreenWidth()/2 - MeasureTextEx(font, "Select a card from your Hand or Graveyard to permanently remove it.", 20, 1).x/2, 100 }, 20, 1, LIGHTGRAY);
    
    const player* p = &game->inner_game.players[0];

    DrawTextEx(font, "Your Hand:", (Vector2){50, 150}, 24, 1, WHITE);
    for (uint32_t i = 0; i < p->hand.SIZE; i++) {
        Rectangle card_bounds = { 50 + i * (CARD_WIDTH + 15), 180, CARD_WIDTH, CARD_HEIGHT };
        bool is_hovered = CheckCollisionPointRec(GetMousePosition(), card_bounds);
        const Card* card_info = get_card_info(p->hand.array[i]);
        DrawCard(card_info, card_bounds, is_hovered, false);
    }
    
    DrawTextEx(font, "Your Graveyard:", (Vector2){50, 400}, 24, 1, WHITE);
    for (uint32_t i = 0; i < p->graveyard.SIZE; i++) {
        Rectangle card_bounds = { 50 + i * (CARD_WIDTH + 15), 430, CARD_WIDTH, CARD_HEIGHT };
        bool is_hovered = CheckCollisionPointRec(GetMousePosition(), card_bounds);
        const Card* card_info = get_card_info(p->graveyard.array[i]);
        DrawCard(card_info, card_bounds, is_hovered, false);
    }

    Rectangle cancel_btn = { GetScreenWidth() - 180.0f, GetScreenHeight() - 70.0f, 160, 50 };
    bool btn_hover = CheckCollisionPointRec(GetMousePosition(), cancel_btn);
    DrawRectangleRec(cancel_btn, btn_hover ? ORANGE : DARKGRAY);
    DrawTextEx(font, "Cancel", (Vector2){ cancel_btn.x + 45, cancel_btn.y + 15 }, 20, 1, WHITE);
}

// DrawGame 函式 - 保持不變
void DrawGame(Game* game) {
    DrawTexture(backgroundTexture, 0, 0, WHITE);
    
    if (game->current_state == GAME_STATE_CHOOSE_CHAR) {
        DrawCharSelection();
        return;
    }
    
    // --- 繪製主要遊戲介面 (對所有非選擇角色狀態都繪製) ---
    DrawGameBoard(game);
    DrawPlayerInfo(game, true);
    DrawPlayerInfo(game, false);
    if (game->current_state != GAME_STATE_SHOP && game->current_state != GAME_STATE_FOCUS_REMOVE) {
         DrawBattleInterface(game);
    }

    // --- 繪製疊加層 (Overlays) ---
    switch (game->current_state) {
        case GAME_STATE_SHOP:
            DrawShop(game);
            break;
        case GAME_STATE_FOCUS_REMOVE:
            DrawFocusSelection(game);
            break;
        case GAME_STATE_AWAITING_BASIC_FOR_SKILL:
            DrawSkillPairingOverlay(game);
            break;
        case GAME_STATE_CHOOSE_MOVE_DIRECTION: {
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.5f));
            DrawTextEx(font, "Choose Direction", (Vector2){480, 300}, 40, 1, WHITE);
            Rectangle leftBtn = {480, 350, 120, 50};
            Rectangle rightBtn = {680, 350, 120, 50};
            DrawRectangleRec(leftBtn, CheckCollisionPointRec(GetMousePosition(), leftBtn) ? SKYBLUE : BLUE);
            DrawText("Left", leftBtn.x + 35, leftBtn.y + 15, 20, WHITE);
            DrawRectangleRec(rightBtn, CheckCollisionPointRec(GetMousePosition(), rightBtn) ? SKYBLUE : BLUE);
            DrawText("Right", rightBtn.x + 30, rightBtn.y + 15, 20, WHITE);
            break;
        }
        case GAME_STATE_GAME_OVER: {
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.6f));
            Vector2 message_size = MeasureTextEx(font, game->message, 60, 2);
            DrawTextEx(font, game->message, (Vector2){ (GetScreenWidth() - message_size.x)/2, (GetScreenHeight()/2) - 80 }, 60, 2, WHITE);
            Rectangle back_btn = { (float)GetScreenWidth()/2 - 125, (float)GetScreenHeight()/2 + 40, 250, 50 };
            bool back_hover = CheckCollisionPointRec(GetMousePosition(), back_btn);
            DrawRectangleRec(back_btn, back_hover ? SKYBLUE : BLUE);
            DrawTextEx(font, "Return to Menu", (Vector2){ back_btn.x + 50, back_btn.y + 15 }, 20, 1, WHITE);
            break;
        }
        default:
            break;
    }
}

// [修改] DrawShop 函式，調整排版
void DrawShop(const Game* game) {
    float screenWidth = GetScreenWidth();
    float screenHeight = GetScreenHeight();
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.85f));
    DrawTextEx(font, "Shop", (Vector2){screenWidth / 2 - MeasureTextEx(font, "Shop", 60, 2).x / 2, 40}, 60, 2, GOLD);
    DrawTextEx(font, TextFormat("Your Energy: %d", game->inner_game.players[0].energy), (Vector2){40, 40}, 30, 1, WHITE);
    
    // 繪製頁籤按鈕
    Rectangle basic_tab = { screenWidth / 2.0f - 210, 150, 200, 40 };
    Rectangle skill_tab = { screenWidth / 2.0f + 10, 150, 200, 40 };

    bool basic_hover = CheckCollisionPointRec(GetMousePosition(), basic_tab);
    DrawRectangleRec(basic_tab, game->shop_page == 0 ? GOLD : (basic_hover ? LIGHTGRAY : DARKGRAY));
    const char* basic_text = "Basic";
    Vector2 basic_text_size = MeasureTextEx(font, basic_text, 20, 1);
    DrawTextEx(font, basic_text, (Vector2){ basic_tab.x + (basic_tab.width - basic_text_size.x) / 2, basic_tab.y + 10 }, 20, 1, BLACK);
    DrawRectangleLinesEx(basic_tab, 2, BLACK);

    bool skill_hover = CheckCollisionPointRec(GetMousePosition(), skill_tab);
    DrawRectangleRec(skill_tab, game->shop_page == 1 ? GOLD : (skill_hover ? LIGHTGRAY : DARKGRAY));
    const char* skill_text = "Skill";
    Vector2 skill_text_size = MeasureTextEx(font, skill_text, 20, 1);
    DrawTextEx(font, skill_text, (Vector2){ skill_tab.x + (skill_tab.width - skill_text_size.x) / 2, skill_tab.y + 10 }, 20, 1, BLACK);
    DrawRectangleLinesEx(skill_tab, 2, BLACK);

    // 根據當前頁面繪製對應內容
    if (game->shop_page == 0) {
        // [修改] 調整基礎牌商店的排版以使其更平均分佈
        // 定義每一欄的寬度，包含卡牌和右側文字的空間
        float column_width = 300; 
        // 計算內容總寬度
        float total_content_width = 3 * column_width;
        // 計算起始 X 座標以使其置中
        float startX = (screenWidth - total_content_width) / 2.0f;
        
        float top_bound = 210;
        float bottom_bound = screenHeight - 90;
        float total_area_height = bottom_bound - top_bound;
        float row_spacing = total_area_height / 3.0f;

        for (int type = 0; type < 3; type++) { // 3 橫列 (Attack, Defense, Move)
            float row_center_y = top_bound + (row_spacing * type) + (row_spacing / 2.0f);
            float card_start_y = row_center_y - (CARD_HEIGHT / 2.0f);

            for (int level = 0; level < 3; level++) { // 3 直行 (LV1, LV2, LV3)
                const vector* pile = &game->shop_piles[type][level];
                if (pile->SIZE > 0) {
                    const Card* card = get_card_info(pile->array[0]);
                    if(card) {
                        // 計算每一張卡牌的 X 座標
                        float card_start_x = startX + (level * column_width);
                        Rectangle bounds = {card_start_x, card_start_y, CARD_WIDTH, CARD_HEIGHT};
                        bool hovered = CheckCollisionPointRec(GetMousePosition(), bounds);
                        DrawCard(card, bounds, hovered, false);
                        
                        // 將文字繪製在卡牌右側
                        float text_x = bounds.x + CARD_WIDTH + 20;
                        DrawText(TextFormat("Cost: %d", card->cost), text_x, bounds.y + 40, 20, WHITE);
                        DrawText(TextFormat("Left: %d", pile->SIZE), text_x, bounds.y + 70, 20, WHITE);
                    }
                }
            }
        }
    } else if (game->shop_page == 1) {
        // 繪製技能牌商店 (目前為空)
        const char* coming_soon_text = "Feature coming soon!";
        Vector2 text_size = MeasureTextEx(font, coming_soon_text, 40, 2);
        DrawTextEx(font, coming_soon_text, (Vector2){screenWidth / 2 - text_size.x / 2, screenHeight / 2}, 40, 2, GRAY);
    }
    
    // 繪製關閉按鈕
    Rectangle close_btn = { screenWidth - 160, screenHeight - 70, 140, 50 };
    bool hover = CheckCollisionPointRec(GetMousePosition(), close_btn);
    DrawRectangleRec(close_btn, hover ? RED : MAROON);
    DrawTextEx(font, "Close", (Vector2){close_btn.x + 45, close_btn.y + 15}, 20, 1, WHITE);
}

void DrawSkillPairingOverlay(const Game* game) {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.8f));
    Vector2 msg_size = MeasureTextEx(font, game->message, 30, 1);
    DrawTextEx(font, game->message, (Vector2){(GetScreenWidth() - msg_size.x)/2, GetScreenHeight()/2 - 40}, 30, 1, WHITE);
    
    Rectangle cancel_btn = { GetScreenWidth() / 2.0f - 100, GetScreenHeight() / 2.0f + 50, 200, 50 };
    bool hover = CheckCollisionPointRec(GetMousePosition(), cancel_btn);
    DrawRectangleRec(cancel_btn, hover ? RED : MAROON);
    DrawTextEx(font, "Cancel", (Vector2){ cancel_btn.x + 65, cancel_btn.y + 15 }, 20, 1, WHITE);

    const player* human = &game->inner_game.players[0];
    
    // [修改] 根據已選擇的技能牌，判斷需要高亮的基礎牌類型
    const Card* pending_skill_card = get_card_info(human->hand.array[game->pending_skill_card_index]);
    if (!pending_skill_card) return; // 安全檢查

    CardType required_type;
    switch (pending_skill_card->id % 10) {
        case 1: required_type = ATTACK; break;
        case 2: required_type = DEFENSE; break;
        case 3: required_type = MOVE; break;
        default: required_type = GENERIC; break; // 無效類型
    }

    int hand_width = human->hand.SIZE * (CARD_WIDTH + 15) - 15;
    float hand_start_x = (GetScreenWidth() - hand_width) / 2.0f;
    float hand_y = GetScreenHeight() - CARD_HEIGHT - 20;

    for (uint32_t i = 0; i < human->hand.SIZE; ++i) {
        Rectangle card_bounds = { hand_start_x + i * (CARD_WIDTH + 15), hand_y, CARD_WIDTH, CARD_HEIGHT };
        const Card* card_info = get_card_info(human->hand.array[i]);
        if (!card_info) continue;

        if ((int)i == game->pending_skill_card_index) {
            DrawRectangleRoundedLinesEx(card_bounds, 0.08f, 10, 5, YELLOW);
        } else {
            // [修改] 只高亮顯示類型相符的基礎牌
            if (card_info->type == required_type) {
                DrawCard(card_info, card_bounds, false, false);
                DrawRectangleRoundedLinesEx(card_bounds, 0.08f, 10, 3, LIME);
            }
        }
    }
}
