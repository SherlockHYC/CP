#include "bot_ai.h"
#include "definitions.h"
#include <stdbool.h>
#include <stddef.h>

// 輔助函式
bool is_skill_card(int32_t card_id) {
    const Card* card = get_card_info(card_id);
    return card && card->type == SKILL;
}

bool is_attack_card(int32_t card_id) {
    const Card* card = get_card_info(card_id);
    return card && card->type == ATTACK;
}

// [新增] 判斷是否為防禦牌
bool is_defense_card(int32_t card_id) {
    const Card* card = get_card_info(card_id);
    return card && card->type == DEFENSE;
}


// Bot 的主要決策函式
int get_bot_action(const game* game_state) {
    const player* me = &game_state->players[game_state->now_turn_player_id];

    if (me->energy <= 0) return -1;

    // --- [修改] 更新 Bot 的決策邏輯 ---
    // 1. 優先使用技能牌
    for (uint32_t i = 0; i < me->hand.SIZE; ++i) {
        if (is_skill_card(me->hand.array[i])) {
            return i;
        }
    }
    
    // 2. 如果生命值低，優先使用防禦牌
    if (me->life < 10) {
        for (uint32_t i = 0; i < me->hand.SIZE; ++i) {
            if (is_defense_card(me->hand.array[i])) {
                return i;
            }
        }
    }

    // 3. 其次使用攻擊牌
    for (uint32_t i = 0; i < me->hand.SIZE; ++i) {
        if (is_attack_card(me->hand.array[i])) {
            return i;
        }
    }
    
    // 4. 如果還有能量，就把剩下的防禦牌打出
    for (uint32_t i = 0; i < me->hand.SIZE; ++i) {
        if (is_defense_card(me->hand.array[i])) {
            return i;
        }
    }

    return -1; // 沒有任何可以出的牌
}
