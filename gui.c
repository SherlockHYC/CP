#include "gui.h"
#include "definitions.h"
#include "raylib.h"

// External data and functions
extern const char* character_names[];
extern Font font;
extern Texture2D backgroundTexture;

void DrawShop(const Game* game);

// =================================================================
//                      Drawing Functions
// =================================================================

void DrawCard(const Card* card, Rectangle bounds, bool is_hovered, bool is_opponent_card) {
    if (is_opponent_card) {
        DrawRectangleRounded(bounds, 0.08f, 10, DARKBLUE);
        DrawRectangleRoundedLinesEx(bounds, 0.08f, 10, 4, BLUE);
        return;
    }

    if (is_hovered) {
        bounds.y -= 20; // Hover effect for player's cards
    }
    DrawRectangleRounded(bounds, 0.08f, 10, RAYWHITE);
    DrawRectangleRoundedLinesEx(bounds, 0.08f, 10, is_hovered ? 5 : 3, is_hovered ? GOLD : BLACK);
    
    if (card) {
        DrawTextEx(font, card->name, (Vector2){ bounds.x + 10, bounds.y + 15 }, 18, 1, BLACK);
        if (card->type == ATTACK) DrawTextEx(font, TextFormat("Attack: %d", card->value), (Vector2){ bounds.x + 15, bounds.y + 50 }, 16, 1, RED);
        else if (card->type == DEFENSE) DrawTextEx(font, TextFormat("Defense: %d", card->value), (Vector2){ bounds.x + 15, bounds.y + 50 }, 16, 1, DARKGREEN);
        else if (card->type == SKILL) DrawTextEx(font, "Skill", (Vector2){ bounds.x + 15, bounds.y + 50 }, 16, 1, BLUE);
        else if (card->type == MOVE) DrawTextEx(font, TextFormat("Move: %d", card->value), (Vector2){ bounds.x + 15, bounds.y + 50 }, 16, 1, PURPLE);
        
        if (card->type == ATTACK || card->type == DEFENSE || card->type == MOVE || card->type == GENERIC) {
             DrawTextEx(font, TextFormat("Energy Gain: +%d", card->value), (Vector2){ bounds.x + 15, bounds.y + CARD_HEIGHT - 35 }, 14, 1, SKYBLUE);
        } else if (card->type == SKILL) {
             DrawTextEx(font, TextFormat("Cost: %d Energy", card->cost), (Vector2){ bounds.x + 15, bounds.y + CARD_HEIGHT - 35 }, 14, 1, DARKGRAY);
        }
    }
}

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
    
    // --- Draw Action Buttons ---
    Rectangle end_turn_btn = { GetScreenWidth() - 200.0f, GetScreenHeight() - 60.0f, 180, 50 };
    bool et_hover = CheckCollisionPointRec(GetMousePosition(), end_turn_btn);
    DrawRectangleRec(end_turn_btn, et_hover ? LIME : GREEN);
    DrawTextEx(font, "End Turn", (Vector2){ end_turn_btn.x + 50, end_turn_btn.y + 15 }, 20, 1, BLACK);

    Rectangle focus_btn = { GetScreenWidth() - 200.0f, GetScreenHeight() - 120.0f, 180, 50 };
    bool focus_hover = CheckCollisionPointRec(GetMousePosition(), focus_btn);
    DrawRectangleRec(focus_btn, game->player_has_acted ? GRAY : (focus_hover ? YELLOW : GOLD));
    DrawTextEx(font, "Focus", (Vector2){ focus_btn.x + 60, focus_btn.y + 15 }, 20, 1, BLACK);

    // [NEW] Draw Shop button
    Rectangle shop_btn = { GetScreenWidth() - 200.0f, GetScreenHeight() - 180.0f, 180, 50 };
    bool shop_hover = CheckCollisionPointRec(GetMousePosition(), shop_btn);
    DrawRectangleRec(shop_btn, shop_hover ? SKYBLUE : BLUE);
    DrawTextEx(font, "Shop", (Vector2){ shop_btn.x + 65, shop_btn.y + 15 }, 20, 1, WHITE);

    Vector2 message_size = MeasureTextEx(font, game->message, 40, 2);
    DrawTextEx(font, game->message, (Vector2){ (GetScreenWidth() - message_size.x)/2, GetScreenHeight() / 2.0f }, 40, 2, WHITE);
}

void DrawShop(const Game* game) {
    float screenWidth = GetScreenWidth();
    float screenHeight = GetScreenHeight();

    // Draw a semi-transparent background overlay
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.8f));
    
    // Draw Shop Title
    DrawTextEx(font, "Shop", (Vector2){screenWidth / 2 - MeasureTextEx(font, "Shop", 60, 2).x / 2, 40}, 60, 2, GOLD);

    // Draw Player's current energy
    DrawTextEx(font, TextFormat("Your Energy: %d", game->inner_game.players[0].energy), (Vector2){40, 40}, 30, 1, WHITE);

    // --- Draw Wares ---
    float startY = 120;
    float startX = 100;
    float card_gap_x = CARD_WIDTH + 20;
    float card_gap_y = CARD_HEIGHT + 40;

    for (int type = 0; type < 3; type++) { // 0:ATK, 1:DEF, 2:MOV
        for (int level = 0; level < 3; level++) {
            const vector* pile = &game->shop_piles[type][level];
            if (pile->SIZE > 0) {
                const Card* card = get_card_info(pile->array[0]);
                if (card) {
                    Rectangle bounds = {startX + level * card_gap_x, startY + type * card_gap_y, CARD_WIDTH, CARD_HEIGHT};
                    bool hovered = CheckCollisionPointRec(GetMousePosition(), bounds);
                    
                    DrawCard(card, bounds, hovered, false);
                    DrawText(TextFormat("Cost: %d", card->cost), bounds.x + 15, bounds.y + CARD_HEIGHT + 5, 20, WHITE);
                    DrawText(TextFormat("Left: %d", pile->SIZE), bounds.x + 15, bounds.y + CARD_HEIGHT + 25, 20, WHITE);
                }
            }
        }
    }
    
    // --- Draw Close Button ---
    Rectangle close_btn = { screenWidth - 160, screenHeight - 70, 140, 50 };
    bool hover = CheckCollisionPointRec(GetMousePosition(), close_btn);
    DrawRectangleRec(close_btn, hover ? RED : MAROON);
    DrawTextEx(font, "Close", (Vector2){close_btn.x + 40, close_btn.y + 15}, 20, 1, WHITE);
}

void DrawGame(Game* game) {
    DrawTexture(backgroundTexture, 0, 0, WHITE);
    if (game->current_state == GAME_STATE_CHOOSE_CHAR) {
        DrawCharSelection();
    } else {
        DrawGameBoard(game);
        DrawPlayerInfo(game, true);
        DrawPlayerInfo(game, false);
        DrawBattleInterface(game);
        
        if (game->current_state == GAME_STATE_CHOOSE_MOVE_DIRECTION) {
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.5f));
            DrawTextEx(font, "Choose Direction", (Vector2){480, 300}, 40, 1, WHITE);
            
            Rectangle leftBtn = {480, 350, 120, 50};
            Rectangle rightBtn = {680, 350, 120, 50};
            
            DrawRectangleRec(leftBtn, CheckCollisionPointRec(GetMousePosition(), leftBtn) ? SKYBLUE : BLUE);
            DrawText("Left", leftBtn.x + 35, leftBtn.y + 15, 20, WHITE);
            
            DrawRectangleRec(rightBtn, CheckCollisionPointRec(GetMousePosition(), rightBtn) ? SKYBLUE : BLUE);
            DrawText("Right", rightBtn.x + 30, rightBtn.y + 15, 20, WHITE);
        }
        
        if (game->current_state == GAME_STATE_SHOP) {
            DrawShop(game);
        }

        if (game->current_state == GAME_STATE_GAME_OVER) {
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.6f));
            Vector2 message_size = MeasureTextEx(font, game->message, 60, 2);
            DrawTextEx(font, game->message, (Vector2){ (GetScreenWidth() - message_size.x)/2, (GetScreenHeight()/2) - 80 }, 60, 2, WHITE);
            
            Rectangle back_btn = { (float)GetScreenWidth()/2 - 125, (float)GetScreenHeight()/2 + 40, 250, 50 };
            bool back_hover = CheckCollisionPointRec(GetMousePosition(), back_btn);
            DrawRectangleRec(back_btn, back_hover ? SKYBLUE : BLUE);
            DrawTextEx(font, "Return to Menu", (Vector2){ back_btn.x + 50, back_btn.y + 15 }, 20, 1, WHITE);
        }
    }
}
