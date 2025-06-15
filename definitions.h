#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <stdint.h>
#include <stdbool.h>

// [新增] 將卡牌尺寸定義移到此處
#define CARD_WIDTH 120
#define CARD_HEIGHT 170



// Card and Character types that all files need to know about
typedef enum { ATTACK, DEFENSE, MOVE, GENERIC, SKILL, STATUS, SUPER } CardType;

typedef struct {
    int32_t id;
    const char* name;
    CardType type;
    int level;
    int value; // e.g., damage for ATTACK, points for DEFENSE
    int cost;  // energy cost to buy
} Card;

typedef enum { 
    RED_HOOD, SNOW_WHITE, SLEEPING_BEAUTY, ALICE, 
    MULAN, KAGUYA, MERMAID, MATCH_GIRL, 
    DOROTHY, SCHEHERAZADE 
} CharacterType;

typedef struct {
    int max_hp;
    int max_shield;
    int ultra_threshold;
} CharacterStats;

static const CharacterStats character_stats[] = {
    {30, 6, 15}, // 小紅帽
    {34, 6, 17}, // 白雪公主
    {42, 6, 21}, // 睡美人
    {32, 6, 16}, // 愛麗絲
    {34, 3, 17}, // 花木蘭
    {32, 6, 16}, // 輝夜姬
    {36, 3, 18}, // 美人魚
    {36, 6, 18}, // 火柴女孩
    {40, 6, 20}, // 桃樂絲
    {36, 6, 18}  // 山魯佐德
};

// Function prototype for our database helper
const Card* get_card_info(int32_t card_id);

#endif // DEFINITIONS_H
