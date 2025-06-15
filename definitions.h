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

// Function prototype for our database helper
const Card* get_card_info(int32_t card_id);

#endif // DEFINITIONS_H
