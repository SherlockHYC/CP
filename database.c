#include "definitions.h"
#include <stddef.h> // for NULL

#define DEF_CARD(id, name, type, lvl, val, cst) {id, name, type, lvl, val, cst}

const Card card_database[] = {
    // Basic Cards (Cost is for buying from the shop)
    DEF_CARD(101, "Attack LV1",    ATTACK,  1, 1, 1), // Cost: 1
    DEF_CARD(102, "Attack LV2",    ATTACK,  2, 2, 3), // Cost: 3
    DEF_CARD(103, "Attack LV3",    ATTACK,  3, 3, 6), // Cost: 6
    DEF_CARD(201, "Defense LV1",   DEFENSE, 1, 1, 1), // Cost: 1
    DEF_CARD(202, "Defense LV2",   DEFENSE, 2, 2, 3), // Cost: 3
    DEF_CARD(203, "Defense LV3",   DEFENSE, 3, 3, 6), // Cost: 6
    DEF_CARD(301, "Move LV1",      MOVE,    1, 1, 1), // Cost: 1
    DEF_CARD(302, "Move LV2",      MOVE,    2, 2, 3), // Cost: 3
    DEF_CARD(303, "Move LV3",      MOVE,    3, 3, 6), // Cost: 6
    DEF_CARD(401, "Crystal",       GENERIC, 0, 1, 0),

    // Skill Cards (Cost is for playing the card from hand)
    DEF_CARD(501, "[Red Hood] Wolf Claw",        SKILL, 1, 2, 1),
    DEF_CARD(502, "[Snow White] Poison Apple",   SKILL, 1, 1, 2),
    DEF_CARD(503, "[Sleeping B.] Lily's Dream",  SKILL, 1, 0, 1),
    DEF_CARD(504, "[Alice] Mad Tea Party",       SKILL, 1, 0, 0),
    DEF_CARD(505, "[Mulan] Chi Slash",           SKILL, 1, 3, 2),
    DEF_CARD(506, "[Kaguya] Five Problems",      SKILL, 1, 0, 1),
    DEF_CARD(507, "[Mermaid] Bubble Song",       SKILL, 1, 0, 0),
    DEF_CARD(508, "[Match Girl] Last Match",     SKILL, 1, 5, 3),
    DEF_CARD(509, "[Dorothy] Tornado Road",      SKILL, 1, 1, 2),
    DEF_CARD(510, "[Scheherazade] 1001 Nights",  SKILL, 1, 0, 1),
    
    // Status Cards
    DEF_CARD(901, "[Status] Poisoned", STATUS, 0, 0, 0),
};
const int NUM_CARD_DEFINITIONS = sizeof(card_database) / sizeof(Card);

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
