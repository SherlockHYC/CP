#include "gui.h"
#include "definitions.h"
#include "raylib.h"

// 外部資料與函式
extern const char* character_names[];
extern Font font; // 引用在 main.c 中載入的全域字型

// 繪製函式
void DrawCard(const Card* card, Rectangle bounds, bool is_selected) {
    DrawRectangleRec(bounds, is_selected ? GOLD : RAYWHITE);
    DrawRectangleLinesEx(bounds, 2, BLACK);
    if (card) {
        // 使用 DrawTextEx 來指定字型
        DrawTextEx(font, card->name, (Vector2){ bounds.x + 10, bounds.y + 10 }, 20, 1, BLACK);
        if (card->type == ATTACK) {
            DrawTextEx(font, TextFormat("Attack: %d", card->value), (Vector2){ bounds.x + 10, bounds.y + 40 }, 18, 1, RED);
        } else if (card->type == SKILL) {
            DrawTextEx(font, "Skill", (Vector2){ bounds.x + 10, bounds.y + 40 }, 18, 1, BLUE);
        }
        DrawTextEx(font, "Cost: 1 Energy", (Vector2){ bounds.x + 10, bounds.y + 120 }, 16, 1, DARKGRAY);
    }
}

void DrawPlayerInfo(const player* p, bool is_human) {
    if (p->life == 0 && p->character == 0) return; // 避免在初始狀態繪製
    
    int y_pos = is_human ? 520 : 20;
    if(p->character < 0 || p->character >= 10) return;
    const char* role_name = character_names[p->character];
    const char* player_title = is_human ? "You" : "Opponent";

    DrawTextEx(font, TextFormat("%s (%s)", player_title, role_name), (Vector2){ 20, (float)y_pos }, 30, 1, WHITE);
    DrawTextEx(font, TextFormat("HP: %d", p->life), (Vector2){ 20, (float)y_pos + 40 }, 24, 1, GREEN);
    DrawTextEx(font, TextFormat("Energy: %d", p->energy), (Vector2){ 20, (float)y_pos + 70 }, 24, 1, SKYBLUE);
}

void DrawCharSelection(Game* game) {
    DrawTextEx(font, "Select Your Hero", (Vector2){ 400, 100 }, 60, 2, WHITE);
    for(int i = 0; i < 10; i++) {
        Rectangle btn_bounds = {200.0f + (i % 5) * 180, 250.0f + (i / 5) * 120, 160, 80};
        bool hover = CheckCollisionPointRec(GetMousePosition(), btn_bounds);
        DrawRectangleRec(btn_bounds, hover ? SKYBLUE : LIGHTGRAY);
        DrawRectangleLinesEx(btn_bounds, 2, BLACK);
        DrawTextEx(font, character_names[i], (Vector2){ btn_bounds.x + 20, btn_bounds.y + 30 }, 20, 1, BLACK);
    }
}


void DrawGame(Game* game) {
    switch(game->current_state) {
        case GAME_STATE_CHOOSE_CHAR:
            DrawCharSelection(game);
            return;

        case GAME_STATE_HUMAN_TURN:
        case GAME_STATE_BOT_TURN:
        case GAME_STATE_GAME_OVER:
            DrawPlayerInfo(&game->inner_game.players[0], true);
            DrawPlayerInfo(&game->inner_game.players[1], false);

            const player* human = &game->inner_game.players[0];
            for (uint32_t i = 0; i < human->hand.SIZE; ++i) {
                Rectangle card_bounds = { 300 + i * 140.0f, 520, 120, 150 };
                const Card* card_info = get_card_info(human->hand.array[i]);
                bool is_selected = CheckCollisionPointRec(GetMousePosition(), card_bounds);
                DrawCard(card_info, card_bounds, is_selected);
            }

            Rectangle end_turn_btn = { 1100, 650, 160, 50 };
            bool btn_hover = CheckCollisionPointRec(GetMousePosition(), end_turn_btn);
            DrawRectangleRec(end_turn_btn, btn_hover ? LIME : GREEN);
            DrawTextEx(font, "End Turn", (Vector2){ end_turn_btn.x + 40, end_turn_btn.y + 15 }, 20, 1, BLACK);
            
            DrawTextEx(font, game->message, (Vector2){ 500, 350 }, 40, 2, WHITE);
            break;
    }
}
