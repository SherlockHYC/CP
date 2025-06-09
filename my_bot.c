#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include "client.h"
#include "architecture.h"
#include "vector.h"

// =================================================================
//                      卡牌與遊戲資料庫
// =================================================================
typedef enum { ATTACK, DEFENSE, MOVE, GENERIC, SKILL, STATUS } CardType;

typedef enum {
    PVB_CHOOSE_MOVE = 1000,
    PVB_USE_ATK = 1001,
    PVB_USE_SKILL = 1002,
    PVB_BUY_CARD = 1003,
    PVB_FOCUS_REMOVE = 1004,
    PVB_END_TURN = 999
} PvB_GameStatus;

typedef struct {
    int32_t id;
    const char* name;
    CardType type;
    int level;
    int value;
    int cost;
} Card;

#define DEF_CARD(id, name, type, lvl, val, cst) {id, name, type, lvl, val, cst}

Card card_database[] = {
    // 攻擊牌
    DEF_CARD(101, "攻擊 LV1", ATTACK, 1, 1, 2),
    DEF_CARD(102, "攻擊 LV2", ATTACK, 2, 2, 4),
    DEF_CARD(103, "攻擊 LV3", ATTACK, 3, 3, 6),
    // 防禦牌
    DEF_CARD(201, "防禦 LV1", DEFENSE, 1, 1, 2),
    DEF_CARD(202, "防禦 LV2", DEFENSE, 2, 2, 4),
    DEF_CARD(203, "防禦 LV3", DEFENSE, 3, 3, 6),
    // 移動牌
    DEF_CARD(301, "移動 LV1", MOVE, 1, 1, 1),
    DEF_CARD(302, "移動 LV2", MOVE, 2, 2, 2),
    DEF_CARD(303, "移動 LV3", MOVE, 3, 3, 3),
    // 通用牌
    DEF_CARD(401, "水晶", GENERIC, 0, 1, 0),
    // 角色技能牌
    DEF_CARD(501, "[小紅帽] 狼之爪擊", SKILL, 1, 2, 0),
    DEF_CARD(502, "[白雪公主] 毒蘋果", SKILL, 1, 1, 0),
    DEF_CARD(503, "[睡美人] 百合之夢", SKILL, 1, 0, 0),
    DEF_CARD(504, "[愛麗絲] 瘋狂茶會", SKILL, 1, 0, 0),
    DEF_CARD(505, "[花木蘭] 氣合斬", SKILL, 1, 3, 0),
    DEF_CARD(506, "[輝夜姬] 五道難題", SKILL, 1, 0, 0),
    DEF_CARD(507, "[美人魚] 泡沫之歌", SKILL, 1, 0, 0),
    DEF_CARD(508, "[火柴女孩] 最後的火柴", SKILL, 1, 5, 0),
    DEF_CARD(509, "[桃樂絲] 龍捲風之路", SKILL, 1, 1, 0),
    DEF_CARD(510, "[山魯佐德] 一千零一夜", SKILL, 1, 0, 0),
    // 狀態牌
    DEF_CARD(901, "[狀態] 中毒", STATUS, 0, 0, 0),
};
const int NUM_CARD_DEFINITIONS = sizeof(card_database) / sizeof(Card);

typedef enum { 
    RED_HOOD, SNOW_WHITE, SLEEPING_BEAUTY, ALICE, 
    MULAN, KAGUYA, MERMAID, MATCH_GIRL, 
    DOROTHY, SCHEHERAZADE 
} CharacterType;

const char* character_names[] = {
    "小紅帽", "白雪公主", "睡美人", "愛麗絲", 
    "花木蘭", "輝夜姬", "美人魚", "火柴女孩", 
    "桃樂絲", "山魯佐德"
};

const Card* get_card_info(int32_t card_id) {
    for (int i = 0; i < NUM_CARD_DEFINITIONS; ++i) {
        if (card_database[i].id == card_id) return &card_database[i];
    }
    return NULL;
}

// =================================================================
//                      輔助函式 (Helper Functions)
// =================================================================

int get_clean_integer_input() {
    int choice = -99;
    char term;
    if (scanf("%d%c", &choice, &term) != 2 || term != '\n') {
        printf("!! 輸入格式錯誤，請輸入一個數字並按下 Enter !!\n");
        while ((term = getchar()) != '\n' && term != EOF);
        return -99;
    }
    return choice;
}

bool is_attack_card(int32_t card_id) {
    const Card* card = get_card_info(card_id);
    return card && card->type == ATTACK;
}
bool is_skill_card(int32_t card_id) {
    const Card* card = get_card_info(card_id);
    return card && card->type == SKILL;
}

void print_vector_with_names(const char* name, const vector* v) {
    printf("%s (共 %u 張): [ ", name, v->SIZE);
    for (uint32_t i = 0; i < v->SIZE; ++i) {
        const Card* card = get_card_info(v->array[i]);
        if (card) printf("%d:%s ", i + 1, card->name);
        else printf("%d:未知(%d) ", i + 1, v->array[i]);
    }
    printf("]\n");
}

int32_t find_card_in_hand(const vector* hand, bool (*check_func)(int32_t)) {
    for (uint32_t i = 0; i < hand->SIZE; ++i) {
        if (check_func(hand->array[i])) return i + 1;
    }
    return 0;
}

// =================================================================
//                      Bot 的大腦 (Bot Logic)
// =================================================================

void my_bot_logic(const game* game_status, void* response_data, size_t* response_size) {
    const player* me = &game_status->players[game_status->now_turn_player_id];
    
    switch (game_status->status) {
        case PVB_CHOOSE_MOVE:
            if (me->energy > 0 && find_card_in_hand(&me->hand, is_skill_card) > 0) *(int32_t*)response_data = 2;
            else if (me->energy > 0 && find_card_in_hand(&me->hand, is_attack_card) > 0) *(int32_t*)response_data = 1;
            else if (me->energy >= 2) *(int32_t*)response_data = 6;
            else *(int32_t*)response_data = 10;
            break;
        case PVB_USE_ATK:
            *(int32_t*)response_data = find_card_in_hand(&me->hand, is_attack_card);
            break;
        case PVB_USE_SKILL:
            *(int32_t*)response_data = find_card_in_hand(&me->hand, is_skill_card);
            break;
        case PVB_BUY_CARD:
            *(int32_t*)response_data = 101;
            break;
        default:
            *(int32_t*)response_data = 0;
            break;
    }
     *response_size = sizeof(int32_t);
}

// =================================================================
//         玩家 vs. Bot 模式 (Player vs. Bot - PvB Mode)
// =================================================================

void shuffle_deck(vector* deck) {
    if (deck->SIZE < 2) return;
    for (uint32_t i = 0; i < deck->SIZE - 1; ++i) {
        uint32_t j = i + rand() % (deck->SIZE - i);
        int32_t temp = deck->array[i];
        deck->array[i] = deck->array[j];
        deck->array[j] = temp;
    }
}

void draw_card(player* p) {
    if (p->deck.SIZE == 0) {
        if (p->graveyard.SIZE == 0) return;
        p->deck = p->graveyard;
        p->graveyard = initVector();
        shuffle_deck(&p->deck);
        printf(">> 牌庫已空，重洗棄牌堆！ <<\n");
    }
    int32_t drawn_card = p->deck.array[p->deck.SIZE - 1];
    popbackVector(&p->deck);
    pushbackVector(&p->hand, drawn_card);
}

void display_supply_piles(const game* g) {
    printf("--- 商店供應區 ---\n");
    // 顯示攻擊/防禦/移動牌
    for (int type_idx = 0; type_idx < 3; ++type_idx) { // 0:ATK, 1:DEF, 2:MOV
        for (int lvl_idx = 0; lvl_idx < 3; ++lvl_idx) { // 0:LV1, 1:LV2, 2:LV3
            const vector* pile = &g->basicBuyDeck[type_idx][lvl_idx];
            if (pile->SIZE > 0) {
                const Card* card = get_card_info(pile->array[0]);
                if(card) {
                    printf("  [%d] %s (剩 %2u 張) - 成本: %d\n", 
                           card->id, card->name, pile->SIZE, card->cost);
                }
            }
        }
    }
    // 顯示通用牌
    const vector* generic_pile = &g->basicBuyDeck[3][0];
    if(generic_pile->SIZE > 0) {
        const Card* card = get_card_info(generic_pile->array[0]);
        if(card) {
             printf("  [%d] %s (剩 %2u 張) - 成本: %d\n", 
                           card->id, card->name, generic_pile->SIZE, card->cost);
        }
    }
    printf("--------------------\n");
}


void display_pvb_state(const game* g) {
    printf("\n\n=============== 遊戲狀態 ===============\n");
    const player* human = &g->players[0];
    const player* bot = &g->players[1];
    printf("  [你] 玩家 0 (%s): 生命 %2u, 防禦 %u, 能量 %u | 牌庫: %u, 棄牌堆: %u\n", 
           character_names[human->character], human->life, human->defense, human->energy, human->deck.SIZE, human->graveyard.SIZE);
    print_vector_with_names("    你的手牌", &human->hand);
    printf("  [敵] BOT  1 (%s): 生命 %2u, 防禦 %u, 能量 %u\n", character_names[bot->character], bot->life, bot->defense, bot->energy);
    printf("======================================\n");
}

void get_human_input(game* game_status, void* response_data, size_t* response_size) {
    int choice = -1;
    switch (game_status->status) {
        case PVB_CHOOSE_MOVE:
            printf(">>> 你的回合！請選擇行動 (0:專注, 1:攻擊, 2:技能, 6:買牌, 10:結束回合): ");
            choice = get_clean_integer_input();
            break;
        case PVB_USE_ATK:
            printf(">>> 請選擇要使用的攻擊牌 (輸入 1-based 索引): ");
            choice = get_clean_integer_input();
            break;
        case PVB_USE_SKILL:
             printf(">>> 請選擇要使用的技能牌 (輸入 1-based 索引): ");
            choice = get_clean_integer_input();
            break;
        case PVB_BUY_CARD:
            display_supply_piles(game_status);
            printf(">>> 請輸入想購買的卡牌 ID (輸入 0 取消): ");
            choice = get_clean_integer_input();
            break;
        case PVB_FOCUS_REMOVE:
            printf(">>> [專注] 請選擇要移除的牌。\n");
            print_vector_with_names("手牌", &game_status->players[0].hand);
            print_vector_with_names("棄牌堆", &game_status->players[0].graveyard);
            printf(">>> (輸入手牌索引 1~5, 或棄牌堆索引 -1, -2...): ");
            choice = get_clean_integer_input();
            break;
    }
    *(int32_t*)response_data = choice;
    *response_size = sizeof(int32_t);
}

void apply_damage(player* defender, int damage) {
    printf("造成 %d 點傷害！\n", damage);
    if (defender->defense >= damage) { defender->defense -= damage; }
    else { defender->life -= (damage - defender->defense); defender->defense = 0; }
}

void apply_card_effect(game* g, int32_t card_index) {
     player* attacker = &g->players[g->now_turn_player_id];
     if (card_index <= 0 || card_index > attacker->hand.SIZE) { printf("錯誤：無效的卡牌索引！\n"); return; }
    
    const Card* card = get_card_info(attacker->hand.array[card_index-1]);
    if (!card) { printf("錯誤：未知的卡牌！\n"); return; }
    
    if ((g->status == PVB_USE_ATK && card->type != ATTACK) || (g->status == PVB_USE_SKILL && card->type != SKILL)) {
        printf("錯誤：選擇的卡牌類型與行動不符！\n"); return;
    }

    if(attacker->energy <= 0){ printf("錯誤：能量不足！\n"); return; }
    attacker->energy--;

    printf("行動結果：玩家 %d 使用了 [%s]！\n", g->now_turn_player_id, card->name);
    
    if (card->type == ATTACK || card->type == SKILL) { if(card->value > 0) apply_damage(&g->players[(g->now_turn_player_id + 1) % 2], card->value); }
    else if (card->type == GENERIC) { attacker->energy += card->value; printf(">>> [%s] 的效果，能量增加了 %d！\n", card->name, card->value); }

    switch(card->id) {
        case 502: printf(">>> [%s] 的效果，將一張 [中毒] 卡牌置入對手棄牌堆！\n", card->name); pushbackVector(&g->players[(g->now_turn_player_id + 1) % 2].graveyard, 901); break;
        case 503: attacker->defense += 2; printf(">>> [%s] 的效果，防禦增加了 2！\n", card->name); break;
        case 504: attacker->energy += 1; draw_card(attacker); printf(">>> [%s] 的效果，能量 +1 並抽 1 張牌！\n", card->name); break;
        case 506: apply_damage(&g->players[(g->now_turn_player_id + 1) % 2], 3); break;
        case 507: attacker->energy += 2; printf(">>> [%s] 的效果，能量增加了 2！\n", card->name); break;
        case 509: draw_card(attacker); printf(">>> [%s] 的效果，造成傷害並抽 1 張牌！\n", card->name); break;
    }
    
    int32_t used_card = attacker->hand.array[card_index - 1];
    eraseVector(&attacker->hand, card_index - 1);
    pushbackVector(&attacker->graveyard, used_card);
}

void apply_focus_remove(game* g, int32_t choice) {
    player* p = &g->players[g->now_turn_player_id];
    if (choice > 0 && choice <= p->hand.SIZE) {
        eraseVector(&p->hand, choice - 1); printf("成功從手牌移除了第 %d 張牌。\n", choice);
    } else if (choice < 0 && -choice <= p->graveyard.SIZE) {
        eraseVector(&p->graveyard, -choice - 1); printf("成功從棄牌堆移除了第 %d 張牌。\n", -choice);
    } else {
        printf("無效的選擇，未移除任何牌。\n");
    }
}

void apply_buy_card(game* g, int32_t card_id) {
    if (card_id == 0) return; // 取消購買
    player* buyer = &g->players[g->now_turn_player_id];
    const Card* card_to_buy = get_card_info(card_id);

    if (!card_to_buy) { printf("錯誤：商店沒有這張牌！\n"); return; }
    if (buyer->energy < card_to_buy->cost) { printf("錯誤：能量不足！\n"); return; }

    bool found_in_supply = false;
    for(int i=0; i < 4; ++i) { // 4 種牌堆類型
        for(int j=0; j < 3; ++j) {
            vector* pile = &g->basicBuyDeck[i][j];
            if(pile->SIZE > 0 && pile->array[0] == card_id) {
                popbackVector(pile); found_in_supply = true; break;
            }
        }
        if(found_in_supply) break;
    }
    
    if(!found_in_supply) { printf("錯誤：該卡牌已售罄！\n"); return; }

    buyer->energy -= card_to_buy->cost;
    pushbackVector(&buyer->graveyard, card_id);
    printf("行動結果：玩家 %d 成功購買 [%s]！\n", g->now_turn_player_id, card_to_buy->name);
}


void apply_action(game* g, int32_t action_choice) {
    switch (action_choice) {
        case 0: g->status = PVB_FOCUS_REMOVE; break;
        case 1: g->status = PVB_USE_ATK; break;
        case 2: g->status = PVB_USE_SKILL; break;
        case 6: g->status = PVB_BUY_CARD; break;
        case 10: g->status = PVB_END_TURN; break;
        default: printf("!! 無效的行動選擇 !!\n"); break;
    }
}

void init_player_deck(player* p, CharacterType character) {
    p->character = character; p->deck = initVector(); p->hand = initVector(); p->graveyard = initVector(); p->life = 20;
    for(int k=0; k<7; ++k) pushbackVector(&p->deck, 101);
    for(int k=0; k<3; ++k) pushbackVector(&p->deck, 401);
    switch(character) {
        case RED_HOOD: for(int k=0; k<2; ++k) pushbackVector(&p->deck, 501); break;
        case SNOW_WHITE: for(int k=0; k<2; ++k) pushbackVector(&p->deck, 502); break;
        case SLEEPING_BEAUTY: for(int k=0; k<2; ++k) pushbackVector(&p->deck, 503); break;
        case ALICE: for(int k=0; k<2; ++k) pushbackVector(&p->deck, 504); break;
        case MULAN: for(int k=0; k<2; ++k) pushbackVector(&p->deck, 505); break;
        case KAGUYA: for(int k=0; k<2; ++k) pushbackVector(&p->deck, 506); break;
        case MERMAID: for(int k=0; k<2; ++k) pushbackVector(&p->deck, 507); break;
        case MATCH_GIRL: for(int k=0; k<2; ++k) pushbackVector(&p->deck, 508); break;
        case DOROTHY: for(int k=0; k<2; ++k) pushbackVector(&p->deck, 509); break;
        case SCHEHERAZADE: for(int k=0; k<2; ++k) pushbackVector(&p->deck, 510); break;
    }
    shuffle_deck(&p->deck);
}


void run_pvb_mode() {
    srand(time(NULL));
    game pvb_game;
    memset(&pvb_game, 0, sizeof(game));

    printf("請選擇你的角色:\n");
    for(int i=0; i < 10; ++i) printf("  %d: %s\n", i, character_names[i]);
    printf("你的選擇: ");
    int choice = get_clean_integer_input();
    
    CharacterType human_char = (choice >= 0 && choice < 10) ? (CharacterType)choice : RED_HOOD;
    CharacterType bot_char = (CharacterType)(rand() % 10);

    init_player_deck(&pvb_game.players[0], human_char);
    init_player_deck(&pvb_game.players[1], bot_char);

    // 初始化商店
    for(int i=0; i<18; ++i) {
        pushbackVector(&pvb_game.basicBuyDeck[0][0], 101); // ATK LV1
        pushbackVector(&pvb_game.basicBuyDeck[0][1], 102); // ATK LV2
        pushbackVector(&pvb_game.basicBuyDeck[0][2], 103); // ATK LV3
        pushbackVector(&pvb_game.basicBuyDeck[1][0], 201); // DEF LV1
        pushbackVector(&pvb_game.basicBuyDeck[1][1], 202); // DEF LV2
        pushbackVector(&pvb_game.basicBuyDeck[1][2], 203); // DEF LV3
        pushbackVector(&pvb_game.basicBuyDeck[2][0], 301); // MOV LV1
        pushbackVector(&pvb_game.basicBuyDeck[2][1], 302); // MOV LV2
        pushbackVector(&pvb_game.basicBuyDeck[2][2], 303); // MOV LV3
        pushbackVector(&pvb_game.basicBuyDeck[3][0], 401); // GENERIC (水晶)
    }


    while (pvb_game.players[0].life > 0 && pvb_game.players[1].life > 0) {
        player* p = &pvb_game.players[pvb_game.now_turn_player_id];
        p->energy = 3; 
        while(p->hand.SIZE < 5) draw_card(p);

        while (true) {
            display_pvb_state(&pvb_game);
            pvb_game.status = PVB_CHOOSE_MOVE;
            
            char response_buffer[256]; size_t response_size;
            if (pvb_game.now_turn_player_id == 0) get_human_input(&pvb_game, response_buffer, &response_size);
            else { my_bot_logic(&pvb_game, response_buffer, &response_size); sleep(1); }
            
            apply_action(&pvb_game, *(int32_t*)response_buffer);
            if (pvb_game.status == PVB_END_TURN) break;

            if (pvb_game.status == PVB_USE_ATK || pvb_game.status == PVB_USE_SKILL) {
                 if (pvb_game.now_turn_player_id == 0) get_human_input(&pvb_game, response_buffer, &response_size);
                 else my_bot_logic(&pvb_game, response_buffer, &response_size);
                 apply_card_effect(&pvb_game, *(int32_t*)response_buffer);
            } else if (pvb_game.status == PVB_FOCUS_REMOVE) {
                if (pvb_game.now_turn_player_id == 0) get_human_input(&pvb_game, response_buffer, &response_size);
                else { *(int32_t*)response_buffer = 0; }
                apply_focus_remove(&pvb_game, *(int32_t*)response_buffer);
                break;
            } else if (pvb_game.status == PVB_BUY_CARD) {
                if (pvb_game.now_turn_player_id == 0) get_human_input(&pvb_game, response_buffer, &response_size);
                else my_bot_logic(&pvb_game, response_buffer, &response_size);
                apply_buy_card(&pvb_game, *(int32_t*)response_buffer);
            }
        }
        pvb_game.now_turn_player_id = (pvb_game.now_turn_player_id + 1) % 2;
    }
    
    printf("\n!!!!!!!!!! 遊戲結束 !!!!!!!!!!\n");
    if (pvb_game.players[0].life <= 0) printf("你輸了！\n");
    else printf("恭喜你，你贏了！\n");
}


void show_help() {
    printf("\n--- Twisted Fables 遊戲說明 ---\n");
    printf("這是一個 1v1 的卡牌對戰遊戲，目標是將對手的生命值降到 0。\n\n");
    printf("【遊戲模式】\n");
    printf("  ./my_bot --pvb         : 玩家 vs. Bot 模式。\n");
    printf("  ./my_bot --help        : 顯示此說明訊息。\n\n");
    printf("【回合流程】\n");
    printf("  在你的回合，你可以執行多次行動，直到你選擇結束回合，或是執行了「專注」。\n\n");
    printf("【可用指令】\n");
    printf("  - 0 (專注): [會結束回合] 讓你從手牌或棄牌堆中永久移除一張牌，以優化牌庫。\n");
    printf("  - 1 (攻擊): 消耗 1 點能量，進入「選擇攻擊牌」階段。\n");
    printf("  - 2 (技能): 消耗 1 點能量，進入「選擇技能牌」階段。\n");
    printf("  - 6 (買牌): 消耗能量購買更強的卡牌強化牌組。\n");
    printf("  - 10(結束回合): 放棄所有剩餘行動，結束你的回合。\n\n");
    printf("祝您遊戲愉快！\n\n");
}
void run_local_mode() { printf("舊的 local 模式已被 PvB 模式取代，請使用 --pvb 進行本機測試。\n"); }
void run_online_mode() { printf("網路連線模式尚未設定正確的 IP，暫時無法使用。\n"); }


int main(int argc, char *argv[]) {
    if (argc > 1) {
        if (strcmp(argv[1], "--help") == 0) show_help();
        else if (strcmp(argv[1], "--pvb") == 0) run_pvb_mode();
        else if (strcmp(argv[1], "--local") == 0) run_local_mode();
        else run_online_mode();
    } else {
        run_online_mode();
    }
    return 0;
}
