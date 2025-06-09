#include "definitions.h"
#include <stddef.h> // for NULL

#define DEF_CARD(id, name, type, lvl, val, cst) {id, name, type, lvl, val, cst}

// 卡牌資料庫實體 (英文版)
const Card card_database[] = {
    // 基本牌
    DEF_CARD(101, "Attack LV1", ATTACK, 1, 1, 2),
    DEF_CARD(102, "Attack LV2", ATTACK, 2, 2, 4),
    DEF_CARD(103, "Attack LV3", ATTACK, 3, 3, 6),
    DEF_CARD(201, "Defense LV1", DEFENSE, 1, 1, 2),
    DEF_CARD(202, "Defense LV2", DEFENSE, 2, 2, 4),
    DEF_CARD(203, "Defense LV3", DEFENSE, 3, 3, 6),
    DEF_CARD(301, "Move LV1", MOVE, 1, 1, 1),
    DEF_CARD(302, "Move LV2", MOVE, 2, 2, 2),
    DEF_CARD(303, "Move LV3", MOVE, 3, 3, 3),
    DEF_CARD(401, "Crystal", GENERIC, 0, 1, 0),
    // 角色技能牌 (英文版)
    DEF_CARD(501, "[Red Hood] Wolf Claw", SKILL, 1, 2, 0),
    DEF_CARD(502, "[Snow White] Poison Apple", SKILL, 1, 1, 0),
    DEF_CARD(503, "[Sleeping Beauty] Lily's Dream", SKILL, 1, 0, 0),
    DEF_CARD(504, "[Alice] Mad Tea Party", SKILL, 1, 0, 0),
    DEF_CARD(505, "[Mulan] Chi Slash", SKILL, 1, 3, 0),
    DEF_CARD(506, "[Kaguya] Five Problems", SKILL, 1, 0, 0),
    DEF_CARD(507, "[Mermaid] Bubble Song", SKILL, 1, 0, 0),
    DEF_CARD(508, "[Match Girl] Last Match", SKILL, 1, 5, 0),
    DEF_CARD(509, "[Dorothy] Tornado Road", SKILL, 1, 1, 0),
    DEF_CARD(510, "[Scheherazade] 1001 Nights", SKILL, 1, 0, 0),
    // 狀態牌 (英文版)
    DEF_CARD(901, "[Status] Poisoned", STATUS, 0, 0, 0),
};
const int NUM_CARD_DEFINITIONS = sizeof(card_database) / sizeof(Card);

// 角色名稱陣列 (英文版)
const char* character_names[] = {
    "Red Hood", "Snow White", "Sleeping Beauty", "Alice", 
    "Mulan", "Kaguya", "Mermaid", "Match Girl", 
    "Dorothy", "Scheherazade"
};

const Card* get_card_info(int32_t card_id) {
    for (int i = 0; i < NUM_CARD_DEFINITIONS; ++i) {
        if (card_database[i].id == card_id) return &card_database[i];
    }
    return NULL;
}
