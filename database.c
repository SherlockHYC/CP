#include "definitions.h"
#include <stddef.h> // for NULL

#define DEF_CARD(id, name, type, lvl, val, cst) {id, name, type, lvl, val, cst}


const Card card_database[] = {
    
    /*
    基本卡牌ID含意
    1(攻擊 防禦 移動) 0(沒有意義) 1(卡片等級)
    */

    // Basic Cards (Cost is for buying from the shop)
    DEF_CARD(101, "Atk LV1",      ATTACK,  1, 1, 1),
    DEF_CARD(102, "Atk LV2",      ATTACK,  2, 2, 3),
    DEF_CARD(103, "Atk LV3",      ATTACK,  3, 3, 6),
    DEF_CARD(201, "Def LV1",     DEFENSE, 1, 1, 1),
    DEF_CARD(202, "Def LV2",     DEFENSE, 2, 2, 3),
    DEF_CARD(203, "Def LV3",     DEFENSE, 3, 3, 6),
    DEF_CARD(301, "Move LV1",        MOVE,    1, 1, 1),
    DEF_CARD(302, "Move LV2",        MOVE,    2, 2, 3),
    DEF_CARD(303, "Move LV3",        MOVE,    3, 3, 6),
    DEF_CARD(401, "Crystal",         GENERIC, 0, 1, 0),

    /*
    技能卡牌ID含意
    5(技能等級5為等級1，6為等級2，7為等級3) 0(角色ID小紅帽) 1(攻擊 防禦 移動)
    */
    //後面三個數字分別是等級 傷害 花費
    // --- Character Skill Cards ---
    // [Red Hood]
    // ATK
    DEF_CARD(501, "快速射擊",        SKILL, 1, 1, 0), // 等級1 傷害1+x 花費0 
    DEF_CARD(601, "精準射擊",        SKILL, 2, 2, 2), // 等級2 傷害2+x 花費2
    DEF_CARD(701, "致命狙擊",        SKILL, 3, 3, 4), // 等級3 傷害3+x 花費4
    
    // DEF
    DEF_CARD(502, "能量護盾",        SKILL, 1, 1, 0), // 等級1 傷害1 花費0
    DEF_CARD(602, "電流護盾",        SKILL, 2, 2, 2), // 等級2 傷害2 花費2
    DEF_CARD(702, "終極護盾",        SKILL, 3, 3, 4), // 等級3 傷害3 花費4
    
    // MOV
    DEF_CARD(503, "彈道噴射",        SKILL, 1, 1, 0), // 等級1 傷害1 花費0
    DEF_CARD(603, "火力噴射",        SKILL, 2, 2, 2), // 等級2 傷害2 花費2
    DEF_CARD(703, "暴怒噴射",        SKILL, 3, 3, 4), // 等級3 傷害3 花費4

    
    // [Snow White]
    // ATK
    DEF_CARD(511, "水晶碎片",        SKILL, 1, 1, 0), // 等級1 傷害1+x 花費0    
    DEF_CARD(611, "水晶漩渦",        SKILL, 2, 2, 2), // 等級2 傷害2+x 花費2
    DEF_CARD(711, "水晶風暴",        SKILL, 3, 3, 4), // 等級3 傷害3+x 花費4

    // DEF
    DEF_CARD(512, "玷污的恩惠",      SKILL, 1, 1, 0), // 等級1 傷害1 花費0
    DEF_CARD(612, "玷污的盛筵",      SKILL, 2, 2, 2), // 等級2 傷害2 花費2
    DEF_CARD(712, "玷污的狂歡",      SKILL, 3, 3, 4), // 等級3 傷害3 花費4

    // MOV
    DEF_CARD(513, "破碎的幻想",      SKILL, 1, 1, 0), // 等級1 傷害1 花費0
    DEF_CARD(613, "破碎的現實",      SKILL, 2, 2, 2), // 等級2 傷害2 花費2
    DEF_CARD(713, "破碎的命運",      SKILL, 3, 3, 4), // 等級3 傷害3 花費4

    // [Sleeping Beauty]
    // ATK
    DEF_CARD(521, "心靈震顫",        SKILL, 1, 1, 0), // 等級1 傷害(1*x)+y,y最多為1 花費0
    DEF_CARD(621, "心靈之怒",        SKILL, 2, 2, 2), // 等級2 傷害(2*x)+y,y最多為2 花費2
    DEF_CARD(721, "心靈狂怒",        SKILL, 3, 3, 4), // 等級3 傷害(3*x)+y,y最多為3 花費4

    // DEF
    DEF_CARD(522, "爆裂之鎖",        SKILL, 1, 0, 0), // 等級1 傷害(其他) 花費0
    DEF_CARD(622, "爆裂之骨",        SKILL, 2, 0, 2), // 等級2 傷害(其他) 花費2
    DEF_CARD(722, "爆裂之魂",        SKILL, 3, 0, 4), // 等級3 傷害(其他) 花費4

    // MOV
    DEF_CARD(523, "黑暗碰觸",        SKILL, 1, 0, 0), // 等級1 傷害x+使用的覺醒token(至多3) 花費0
    DEF_CARD(623, "黑暗糾纏",        SKILL, 2, 0, 2), // 等級2 傷害x+使用的覺醒token(至多3) 花費2
    DEF_CARD(723, "黑暗絞殺",        SKILL, 3, 0, 4), // 等級3 傷害x+使用的覺醒token(至多3) 花費4
    
    // [Alice]
    // ATK
    DEF_CARD(531, "開啟牌局",        SKILL, 1, 1, 0), // 等級1 傷害1+(3-對手展示的基本牌) 花費0
    DEF_CARD(631, "扭轉牌局",        SKILL, 2, 2, 2), // 等級2 傷害2+(4-對手展示的基本牌) 花費2
    DEF_CARD(731, "操控牌局",        SKILL, 3, 3, 4), // 等級3 傷害3+(5-對手展示的基本牌) 花費4
    
    // DEF
    DEF_CARD(532, "魔力技巧",        SKILL, 1, 0, 0), // 等級1 傷害0 花費0 防禦值固定為1
    DEF_CARD(632, "精神幻術",        SKILL, 2, 0, 2), // 等級2 傷害0 花費2 防禦值固定為2
    DEF_CARD(732, "帽子戲法",        SKILL, 3, 0, 4), // 等級3 傷害0 花費4 防禦值固定為3
    
    // MOV
    DEF_CARD(533, "詭異的敏捷",      SKILL, 1, 0, 0), // 等級1 傷害0 花費0 穿過對手抽1
    DEF_CARD(633, "詭異的隱蔽",      SKILL, 2, 0, 2), // 等級2 傷害0 花費2 穿過對手抽2
    DEF_CARD(733, "詭異的詭異",      SKILL, 3, 0, 4), // 等級3 傷害0 花費4 穿過對手抽3

    // [Mulan]
    // ATK
    DEF_CARD(541, "不容小覷",        SKILL, 1, 1, 0), // 等級1 傷害1+x 花費0
    DEF_CARD(641, "勢不可擋",        SKILL, 2, 2, 2), // 等級2 傷害2+x 花費2
    DEF_CARD(741, "堅不可摧",        SKILL, 3, 3, 4), // 等級3 傷害3+x 花費4

    // DEF 
    DEF_CARD(542, "以靜制動",        SKILL, 1, 0, 0), // 等級1 傷害0 花費0 防禦x
    DEF_CARD(642, "以柔克剛",        SKILL, 2, 0, 2), // 等級2 傷害0 花費2 防禦x
    DEF_CARD(742, "以弱勝強",        SKILL, 3, 0, 4), // 等級3 傷害0 花費4 防禦x

    // MOV
    DEF_CARD(543, "永不退縮",        SKILL, 1, 1, 0), // 等級1 傷害1 花費0 擊退x
    DEF_CARD(643, "毫不留情",        SKILL, 2, 2, 2), // 等級2 傷害2 花費2 擊退x
    DEF_CARD(743, "絕不饒恕",        SKILL, 3, 3, 4), // 等級3 傷害3 花費4 擊退x

    // [Kaguya]
    // ATK
    DEF_CARD(551, "領悟的光芒",      SKILL, 1, 1, 0), // 等級1 傷害1+x 花費0 每3點防禦傷害+1
    DEF_CARD(651, "領悟的榮耀",      SKILL, 2, 2, 2), // 等級2 傷害2+x 花費2 每3點防禦傷害+1
    DEF_CARD(751, "領悟的化身",      SKILL, 3, 3, 4), // 等級3 傷害3+x 花費4 每3點防禦傷害+1

    // DEF
    DEF_CARD(552, "困惑的回聲",      SKILL, 1, 0, 0), // 等級1 傷害0 花費0 防禦1+x
    DEF_CARD(652, "久遠的回響",      SKILL, 2, 0, 2), // 等級2 傷害0 花費2 防禦2+x
    DEF_CARD(752, "神性的召喚",      SKILL, 3, 0, 4), // 等級3 傷害0 花費4 防禦3+x
    
    // MOV
    DEF_CARD(553, "專注的自省",      SKILL, 1, 1, 0), // 等級1 傷害1 花費0 射程x
    DEF_CARD(653, "頓悟的決心",      SKILL, 2, 2, 2), // 等級2 傷害2 花費2 射程x
    DEF_CARD(753, "痛徹的淨化",      SKILL, 3, 3, 4), // 等級3 傷害3 花費4 射程x
       
    // [Mermaid]
    // ATK
    DEF_CARD(561, "海妖的召喚",      SKILL, 1, 1, 0), // 等級2 傷害1+x 花費0
    DEF_CARD(661, "海妖的歌聲",      SKILL, 2, 2, 2), // 等級2 傷害2+x 花費2
    DEF_CARD(761, "海妖的尖嘯",      SKILL, 3, 3, 4), // 等級3 傷害3+x 花費4

    // DEF
    DEF_CARD(562, "洶湧之怒",        SKILL, 1, 0, 0), // 等級1 傷害0 花費0
    //如果對手位於一個觸手所在的格子，對他造成x點傷害 你可以將自己放置到一個觸手所在的格子並獲得防禦1
    DEF_CARD(662, "噴薄之怒",        SKILL, 2, 0, 2), // 等級2 傷害0 花費2
    //如果對手位於一個觸手所在的格子，對他造成x點傷害 你可以將自己放置到一個觸手所在的格子並獲得防禦2
    DEF_CARD(762, "復仇之怒",        SKILL, 3, 0, 4), // 等級3 傷害0 花費4
    //如果對手位於一個觸手所在的格子，對他造成x點傷害 你可以將自己放置到一個觸手所在的格子並獲得防禦3

    // MOV
    DEF_CARD(563, "深淵的蠶食",      SKILL, 1, 0, 0), // 等級1 傷害0 花費0
    DEF_CARD(663, "深淵的入侵",      SKILL, 2, 0, 2), // 等級2 傷害0 花費2
    DEF_CARD(763, "深淵的征服",      SKILL, 3, 0, 4), // 等級3 傷害0 花費4

    // [Match Girl]
    // ATK !!!攻擊距離助教寫1 但卡牌上是1 2 3!!!
    DEF_CARD(571, "虛幻的願望",      SKILL, 1, 1, 0), // 等級1 傷害1+x 花費0 
    //且可以花費能量強化此傷害，每花費三點能量傷害+1
    DEF_CARD(671, "隱密的期望",      SKILL, 2, 2, 2), // 等級2 傷害2+x 花費2 
    //且可以花費能量強化此傷害，每花費三點能量傷害+1
    DEF_CARD(771, "無厭的奢望",      SKILL, 3, 3, 4), // 等級3 傷害3+x 花費4 
    //且可以花費能量強化此傷害，每花費三點能量傷害+1

    // DEF !!!防禦助教都寫1 但卡牌上是根據棄置的基本牌決定!!!
    DEF_CARD(572, "惡魔的祭品",      SKILL, 1, 0, 0), // 等級1 傷害0 花費0 防禦1 
    //可以失去X點生命來抽取X張牌，X至多為1
    DEF_CARD(672, "惡魔的賭注",      SKILL, 2, 0, 2), // 等級2 傷害0 花費2 防禦1 
    //可以失去X點生命來抽取X張牌，X至多為2
    DEF_CARD(772, "惡魔的契約",      SKILL, 3, 0, 4), // 等級3 傷害0 花費4 防禦1 
    //可以失去X點生命來抽取X張牌，X至多為3

    // MOV
    DEF_CARD(573, "失重的靈魂",      SKILL, 1, 1, 0), // 等級2 傷害1 花費0 射程1
    //你可以從對手棄牌堆中將至多x張火柴放回到火柴牌庫 每以此放回一張，獲得1點能量
    DEF_CARD(673, "虧欠的靈魂",      SKILL, 2, 2, 2), // 等級2 傷害2 花費2 射程2 
    //你可以從對手棄牌堆中將至多x張火柴放回到火柴牌庫 每以此放回一張，獲得2點能量，恢復1點生命
    DEF_CARD(773, "殘破的靈魂",      SKILL, 3, 3, 4), // 等級3 傷害3 花費4 射程3
    //你可以從對手棄牌堆中將至多x張火柴放回到火柴牌庫 每以此放回一張，獲得3點能量，恢復2點生命
 
    // [Dorothy]
    // ATK
    DEF_CARD(581, "目標確認",        SKILL, 1, 1, 0), // 等級1 傷害1+x 花費0 擊退x格
    //如果對手位於戰鬥軌道邊緣而無法後退 獲得傷害+y y為無法後退的格子數量
    DEF_CARD(681, "目標鎖定",        SKILL, 2, 2, 2), // 等級2 傷害2+x 花費2 擊退x格
    //如果對手位於戰鬥軌道邊緣而無法後退 獲得傷害+y y為無法後退的格子數量
    DEF_CARD(781, "目標清除",        SKILL, 3, 3, 4), // 等級3 傷害3+x 花費4 擊退x格
    //如果對手位於戰鬥軌道邊緣而無法後退 獲得傷害+y y為無法後退的格子數量
    
    // DEF
    DEF_CARD(582, "思想刺探",        SKILL, 1, 0, 0), // 等級1 傷害x 花費2 射程6
    //你可以從手牌中棄掉至多1張基本牌 抽取y+1張牌，y為你棄掉的卡牌數量
    DEF_CARD(682, "深度搜索",        SKILL, 2, 0, 2), // 等級2 傷害x 花費2 射程7
    //你可以從手牌中棄掉至多1張基本牌 抽取y+1張牌，y為你棄掉的卡牌數量
    DEF_CARD(782, "讀取完畢",        SKILL, 3, 0, 4), // 等級3 傷害x 花費4 射程8
    //你可以從手牌中棄掉至多1張基本牌 抽取y+1張牌，y為你棄掉的卡牌數量

    // MOV
    DEF_CARD(583, "發現敵蹤",        SKILL, 1, 1, 0), // 等級1 傷害y+1 花費0
    //y為你與對手之間的格子數量 射程1+x
    DEF_CARD(683, "進入視野",        SKILL, 2, 1, 2), // 等級2 傷害y+1 花費2
    //y為你與對手之間的格子數量 射程2+x
    DEF_CARD(783, "使命終結",        SKILL, 3, 1, 4), // 等級3 傷害y+1 花費4
    //y為你與對手之間的格子數量 射程3+x


    // [Scheherazade]
    // ATK
    DEF_CARD(591, "消除夢境",        SKILL, 1, 1, 0), // 等級1 傷害1+x 花費0 射程1
    DEF_CARD(691, "銷毀記憶",        SKILL, 2, 2, 2), // 等級2 傷害2+x 花費2 射程2
    DEF_CARD(791, "扼殺存在",        SKILL, 3, 3, 4), // 等級3 傷害3+x 花費4 射程3
    
    // DEF
    DEF_CARD(592, "浸沒之網",        SKILL, 1, 0, 0), // 等級1 傷害0 花費0 防禦x
    DEF_CARD(692, "沈迷之網",        SKILL, 2, 0, 2), // 等級2 傷害0 花費2 防禦x
    DEF_CARD(792, "消融之網",        SKILL, 3, 0, 4), // 等級3 傷害0 花費4 防禦x
      
    // MOV
    DEF_CARD(593, "監視之眼",        SKILL, 1, 0, 0), // 等級2 傷害1 花費0 射程1
    DEF_CARD(693, "操縱之手",        SKILL, 2, 2, 2), // 等級2 傷害2 花費2 射程2
    DEF_CARD(793, "支配之腦",        SKILL, 3, 3, 4), // 等級3 傷害3 花費4 射程3
    
    
    //個個角色必殺技
    // [Red Hood] 必殺技 (ID: 801~803)
    DEF_CARD(801, "[RH]餓狼吞噬", SUPER, 1, 5, 3),
    DEF_CARD(802, "[RH]系統入侵", SUPER, 2, 7, 5),
    DEF_CARD(803, "[RH]復仇之雨", SUPER, 3, 10, 7),

    // [Snow White] 必殺技 (ID: 811~813)
    DEF_CARD(811, "[SW]七蛇之怒", SUPER, 1, 5, 3),
    DEF_CARD(812, "[SW]魔鏡之雨", SUPER, 2, 7, 5),
    DEF_CARD(813, "[SW]醞釀之災", SUPER, 3, 10, 7),

    // [Sleeping Beauty] 必殺技 (ID: 821~823)
    DEF_CARD(821, "[SB]喚醒沉睡", SUPER, 1, 5, 3),
    DEF_CARD(822, "[SB]白日夢魘", SUPER, 2, 7, 5),
    DEF_CARD(823, "[SB]血脈重鑄", SUPER, 3, 10, 7),

    // [Alice] 必殺技 (ID: 831~833)
    DEF_CARD(831, "[AL]無休止的派對", SUPER, 1, 5, 3),
    DEF_CARD(832, "[AL]精彩的奇妙日", SUPER, 2, 7, 5),
    DEF_CARD(833, "[AL]遊戲盡在掌控", SUPER, 3, 10, 7),

    // [Mulan] 必殺技 (ID: 841~843)
    DEF_CARD(841, "[MU]氣沖雲霄", SUPER, 1, 5, 3),
    DEF_CARD(842, "[MU]直面混沌", SUPER, 2, 7, 5),
    DEF_CARD(843, "[MU]雷霆一擊", SUPER, 3, 10, 7),

    // [Kaguya] 必殺技 (ID: 851~853)
    DEF_CARD(851, "[KA]炙熱的竹刀", SUPER, 1, 5, 3),
    DEF_CARD(852, "[KA]注定的審判", SUPER, 2, 7, 5),
    DEF_CARD(853, "[KA]躁動的血性", SUPER, 3, 10, 7),

    // [Mermaid] 必殺技 (ID: 861~863)
    DEF_CARD(861, "[MM]人魚復興", SUPER, 1, 5, 3),
    DEF_CARD(862, "[MM]遠古甦醒", SUPER, 2, 7, 5),
    DEF_CARD(863, "[MM]淨化之潮", SUPER, 3, 10, 7),

    // [Match Girl] 必殺技 (ID: 871~873)
    DEF_CARD(871, "[MG]地獄烈焰", SUPER, 1, 5, 3),
    DEF_CARD(872, "[MG]厄運降臨", SUPER, 2, 7, 5),
    DEF_CARD(873, "[MG]貪婪詛咒", SUPER, 3, 10, 7),

    // [Dorothy] 必殺技 (ID: 881~883)
    DEF_CARD(881, "[DO]獅子", SUPER, 1, 5, 3),
    DEF_CARD(882, "[DO]鐵皮人", SUPER, 2, 7, 5),
    DEF_CARD(883, "[DO]稻草人", SUPER, 3, 10, 7),

    // [Scheherazade] 必殺技 (ID: 891~893)
    DEF_CARD(891, "[SH]系統刪除", SUPER, 1, 5, 3),
    DEF_CARD(892, "[SH]無法自拔", SUPER, 2, 7, 5),
    DEF_CARD(893, "[SH]切斷通路", SUPER, 3, 10, 7),
    
    // Status Cards
    DEF_CARD(911, "[Status] Poison LV1", STATUS, 1, 0, 0),
    DEF_CARD(912, "[Status] Poison LV2", STATUS, 2, 0, 0),
    DEF_CARD(913, "[Status] Poison LV3", STATUS, 3, 0, 0),
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
