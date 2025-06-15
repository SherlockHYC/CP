#include "bot_ai.h"
#include "definitions.h"
#include "vector.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int planned_actions[16];   // 最多 16 張出牌計劃
static int planned_count = 0;
static int planned_index = 0;

// 判斷卡片類型是否一致
bool is_card_type(int32_t card_id, CardType type) {
    const Card* card = get_card_info(card_id);
    return card && card->type == type;
}

// 判斷能否支付費用（技能卡不需能量）
bool card_cost_affordable(const Card* card, int energy) {
    if (!card) return false;
    if (card->type == SKILL) return true; // 技能卡不消耗能量
    return card->cost <= energy;
}

// 根據技能卡 ID 回傳需要捨棄的基本牌類型（限定支援角色：小紅帽與白雪公主）
CardType get_skill_sacrifice_type(int32_t skill_card_id) {
    if (skill_card_id == 501 || skill_card_id == 511) return ATTACK;
    if (skill_card_id == 502 || skill_card_id == 512) return DEFENSE;
    if (skill_card_id == 503 || skill_card_id == 513) return MOVE;
    return GENERIC; // 預設無需捨牌
}

int get_bot_action(const game* game_state) {
    // [BOT AI] 檢查目前是否已規劃好出牌計劃
    const player* me = &game_state->players[game_state->now_turn_player_id];
    const player* enemy = &game_state->players[(game_state->now_turn_player_id + 1) % 2];

    if (planned_index < planned_count) {
        int card_idx = planned_actions[planned_index++];
        const Card* card = get_card_info(me->hand.array[card_idx]);
        if (card) {
            printf("[BOT] Playing card: ID=%d, Name=%s, Type=%d, Cost=%d\n", card->id, card->name, card->type, card->cost);
        }
        return card_idx; // 已規劃好，直接出牌
    }

    planned_index = planned_count = 0;

    vector attack_idxs, defense_idxs, move_idxs, skill_idxs, other_idxs;
    attack_idxs.SIZE = 0;
    defense_idxs.SIZE = 0;
    move_idxs.SIZE = 0;
    skill_idxs.SIZE = 0;
    other_idxs.SIZE = 0;

    // 分類手牌
    for (uint32_t i = 0; i < me->hand.SIZE; ++i) {
        int32_t id = me->hand.array[i];
        const Card* card = get_card_info(id);
        if (!card) continue;

        switch (card->type) {
            case ATTACK: pushbackVector(&attack_idxs, i); break;
            case DEFENSE: pushbackVector(&defense_idxs, i); break;
            case MOVE: pushbackVector(&move_idxs, i); break;
            case SKILL: pushbackVector(&skill_idxs, i); break;
            default: pushbackVector(&other_idxs, i); break;
        }
    }

    // 嘗試為每張技能卡建立出牌計劃（必須能捨棄對應基本牌）
    for (uint32_t i = 0; i < skill_idxs.SIZE; ++i) {
        int skill_idx = skill_idxs.array[i];
        int32_t skill_id = me->hand.array[skill_idx];
        const Card* skill_card = get_card_info(skill_id);
        if (!skill_card) continue;

        CardType required_type = get_skill_sacrifice_type(skill_id);
        vector* pool = NULL;
        if (required_type == ATTACK) pool = &attack_idxs;
        else if (required_type == DEFENSE) pool = &defense_idxs;
        else if (required_type == MOVE) pool = &move_idxs;

        if (pool && pool->SIZE >= 1) {
            planned_actions[planned_count++] = skill_idx;

            // 移除犧牲牌（第一張即可）
            for (int k = 0; k < pool->SIZE - 1; ++k) {
                pool->array[k] = pool->array[k + 1];
            }
            pool->SIZE--;

            // 技能後繼續出所有能出的基本牌（先防再移再打）
            for (uint32_t j = 0; j < defense_idxs.SIZE; ++j) {
                const Card* c = get_card_info(me->hand.array[defense_idxs.array[j]]);
                if (card_cost_affordable(c, me->energy)) {
                    planned_actions[planned_count++] = defense_idxs.array[j];
                }
            }
            for (uint32_t j = 0; j < move_idxs.SIZE; ++j) {
                const Card* c = get_card_info(me->hand.array[move_idxs.array[j]]);
                if (card_cost_affordable(c, me->energy)) {
                    planned_actions[planned_count++] = move_idxs.array[j];
                }
            }
            for (uint32_t j = 0; j < attack_idxs.SIZE; ++j) {
                const Card* c = get_card_info(me->hand.array[attack_idxs.array[j]]);
                if (card_cost_affordable(c, me->energy)) {
                    planned_actions[planned_count++] = attack_idxs.array[j];
                }
            }
            break; // 一次只規劃一個技能行動
        }
    }

    // 沒技能卡，出基本牌：先防再移再打
    if (planned_count == 0) {
        for (uint32_t i = 0; i < defense_idxs.SIZE; ++i) {
            const Card* c = get_card_info(me->hand.array[defense_idxs.array[i]]);
            if (card_cost_affordable(c, me->energy)) {
                planned_actions[planned_count++] = defense_idxs.array[i];
            }
        }
        for (uint32_t i = 0; i < move_idxs.SIZE; ++i) {
            const Card* c = get_card_info(me->hand.array[move_idxs.array[i]]);
            if (card_cost_affordable(c, me->energy)) {
                planned_actions[planned_count++] = move_idxs.array[i];
            }
        }
        for (uint32_t i = 0; i < attack_idxs.SIZE; ++i) {
            const Card* c = get_card_info(me->hand.array[attack_idxs.array[i]]);
            if (card_cost_affordable(c, me->energy)) {
                planned_actions[planned_count++] = attack_idxs.array[i];
            }
        }
    }

    // 執行第一張計劃
    if (planned_count > 0) {
        int card_idx = planned_actions[planned_index++];
        const Card* card = get_card_info(me->hand.array[card_idx]);
        if (card) {
            printf("[BOT] Playing card: ID=%d, Name=%s, Type=%d, Cost=%d\n", card->id, card->name, card->type, card->cost);
        }
        return card_idx;
    }

    return -1;
}
