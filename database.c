#include "definitions.h"
#include <stddef.h> // for NULL

#define DEF_CARD(id, name, type, lvl, val, cst) {id, name, type, lvl, val, cst}


const Card card_database[] = {
    
    /*
    基本卡牌ID含意
    1(攻擊 防禦 移動) 0(沒有意義) 1(卡片等級)
    */

    // Basic Cards (Cost is for buying from the shop)
    DEF_CARD(101, "Attack LV1",      ATTACK,  1, 1, 1),
    DEF_CARD(102, "Attack LV2",      ATTACK,  2, 2, 3),
    DEF_CARD(103, "Attack LV3",      ATTACK,  3, 3, 6),
    DEF_CARD(201, "Defense LV1",     DEFENSE, 1, 1, 1),
    DEF_CARD(202, "Defense LV2",     DEFENSE, 2, 2, 3),
    DEF_CARD(203, "Defense LV3",     DEFENSE, 3, 3, 6),
    DEF_CARD(301, "Move LV1",        MOVE,    1, 1, 1),
    DEF_CARD(302, "Move LV2",        MOVE,    2, 2, 3),
    DEF_CARD(303, "Move LV3",        MOVE,    3, 3, 6),
    DEF_CARD(401, "Crystal",         GENERIC, 0, 1, 0),

    /*
    技能卡牌ID含意
    5(技能等級5為等級1，6為等級2，7為等級3) 0(角色ID小紅帽) 1(攻擊 防禦 移動)
    */

    // --- Character Skill Cards ---
    // [Red Hood]
    DEF_CARD(501, "[RH] Wolf Claw",      SKILL, 1, 2, 1), // ATK
    DEF_CARD(601, "[RH] 精準射擊",        SKILL, 2, 2, 2), // 射程2 傷害2+O
    DEF_CARD(701, "[RH] 致命狙擊",        SKILL, 3, 3, 4), // 射程3 傷害3+0
    
    DEF_CARD(502, "[RH] Dodge",          SKILL, 1, 0, 1), // DEF
    DEF_CARD(602, "[RH] 電流護盾",        SKILL, 2, 2, 2), // 射程2 傷害2
    DEF_CARD(702, "[RH] 終極護盾",        SKILL, 3, 3, 4), // 射程3 傷害3
    
    DEF_CARD(503, "[RH] Quick Step",     SKILL, 1, 0, 1), // MOV
    DEF_CARD(603, "[RH] 火力噴射",        SKILL, 2, 2, 2), // 射程2 傷害2 擊退
    DEF_CARD(703, "[RH] 暴怒噴射",        SKILL, 3, 3, 4), // 射程3 傷害3 擊退

    
    // [Snow White]
    DEF_CARD(511, "[SW] Poison Apple",   SKILL, 1, 1, 2), // ATK
    DEF_CARD(512, "[SW] Dwarves' Guard", SKILL, 1, 0, 1), // DEF
    DEF_CARD(513, "[SW] Mirror Transport",SKILL, 1, 0, 1), // MOV
    // [Sleeping Beauty]
    DEF_CARD(521, "[SB] Nightmare Lash", SKILL, 1, 2, 1), // ATK
    DEF_CARD(522, "[SB] Thorn Guard",    SKILL, 1, 0, 1), // DEF
    DEF_CARD(523, "[SB] Somnambulism",   SKILL, 1, 0, 1), // MOV
    // [Alice]
    DEF_CARD(531, "[AL] Queen's Command",SKILL, 1, 1, 1), // ATK
    DEF_CARD(532, "[AL] Croquet Guard",  SKILL, 1, 0, 1), // DEF
    DEF_CARD(533, "[AL] Down the Hole",  SKILL, 1, 0, 1), // MOV
    // [Mulan]
    DEF_CARD(541, "[MU] Chi Slash",      SKILL, 1, 3, 2), // ATK
    DEF_CARD(542, "[MU] Deflect",        SKILL, 1, 0, 1), // DEF
    DEF_CARD(543, "[MU] Rapid March",    SKILL, 1, 0, 1), // MOV
    // [Kaguya]
    DEF_CARD(551, "[KA] Celestial Punishment", SKILL, 1, 2, 2), // ATK
    DEF_CARD(552, "[KA] Jeweled Branch", SKILL, 1, 0, 1), // DEF
    DEF_CARD(553, "[KA] Moonlit Escape", SKILL, 1, 0, 1), // MOV
    // [Mermaid]
    DEF_CARD(561, "[MM] Aqua Jet",       SKILL, 1, 1, 1), // ATK
    DEF_CARD(562, "[MM] Coral Shield",   SKILL, 1, 0, 1), // DEF
    DEF_CARD(563, "[MM] Tidal Surge",    SKILL, 1, 0, 1), // MOV
    // [Match Girl]
    DEF_CARD(571, "[MG] Last Match",     SKILL, 1, 5, 3), // ATK
    DEF_CARD(572, "[MG] Warm Shelter",   SKILL, 1, 0, 1), // DEF
    DEF_CARD(573, "[MG] Fleeting Hope",  SKILL, 1, 0, 1), // MOV
    // [Dorothy]
    DEF_CARD(581, "[DO] Tornado Road",   SKILL, 1, 1, 2), // ATK
    DEF_CARD(582, "[DO] Scarecrow's Ward",SKILL, 1, 0, 1), // DEF
    DEF_CARD(583, "[DO] Yellow Brick Path",SKILL, 1, 0, 1),// MOV
    // [Scheherazade]
    DEF_CARD(591, "[SH] Punishing Tale", SKILL, 1, 2, 1), // ATK
    DEF_CARD(592, "[SH] Tale of Defense",SKILL, 1, 0, 1), // DEF
    DEF_CARD(593, "[SH] Story-Hopping",  SKILL, 1, 0, 1), // MOV
    
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
