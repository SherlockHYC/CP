#include "gui.h"
#include "definitions.h"
#include "raylib.h"

// 外部資料與函式
extern const char* character_names[];
extern Font font;
extern Texture2D backgroundTexture;

// 定義卡牌尺寸
const int CARD_WIDTH = 120;
const int CARD_HEIGHT = 170;

// =================================================================
//                      繪製函式 (Drawing Functions)
// =================================================================

void DrawCard(const Card* card, Rectangle bounds, bool is_selected) {
    DrawRectangleRec(bounds, is_selected ? GOLD : RAYWHITE);
    DrawRectangleLinesEx(bounds, 3, is_selected ? MAROON : BLACK);
    if (card) {
        DrawTextEx(font, card->name, (Vector2){ bounds.x + 10, bounds.y + 10 }, 20, 1, BLACK);
        if (card->type == ATTACK) DrawTextEx(font, TextFormat("Attack: %d", card->value), (Vector2){ bounds.x + 10, bounds.y + 40 }, 18, 1, RED);
        else if (card->type == SKILL) DrawTextEx(font, "Skill", (Vector2){ bounds.x + 10, bounds.y + 40 }, 18, 1, BLUE);
        DrawTextEx(font, "Cost: 1 Energy", (Vector2){ bounds.x + 10, bounds.y + CARD_HEIGHT - 30 }, 16, 1, DARKGRAY);
    }
}

void DrawPlayerInfo(const player* p, bool is_human) {
    if (p->life == 0 && p->character == 0) return;
    int y_pos = is_human ? GetScreenHeight() - 120 : 20;
    if(p->character < 0 || p->character >= 10) return;
    const char* role_name = character_names[p->character];
    const char* player_title = is_human ? "You" : "Opponent";
    DrawTextEx(font, TextFormat("%s (%s)", player_title, role_name), (Vector2){ 20, (float)y_pos }, 30, 1, WHITE);
    DrawTextEx(font, TextFormat("HP: %d", p->life), (Vector2){ 20, (float)y_pos + 40 }, 24, 1, LIME);
    DrawTextEx(font, TextFormat("Energy: %d", p->energy), (Vector2){ 20, (float)y_pos + 70 }, 24, 1, SKYBLUE);
}

void DrawCharSelection(Game* game) {
    DrawTexture(backgroundTexture, 0, 0, WHITE);
    DrawTextEx(font, "Select Your Hero", (Vector2){ (float)GetScreenWidth()/2 - MeasureTextEx(font, "Select Your Hero", 60, 2).x/2, 100 }, 60, 2, WHITE);
    for(int i = 0; i < 10; i++) {
        Rectangle btn_bounds = {200.0f + (i % 5) * 180, 250.0f + (i / 5) * 120, 160, 80};
        bool hover = CheckCollisionPointRec(GetMousePosition(), btn_bounds);
        DrawRectangleRec(btn_bounds, hover ? SKYBLUE : LIGHTGRAY);
        DrawRectangleLinesEx(btn_bounds, 2, BLACK);
        DrawTextEx(font, character_names[i], (Vector2){ btn_bounds.x + 20, btn_bounds.y + 30 }, 20, 1, BLACK);
    }

    // [新增] 繪製退出按鈕
    Rectangle exit_btn = { GetScreenWidth() - 180.0f, GetScreenHeight() - 70.0f, 160, 50 };
    bool btn_hover = CheckCollisionPointRec(GetMousePosition(), exit_btn);
    DrawRectangleRec(exit_btn, btn_hover ? RED : MAROON);
    DrawTextEx(font, "Exit Game", (Vector2){ exit_btn.x + 40, exit_btn.y + 15 }, 20, 1, WHITE);
}

// [FIXED] Rewrote the function for correct alignment and to fix the compilation error.
void DrawGameBoard() {
    float screenWidth = GetScreenWidth();
    float screenHeight = GetScreenHeight();

    // --- 1. Define the 11-slot core gameplay area ---
    const int gameplay_slots = 11;
    const float slot_w = 70.0f;
    const float slot_h = 100.0f;
    const float spacing = 15.0f;
    float gameplay_area_w = gameplay_slots * (slot_w + spacing) - spacing;
    float start_x = (screenWidth - gameplay_area_w) / 2.0f; // This is the start of the 11 aligned slots
    float mid_y = (screenHeight - slot_h) / 2.0f;

    // --- 2. Draw the 11 aligned slots (middle, top, bottom) ---
    for (int i = 0; i < gameplay_slots; i++) {
        // Middle row (card slots)
        Rectangle card_slot = {start_x + i * (slot_w + spacing), mid_y, slot_w, slot_h};
        DrawRectangleRec(card_slot, Fade(BLACK, 0.5f));
        DrawRectangleLinesEx(card_slot, 2, LIGHTGRAY);

        // Top and Bottom rows (circles)
        const float circle_radius = 25.0f;
        float top_y = mid_y - circle_radius - 60.0f;
        float bottom_y = mid_y + slot_h + circle_radius + 60.0f;
        float circle_center_x = card_slot.x + (slot_w / 2.0f); // Align center of circle with center of card slot
        Color color = (i == 0 || i == gameplay_slots - 1) ? BROWN : GOLD;
        float thickness = (i == 5 || i == 6) ? 4.0f : 2.0f; // Indices 5 and 6 are the middle two of 11 slots

        Vector2 top_center = { circle_center_x, top_y };
        DrawCircleV(top_center, circle_radius, color);
        DrawCircleLinesV(top_center, circle_radius, DARKGRAY);
        if(i == 5 || i == 6) DrawRing(top_center, circle_radius - thickness, circle_radius, 0, 360, 36, DARKGRAY);

        Vector2 bottom_center = { circle_center_x, bottom_y };
        DrawCircleV(bottom_center, circle_radius, color);
        DrawCircleLinesV(bottom_center, circle_radius, DARKGRAY);
        if(i == 5 || i == 6) DrawRing(bottom_center, circle_radius - thickness, circle_radius, 0, 360, 36, DARKGRAY);
    }
    
    // --- 3. Draw the Deck and Discard slots outside the core area ---
    Rectangle deck_slot = {start_x - slot_w - spacing * 2, mid_y, slot_w, slot_h};
    DrawRectangleRec(deck_slot, Fade(DARKBLUE, 0.6f));
    DrawRectangleLinesEx(deck_slot, 2, SKYBLUE);
    DrawTextEx(font, "Deck", (Vector2){deck_slot.x + 15, deck_slot.y - 25}, 20, 1, WHITE);

    Rectangle discard_slot = {start_x + gameplay_area_w + spacing, mid_y, slot_w, slot_h};
    DrawRectangleRec(discard_slot, Fade(MAROON, 0.6f));
    DrawRectangleLinesEx(discard_slot, 2, RED);
    DrawTextEx(font, "Discard", (Vector2){discard_slot.x, discard_slot.y - 25}, 20, 1, WHITE);
}


void DrawGame(Game* game) {
    if (game->current_state == GAME_STATE_CHOOSE_CHAR) {
        DrawCharSelection(game);
    } else {
        // We still draw the background image first.
        DrawTexture(backgroundTexture, 0, 0, WHITE);
        // Then draw the actual game board on top.
        DrawGameBoard();

        DrawPlayerInfo(&game->inner_game.players[0], true);
        DrawPlayerInfo(&game->inner_game.players[1], false);

        const player* human = &game->inner_game.players[0];
        int hand_width = human->hand.SIZE * (CARD_WIDTH + 10) - 10;
        float hand_start_x = (GetScreenWidth() - hand_width) / 2.0f;
        for (uint32_t i = 0; i < human->hand.SIZE; ++i) {
            Rectangle card_bounds = { hand_start_x + i * (CARD_WIDTH + 10), GetScreenHeight() - CARD_HEIGHT - 20, CARD_WIDTH, CARD_HEIGHT };
            const Card* card_info = get_card_info(human->hand.array[i]);
            bool is_selected = CheckCollisionPointRec(GetMousePosition(), card_bounds);
            DrawCard(card_info, card_bounds, is_selected);
        }
        
        const player* bot = &game->inner_game.players[1];
        int bot_hand_width = bot->hand.SIZE * 80 - 10;
        float bot_hand_start_x = (GetScreenWidth() - bot_hand_width) / 2.0f;
        for (uint32_t i = 0; i < bot->hand.SIZE; ++i) {
             Rectangle bot_card = {bot_hand_start_x + i * 80, 20, CARD_WIDTH, CARD_HEIGHT};
             DrawRectangleRec(bot_card, DARKBLUE);
             DrawRectangleLinesEx(bot_card, 2, BLUE);
        }

        Rectangle end_turn_btn = { GetScreenWidth() - 180.0f, GetScreenHeight() - 70.0f, 160, 50 };
        bool btn_hover = CheckCollisionPointRec(GetMousePosition(), end_turn_btn);
        DrawRectangleRec(end_turn_btn, btn_hover ? LIME : GREEN);
        DrawTextEx(font, "End Turn", (Vector2){ end_turn_btn.x + 40, end_turn_btn.y + 15 }, 20, 1, BLACK);
        
        Vector2 message_size = MeasureTextEx(font, game->message, 40, 2);
        DrawTextEx(font, game->message, (Vector2){ (GetScreenWidth() - message_size.x)/2, (GetScreenHeight() - message_size.y)/2 }, 40, 2, WHITE);
    
        if (game->current_state == GAME_STATE_GAME_OVER) {
            Rectangle back_btn = { (float)GetScreenWidth()/2 - 125, (float)GetScreenHeight()/2 + 40, 250, 50 };
            bool back_hover = CheckCollisionPointRec(GetMousePosition(), back_btn);
            DrawRectangleRec(back_btn, back_hover ? SKYBLUE : BLUE);
            DrawTextEx(font, "Return to Menu", (Vector2){ back_btn.x + 50, back_btn.y + 15 }, 20, 1, WHITE);
        }
    }
}
