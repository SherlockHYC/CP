#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <stdint.h>
#include <stdbool.h>

// 卡牌類型
typedef enum { ATTACK, DEFENSE, MOVE, GENERIC, SKILL, STATUS } CardType;

// 卡牌結構
typedef struct {
    int32_t id;
    const char* name;
    CardType type;
    int level;
    int value; // 效果值 (傷害/防禦等)
    int cost;  // 購買成本
} Card;

// 角色類型
typedef enum { 
    RED_HOOD, SNOW_WHITE, SLEEPING_BEAUTY, ALICE, 
    MULAN, KAGUYA, MERMAID, MATCH_GIRL, 
    DOROTHY, SCHEHERAZADE 
} CharacterType;

// 函式原型，讓其他檔案知道這個函式存在
const Card* get_card_info(int32_t card_id);

#endif // DEFINITIONS_H
