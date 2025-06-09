#include "bot_ai.h"
#include "definitions.h"
#include <stdbool.h>
#include <stddef.h>

// 輔助函式：判斷是否為技能牌
bool is_skill_card(int32_t card_id) {
    const Card* card = get_card_info(card_id);
    return card && card->type == SKILL;
}

// 輔助函式：判斷是否為攻擊牌
bool is_attack_card(int32_t card_id) {
    const Card* card = get_card_info(card_id);
    return card && card->type == ATTACK;
}

// Bot 的主要決策函式
int get_bot_action(const game* game_state) {
    const player* me = &game_state->players[game_state->now_turn_player_id];

    // 如果沒有能量，就無法行動
    if (me->energy <= 0) return -1;

    // --- 角色專屬的優先決策 ---
    // AI 會根據自己的角色，優先考慮使用最有利的技能
    switch (me->character) {
        // 資源型角色：優先使用能產生能量或抽牌的技能
        case ALICE:
        case MERMAID:
        case DOROTHY: {
            for (uint32_t i = 0; i < me->hand.SIZE; ++i) {
                int32_t card_id = me->hand.array[i];
                // 愛麗絲(504), 美人魚(507), 桃樂絲(509) 的牌能帶來額外優勢
                if (card_id == 504 || card_id == 507 || card_id == 509) {
                    return i; // 立刻決定使用這張資源牌
                }
            }
            break; // 如果沒有，則進入下方的通用邏輯
        }
        
        // 高傷害角色：優先使用強力攻擊技能
        case MATCH_GIRL: {
            for (uint32_t i = 0; i < me->hand.SIZE; ++i) {
                if (me->hand.array[i] == 508) return i; // 找到最後的火柴就立刻使用
            }
            break;
        }

        // 其他角色可以陸續加入專屬的判斷邏輯...
    }

    // --- 通用的後備邏輯 ---
    // 如果沒有觸發任何角色專屬的優先行動，則執行以下通用策略

    // 1. 尋找並使用任何可用的技能牌
    for (uint32_t i = 0; i < me->hand.SIZE; ++i) {
        if (is_skill_card(me->hand.array[i])) {
            return i; // 回傳要打出的手牌索引 (0-based)
        }
    }
    
    // 2. 如果沒有技能牌，則尋找並使用任何可用的攻擊牌
    for (uint32_t i = 0; i < me->hand.SIZE; ++i) {
        if (is_attack_card(me->hand.array[i])) {
            return i;
        }
    }

    // 3. 如果沒有任何可以出的牌，則不行動
    return -1;
}
