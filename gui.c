#include "gui.h"
#include "definitions.h"
#include "raylib.h"
#include "database.h"
#include <stdbool.h>
#define ULTRA_CARD_WIDTH  140*1.2
#define ULTRA_CARD_HEIGHT 200*1.2

// 外部變數與函式
extern const char* character_names[];
extern Font font;
extern Texture2D backgroundTexture;

//能否打開必殺欄位
bool can_use_ultra1 = false;

// 函式原型
void DrawShop(const Game* game);
void DrawFocusSelection(const Game* game);
void DrawBattleInterface(const Game* game);
void DrawGameBoard(const Game* game);
void DrawCharSelection(Texture2D character_images[10]);
void DrawPlayerInfo(const Game* game, bool is_human);
void DrawCard(const Card* card, Rectangle bounds, bool is_hovered, bool is_opponent_card);
void DrawSkillPairingOverlay(const Game* game);
void apply_buy_card(Game* game, int card_id);
void DrawPassiveInfoOverlay(const Game* game);
void DrawPassiveButton(Rectangle bounds, const char* text, bool isHovered, bool isSelected);
void DrawUltraOverlay(const Game* game); // ✅ 加這行

// =================================================================
//                               繪製函式
// =================================================================

void DrawCard(const Card* card, Rectangle bounds, bool is_hovered, bool is_opponent_card) {
    if (is_opponent_card) {
        DrawRectangleRounded(bounds, 0.08f, 10, DARKBLUE);
        DrawRectangleRoundedLinesEx(bounds, 0.08f, 10, 4, BLUE);
        return;
    }
    if (is_hovered) {
        bounds.y -= 20;
    }
    DrawRectangleRounded(bounds, 0.08f, 10, RAYWHITE);
    DrawRectangleRoundedLinesEx(bounds, 0.08f, 10, is_hovered ? 5 : 3, is_hovered ? GOLD : BLACK);
    if (card) {
        DrawTextEx(font, card->name, (Vector2){ bounds.x + 5 , bounds.y + 15 }, 18, 1, BLACK);
        
        // --- 卡牌類型標籤 ---
        if (card->type == ATTACK) {
            DrawTextEx(font, TextFormat("Attack: %d", card->value), (Vector2){ bounds.x + 5, bounds.y + 50 }, 16, 1, RED);
        } else if (card->type == DEFENSE) {
            DrawTextEx(font, TextFormat("Defense: %d", card->value), (Vector2){ bounds.x + 5, bounds.y + 50 }, 16, 1, DARKGREEN);
        } else if (card->type == MOVE) {
            DrawTextEx(font, TextFormat("Move: %d", card->value), (Vector2){ bounds.x + 5, bounds.y + 50 }, 16, 1, PURPLE);
        } else if (card->type == SKILL) {
            int subtype = card->id % 10;

            // 判斷技能等級（根據 id 區間）
            int level = 0;
            if (card->id >= 500 && card->id < 600) level = 1;
            else if (card->id >= 600 && card->id < 700) level = 2;
            else if (card->id >= 700 && card->id < 800) level = 3;

            const char* skill_type_text = "Skill";
            switch (subtype) {
                case 1: skill_type_text = TextFormat("[ATK] Skill%d", level); break;
                case 2: skill_type_text = TextFormat("[DEF] Skill%d", level); break;
                case 3: skill_type_text = TextFormat("[MOV] Skill%d", level); break;
            }

            DrawTextEx(font, skill_type_text, (Vector2){ bounds.x + 5, bounds.y + 50 }, 16, 1, BLUE);
        }

        
        // --- 能量獲取標籤 ---
        if (card->type == ATTACK || card->type == DEFENSE || card->type == MOVE || card->type == GENERIC) {
             DrawTextEx(font, TextFormat("Energy Gain: +%d", card->value), (Vector2){ bounds.x + 5, bounds.y + CARD_HEIGHT - 35 }, 14, 1, SKYBLUE);
        }

        // --- 顯示卡片 cost ---
        //DrawTextEx(font, TextFormat("Cost: %d", card->cost), (Vector2){ bounds.x + 15, bounds.y + CARD_HEIGHT - 18 }, 14, 1, ORANGE);
    }

    

    bool isRedHoodSkill        = (card->id >= 501 && card->id <= 503) ||
                                 (card->id >= 601 && card->id <= 603) ||
                                 (card->id >= 701 && card->id <= 703);
    
    bool isSnowWhiteSkill      = (card->id >= 511 && card->id <= 513) ||
                                 (card->id >= 611 && card->id <= 613) ||
                                 (card->id >= 711 && card->id <= 713);
    
    bool isSleepingBeautySkill = (card->id >= 521 && card->id <= 523) ||
                                 (card->id >= 621 && card->id <= 623) ||
                                 (card->id >= 721 && card->id <= 723);
    
    // 愛麗絲技能卡 ID：531~533, 631~633, 731~733
    bool isAliceSkill          = (card->id >= 531 && card->id <= 533) ||
                                 (card->id >= 631 && card->id <= 633) ||
                                 (card->id >= 731 && card->id <= 733);
    
    // 花木蘭技能卡 ID：541~543, 641~643, 741~743
    bool isMulanSkill = (card->id >= 541 && card->id <= 543) ||
                        (card->id >= 641 && card->id <= 643) ||
                        (card->id >= 741 && card->id <= 743);

    // 輝夜姬技能卡 ID：551~553, 651~653, 751~753
    bool isKaguyaSkill = (card->id >= 551 && card->id <= 553) ||
                        (card->id >= 651 && card->id <= 653) ||
                        (card->id >= 751 && card->id <= 753);

    // 美人魚技能卡 ID：561~563, 661~663, 761~763
    bool isMermaidSkill = (card->id >= 561 && card->id <= 563) ||
                        (card->id >= 661 && card->id <= 663) ||
                        (card->id >= 761 && card->id <= 763);

    // 火柴女孩技能卡 ID：571~573, 671~673, 771~773
    bool isMatchGirlSkill = (card->id >= 571 && card->id <= 573) ||
                            (card->id >= 671 && card->id <= 673) ||
                            (card->id >= 771 && card->id <= 773);

    // 桃樂絲技能卡 ID：581~583, 681~683, 781~783
    bool isDorothySkill = (card->id >= 581 && card->id <= 583) ||
                        (card->id >= 681 && card->id <= 683) ||
                        (card->id >= 781 && card->id <= 783);

    // 山魯佐德技能卡 ID：591~593, 691~693, 791~793
    bool isScheherazadeSkill = (card->id >= 591 && card->id <= 593) ||
                            (card->id >= 691 && card->id <= 693) ||
                            (card->id >= 791 && card->id <= 793);

                        
    // 顯示小紅帽卡牌說明
    if (is_hovered && isRedHoodSkill) {
        int level = card->id / 100 - 4;   // 501 → 1, 601 → 2, 701 → 3
        int type  = card->id % 10;        // 1:攻擊 2:防禦 3:移動

        const char* line1 = "";
        const char* line2 = "";
        const char* line3 = "";

        if (type == 1) {
            if (level == 1) line1 = "射程1, 傷害1+x";
            if (level == 2) line1 = "射程2, 傷害2+x";
            if (level == 3) line1 = "射程3, 傷害3+x";
        } else if (type == 2) {
            if (level == 1) {
                line1 = "射程1, 傷害1, 防禦x";
                line2 = "回合開始時";                
                line3 = "若防禦>0時對範圍1造成2傷害";
            }
            if (level == 2) {
                line1 = "射程2, 傷害2, 防禦x";
                line2 = "回合開始時";
                line3 = "若防禦>0時對範圍2造成4傷害";
            }
            if (level == 3) {
                line1 = "射程3, 傷害3, 防禦x";
                line2 = "回合開始時";
                line3 = "若防禦>0時對範圍3造成6傷害";
            }
        } else if (type == 3) {
            if (level == 1) line1 = "射程1, 傷害1, 擊退x";
            if (level == 2) line1 = "射程2, 傷害2, 擊退x";
            if (level == 3) line1 = "射程3, 傷害3, 擊退x";
        }

        // ⬛ 對話框設計
        float descWidth = 270;
        float descHeight =120; 

        Rectangle descBox = {
            bounds.x,
            bounds.y - descHeight - 8,
            descWidth,
            descHeight
        };

        DrawRectangleRounded(descBox, 0.2f, 6, Fade(BLACK, 0.7f));
        DrawTextEx(font, line1, (Vector2){ descBox.x + 10, descBox.y + 6 }, 16, 1, WHITE);
        DrawTextEx(font, line2, (Vector2){ descBox.x + 10, descBox.y + 30 }, 16, 1, WHITE);
        DrawTextEx(font, line3, (Vector2){ descBox.x + 10, descBox.y + 54 }, 16, 1, WHITE);
    }

        // ✅ 白雪公主技能說明
    else if (is_hovered && isSnowWhiteSkill) {
        int level = card->id / 100 - 4;  // 511→1, 611→2, 711→3
        int type  = card->id % 10;       // 1:攻擊, 2:防禦, 3:移動

        const char* line1 = "";
        const char* line2 = "";
        const char* line3 = "";

        if (type == 1) {
            line1 = TextFormat("射程1, 傷害%d+x", level);
            line2 = TextFormat("將對手牌庫最上面%d張", level);
            line3 = "放入棄牌區";
        } else if (type == 2) {
            line1 = TextFormat("射程1, 傷害%d", level);
            line2 = "將中毒牌庫頂部至多x張";
            line3 = "放入對手棄牌區";
        } else if (type == 3) {
            line1 = TextFormat("射程x+%d, 傷害%d", level - 1, level);
            line2 = "將自己放置到與敵人";
            line3 = "相鄰的格子";
        }

        float descWidth = 270;
        float descHeight =120; 

        Rectangle descBox = {
            bounds.x,
            bounds.y - descHeight - 8,
            descWidth,
            descHeight
        };

        DrawRectangleRounded(descBox, 0.2f, 6, Fade(BLACK, 0.7f));
        DrawTextEx(font, line1, (Vector2){ descBox.x + 10, descBox.y + 6 }, 16, 1, WHITE);
        DrawTextEx(font, line2, (Vector2){ descBox.x + 10, descBox.y + 30 }, 16, 1, WHITE);
        DrawTextEx(font, line3, (Vector2){ descBox.x + 10, descBox.y + 54 }, 16, 1, WHITE);
    }
    
    
    //睡美人技能說明
    else if (is_hovered && isSleepingBeautySkill) {
        int level = card->id / 100 - 4;  // 521→1, 621→2, 721→3
        int type  = card->id % 10;       // 1:攻擊, 2:防禦, 3:移動

        const char* line1 = "";
        const char* line2 = "";
        const char* line3 = "";

        if (type == 1) {
            line1 = TextFormat("射程1, 傷害%dx", level);
            line2 = TextFormat("可對自己造成y傷害獲得", "");
            line3 = TextFormat("傷害+y，y最多為%d", level);
        } else if (type == 2) {
            line1 = "直到回合結束前";
            line2 = TextFormat("x+Token 次攻擊將獲得傷害+%d", level);
            line3 = "（傷害不須連續使用）";
        } else if (type == 3) {
            line1 = TextFormat("射程%d, 傷害x+Token", level + 1);
            line2 = "將對手向我方向移動x格";
            line3 = "";
        }

        float descWidth = 270;
        float descHeight = 120;

        Rectangle descBox = {
            bounds.x,
            bounds.y - descHeight - 8,
            descWidth,
            descHeight
        };

        DrawRectangleRounded(descBox, 0.2f, 6, Fade(BLACK, 0.7f));
        DrawTextEx(font, line1, (Vector2){ descBox.x + 10, descBox.y + 6 }, 16, 1, WHITE);
        DrawTextEx(font, line2, (Vector2){ descBox.x + 10, descBox.y + 30 }, 16, 1, WHITE);
        DrawTextEx(font, line3, (Vector2){ descBox.x + 10, descBox.y + 54 }, 16, 1, WHITE);
    }

    // 愛麗絲技能說明
    else if (is_hovered && isAliceSkill) {
        int level = card->id / 100 - 4;  // 531→1, 631→2, 731→3
        int type  = card->id % 10;       // 1:攻擊, 2:防禦, 3:移動

        const char* line1 = "";
        const char* line2 = "";
        const char* line3 = "";
        const char* line4 = "";

        if (type == 1) {
            // 攻擊技能（紅心皇后身分）
            line1 = TextFormat("射程%d, 傷害%d", level, level);
            line2 = TextFormat("展示 %d 張牌，沒展示造成額外傷害", level + 2);
            line3 = "可從展示牌找供應區同名牌入棄牌堆";
            line4 = "身分轉變為紅心皇后";
        } else if (type == 2) {
            // 防禦技能（瘋帽子身分）
            line1 = TextFormat("防禦 %d，移除基本牌", level);
            line2 = "每移除一張，從基本牌庫放一張";
            line3 = TextFormat("等級<= %d 基本牌入棄牌堆", level);
            line4 = "身分轉變為瘋帽子";
        } else if (type == 3) {
            // 移動技能（柴郡貓身分）
            line1 = TextFormat("向一方向移動至多 %d+x 格", level);
            line2 = "穿過對手時可抽牌";
            line3 = TextFormat("抽 %d 張牌", level);
            line4 = "身分轉變為柴郡貓";
        }

        float descWidth = 270;
        float descHeight = 120; // 多一行 → 增高

        Rectangle descBox = {
            bounds.x,
            bounds.y - descHeight - 8,
            descWidth,
            descHeight
        };

        DrawRectangleRounded(descBox, 0.2f, 6, Fade(BLACK, 0.7f));
        DrawTextEx(font, line1, (Vector2){ descBox.x + 10, descBox.y + 6 }, 16, 1, WHITE);
        DrawTextEx(font, line2, (Vector2){ descBox.x + 10, descBox.y + 30 }, 16, 1, WHITE);
        DrawTextEx(font, line3, (Vector2){ descBox.x + 10, descBox.y + 54 }, 16, 1, WHITE);
        DrawTextEx(font, line4, (Vector2){ descBox.x + 10, descBox.y + 78 }, 16, 1, WHITE);
    }

    // 花木蘭技能說明
    else if (is_hovered && isMulanSkill) {
        int level = card->id / 100 - 4;  // 541→1, 641→2, 741→3
        int type  = card->id % 10;       // 1:攻擊, 2:防禦, 3:移動

        const char* line1 = "";
        const char* line2 = "";
        const char* line3 = "";
        const char* line4 = "";

        if (type == 1) {
            // 攻擊技能
            line1 = TextFormat("射程1, 傷害%d+x", level);
            line2 = "可將對手移動到你相鄰的一格";
            line3 = "若對手在邊緣，隨機棄一張手牌";
            line4 = "";
        } else if (type == 2) {
            // 防禦技能
            line1 = "防禦x，回合結束時抽牌階段";
            line2 = TextFormat("可花y氣額外抽y張牌，y最多為%d", level);
            line3 = "";
            line4 = "";
        } else if (type == 3) {
            // 移動技能
            line1 = TextFormat("射程1, 傷害%d，擊退對手x格", level);
            line2 = "自己移動到與對手相鄰的格子";
            line3 = "若對手在邊緣，隨機棄一張手牌";
            line4 = "";
        }

        float descWidth = 270;
        float descHeight = 120;

        Rectangle descBox = {
            bounds.x,
            bounds.y - descHeight - 8,
            descWidth,
            descHeight
        };

        DrawRectangleRounded(descBox, 0.2f, 6, Fade(BLACK, 0.7f));
        DrawTextEx(font, line1, (Vector2){ descBox.x + 10, descBox.y + 6 }, 16, 1, WHITE);
        DrawTextEx(font, line2, (Vector2){ descBox.x + 10, descBox.y + 30 }, 16, 1, WHITE);
        DrawTextEx(font, line3, (Vector2){ descBox.x + 10, descBox.y + 54 }, 16, 1, WHITE);
        DrawTextEx(font, line4, (Vector2){ descBox.x + 10, descBox.y + 78 }, 16, 1, WHITE);
    }

    // 輝夜姬技能說明
    else if (is_hovered && isKaguyaSkill) {
        int level = card->id / 100 - 4;  // 551→1, 651→2, 751→3
        int type  = card->id % 10;       // 1:攻擊, 2:防禦, 3:移動

        const char* line1 = "";
        const char* line2 = "";
        const char* line3 = "";
        const char* line4 = "";

        if (type == 1) {
            // 攻擊技能
            line1 = TextFormat("射程1, 傷害%d+x", level);
            line2 = "若當前擁有3點防禦";
            line3 = "則額外+1傷害";
            line4 = "";
        } else if (type == 2) {
            // 防禦技能
            line1 = TextFormat("防禦%d+x，展示牌庫頂 %d 張牌", level, level);
            line2 = "若是防禦技能牌則加入手牌";
            line3 = "否則可選擇棄掉或放回頂部";
            line4 = "";
        } else if (type == 3) {
            // 移動技能
            line1 = TextFormat("射程x, 傷害%d，失去1生命", level);
            line2 = "可移除手牌或棄牌堆中一張牌";
            if (level >= 2) {
                line3 = "持續：若敵人超出射程4-x";
                line4 = TextFormat("則對他造成 %d 點傷害", level * 2);
            } else {
                line3 = "";
                line4 = "";
            }
        }

        float descWidth = 270;
        float descHeight = 120; 

        Rectangle descBox = {
            bounds.x,
            bounds.y - descHeight - 8,
            descWidth,
            descHeight
        };

        DrawRectangleRounded(descBox, 0.2f, 6, Fade(BLACK, 0.7f));
        DrawTextEx(font, line1, (Vector2){ descBox.x + 10, descBox.y + 6 }, 16, 1, WHITE);
        DrawTextEx(font, line2, (Vector2){ descBox.x + 10, descBox.y + 30 }, 16, 1, WHITE);
        DrawTextEx(font, line3, (Vector2){ descBox.x + 10, descBox.y + 54 }, 16, 1, WHITE);
        DrawTextEx(font, line4, (Vector2){ descBox.x + 10, descBox.y + 78 }, 16, 1, WHITE);
    }


    // 美人魚技能說明
    else if (is_hovered && isMermaidSkill) {
        int level = card->id / 100 - 4;  // 561→1, 661→2, 761→3
        int type  = card->id % 10;       // 1:攻擊, 2:防禦, 3:移動

        const char* line1 = "";
        const char* line2 = "";
        const char* line3 = "";
        const char* line4 = "";

        if (type == 1) {
            // 攻擊技能
            line1 = TextFormat("射程%d, 傷害%d+x", level, level);
            line2 = TextFormat("若敵在觸手格, 傷害+%d", level);
            if (level == 1) line3 = "若你在觸手格, 能量+1";
            else line3 = "若你在觸手格, 能量+3";
            line4 = "";
        } else if (type == 2) {
            // 防禦技能
            line1 = "若敵在觸手格, 對其造成x傷害";
            line2 = TextFormat("你可移動到觸手格並獲得防禦%d", level);
            line3 = "（不能與對手在同一格）";
            line4 = "";
        } else if (type == 3) {
            // 移動技能
            line1 = "選一觸手並移動至多x格(≥1)";
            line2 = TextFormat("若你在觸手格, 抽 %d 張牌", level);
            if (level == 1) {
                line3 = "";
                line4 = "";
            } else {
                line3 = TextFormat("若敵在觸手格, 捨棄 %d 張手牌", level - 1);
                line4 = "";
            }
        }

        float descWidth = 270;
        float descHeight = 120;

        Rectangle descBox = {
            bounds.x,
            bounds.y - descHeight - 8,
            descWidth,
            descHeight
        };

        DrawRectangleRounded(descBox, 0.2f, 6, Fade(BLACK, 0.7f));
        DrawTextEx(font, line1, (Vector2){ descBox.x + 10, descBox.y + 6 }, 16, 1, WHITE);
        DrawTextEx(font, line2, (Vector2){ descBox.x + 10, descBox.y + 30 }, 16, 1, WHITE);
        DrawTextEx(font, line3, (Vector2){ descBox.x + 10, descBox.y + 54 }, 16, 1, WHITE);
        DrawTextEx(font, line4, (Vector2){ descBox.x + 10, descBox.y + 78 }, 16, 1, WHITE);
    }

    // 火柴女孩技能說明
    else if (is_hovered && isMatchGirlSkill) {
        int level = card->id / 100 - 4;  // 571→1, 671→2, 771→3
        int type  = card->id % 10;       // 1:攻擊, 2:防禦, 3:移動

        const char* line1 = "";
        const char* line2 = "";
        const char* line3 = "";
        const char* line4 = "";

        if (type == 1) {
            // 攻擊技能
            line1 = TextFormat("射程1, 傷害%d+x", level);
            line2 = "可花能量強化傷害";
            line3 = "每花3點能量 → 傷害+1";
            line4 = "";
        } else if (type == 2) {
            // 防禦技能
            line1 = "防禦1，可失去生命來抽牌";
            line2 = TextFormat("最多失去 %d 點生命", level);
            line3 = TextFormat("抽取 %d 張牌", level);
            line4 = "";
        } else if (type == 3) {
            // 移動技能
            line1 = TextFormat("射程%d, 傷害%d", level, level);
            line2 = "可將對手棄牌堆中火柴放回火柴庫";
            if (level == 1) {
                line3 = "每放1張 → 能量+1";
                line4 = "";
            } else if (level == 2) {
                line3 = "每放1張 → 能量+2, 生命+1";
                line4 = "";
            } else {
                line3 = "每放1張 → 能量+3, 生命+2";
                line4 = "";
            }
        }

        float descWidth = 270;
        float descHeight = 120;

        Rectangle descBox = {
            bounds.x,
            bounds.y - descHeight - 8,
            descWidth,
            descHeight
        };

        DrawRectangleRounded(descBox, 0.2f, 6, Fade(BLACK, 0.7f));
        DrawTextEx(font, line1, (Vector2){ descBox.x + 10, descBox.y + 6 }, 16, 1, WHITE);
        DrawTextEx(font, line2, (Vector2){ descBox.x + 10, descBox.y + 30 }, 16, 1, WHITE);
        DrawTextEx(font, line3, (Vector2){ descBox.x + 10, descBox.y + 54 }, 16, 1, WHITE);
        DrawTextEx(font, line4, (Vector2){ descBox.x + 10, descBox.y + 78 }, 16, 1, WHITE);
    }

    // 桃樂絲技能說明
    else if (is_hovered && isDorothySkill) {
        int level = card->id / 100 - 4;  // 581→1, 681→2, 781→3
        int type  = card->id % 10;       // 1:攻擊, 2:防禦, 3:移動

        const char* line1 = "";
        const char* line2 = "";
        const char* line3 = "";
        const char* line4 = "";

        if (type == 1) {
            // 攻擊技能
            line1 = TextFormat("射程1, 傷害%d+x，擊退x格", level);
            line2 = "若對手無法後退";
            line3 = "則每格無法後退 → 傷害+1";
            line4 = "";
        } else if (type == 2) {
            // 防禦技能
            line1 = TextFormat("射程%d, 傷害x", level + 5);  // 6~8
            line2 = TextFormat("可棄至多 %d 張牌", level);
            line3 = TextFormat("抽取 y+1 張牌 (y為棄牌數)", level);
            line4 = "";
        } else if (type == 3) {
            // 移動技能
            line1 = TextFormat("射程%d+x", level);
            line2 = "傷害 = y+1";
            line3 = "y為你與對手之間的格子數";
            line4 = "";
        }

        float descWidth = 270;
        float descHeight = 120;

        Rectangle descBox = {
            bounds.x,
            bounds.y - descHeight - 8,
            descWidth,
            descHeight
        };

        DrawRectangleRounded(descBox, 0.2f, 6, Fade(BLACK, 0.7f));
        DrawTextEx(font, line1, (Vector2){ descBox.x + 10, descBox.y + 6 }, 16, 1, WHITE);
        DrawTextEx(font, line2, (Vector2){ descBox.x + 10, descBox.y + 30 }, 16, 1, WHITE);
        DrawTextEx(font, line3, (Vector2){ descBox.x + 10, descBox.y + 54 }, 16, 1, WHITE);
        DrawTextEx(font, line4, (Vector2){ descBox.x + 10, descBox.y + 78 }, 16, 1, WHITE);
    }

    // 山魯佐德技能說明
    else if (is_hovered && isScheherazadeSkill) {
        int level = card->id / 100 - 4;  // 591→1, 691→2, 791→3
        int type  = card->id % 10;       // 1:攻擊, 2:防禦, 3:移動

        const char* line1 = "";
        const char* line2 = "";
        const char* line3 = "";
        const char* line4 = "";
        const char* line5 = "";

        if (type == 1) {
            // 攻擊技能
            line1 = TextFormat("射程%d, 傷害%d+x", level, level);
            line2 = TextFormat("可翻轉最多 %d 枚", level);
            line3 = "藍色命運TOKEN為紅色";
            line4 = "";
            line5 = "";
        } else if (type == 2) {
            // 防禦技能
            line1 = "防禦x，放置命運TOKEN";
            line2 = TextFormat("可放至對手供應區最多 %d 張牌", level);
            line3 = "持續：對手購買該牌時";
            line4 = "需額外支付1點能量";
            line5 = "";
        } else if (type == 3) {
            // 移動技能
            line1 = TextFormat("射程%d, 傷害%d", level, level);
            line2 = "展示對手牌庫頂x張牌";
            line3 = "每來自帶藍TOKEN的牌庫";
            line4 = "就翻轉該牌庫1枚TOKEN為紅色";
            line5 = "每張展示牌可棄掉或放回原位";
        }

        float descWidth = 270;
        float descHeight = 120;  // 五行顯示高度

        Rectangle descBox = {
            bounds.x,
            bounds.y - descHeight - 8,
            descWidth,
            descHeight
        };

        DrawRectangleRounded(descBox, 0.2f, 6, Fade(BLACK, 0.7f));
        DrawTextEx(font, line1, (Vector2){ descBox.x + 10, descBox.y + 6  }, 16, 1, WHITE);
        DrawTextEx(font, line2, (Vector2){ descBox.x + 10, descBox.y + 30 }, 16, 1, WHITE);
        DrawTextEx(font, line3, (Vector2){ descBox.x + 10, descBox.y + 54 }, 16, 1, WHITE);
        DrawTextEx(font, line4, (Vector2){ descBox.x + 10, descBox.y + 78 }, 16, 1, WHITE);
        DrawTextEx(font, line5, (Vector2){ descBox.x + 10, descBox.y + 102 }, 16, 1, WHITE);
    }


}

// DrawPlayerInfo 函式 - 保持不變
void DrawPlayerInfo(const Game* game, bool is_human) {
    int player_idx = is_human ? 0 : 1;
    const player* p = &game->inner_game.players[player_idx];
    if(p->character >= 10) return;

    int x_pos = is_human ? 30 : GetScreenWidth() - 330;
    int y_pos = is_human ? GetScreenHeight() - 110 : 30;
    
    const char* role_name = character_names[p->character];
    
    Rectangle info_box = {(float)x_pos, (float)y_pos, 300, 80};
    DrawRectangleRec(info_box, Fade(BLACK, 0.7f));
    DrawRectangleLinesEx(info_box, 2, is_human ? SKYBLUE : RED);
    
    DrawTextEx(font, role_name, (Vector2){ (float)x_pos + 15, (float)y_pos + 10}, 24, 1, WHITE);
    DrawTextEx(font, TextFormat("HP: %d", p->life), (Vector2){ (float)x_pos + 15, (float)y_pos + 45 }, 20, 1, LIME);
    DrawTextEx(font, TextFormat("Defense: %d", p->defense), (Vector2){ (float)x_pos + 110, (float)y_pos + 45 }, 20, 1, GRAY);
    DrawTextEx(font, TextFormat("Energy: %d", p->energy), (Vector2){ (float)x_pos + 210, (float)y_pos + 45 }, 20, 1, SKYBLUE);

    // 被動技能按鈕（僅在玩家一側顯示）
    if (is_human) {
        // 🎯 被動技按鈕 P
        Rectangle passive_btn = { x_pos + 160, y_pos - 50, 40, 40 };
        bool hover = CheckCollisionPointRec(GetMousePosition(), passive_btn);
        bool click = hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

        // 改成使用你自定義的函式
        DrawPassiveButton(passive_btn, "P", hover, game->current_state == GAME_STATE_PASSIVE_INFO);

        if (click) {
            ((Game*)game)->current_state = GAME_STATE_PASSIVE_INFO;
        }
    
        // 🎯 必殺技按鈕 U
        Rectangle ultra_btn = { x_pos + 210, y_pos - 50, 40, 40 };
        bool ultra_hover = CheckCollisionPointRec(GetMousePosition(), ultra_btn);
        bool ultra_click = ultra_hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

        // 取得目前玩家
        int player_id = game->inner_game.now_turn_player_id;
        const player* p = &game->inner_game.players[player_id];

        // 即時檢查生命是否低於觸發閥值
        bool can_use_ultra = (p->life <= p->specialGate);
        if(can_use_ultra) can_use_ultra1 = true;

        //can_use_ultra1 = true;
        // 使用粉紅主題的 DrawPassiveButton 畫 U 鍵（亮起或灰色）
        DrawPassiveButton(ultra_btn, "U", ultra_hover, can_use_ultra1 && game->current_state == GAME_STATE_ULTRA);

        // 畫鎖頭提示
        if (!can_use_ultra1) {
            // 計算中心點位置
            int fontSize = 20;
            Vector2 lockSize = MeasureTextEx(font, " X ", fontSize+40, 1);
            DrawTextEx(font, " X ",
                (Vector2){
                    (ultra_btn.x + (ultra_btn.width  - lockSize.x) / 2) +6,
                    (ultra_btn.y + (ultra_btn.height - lockSize.y) / 2) +5
                },
                fontSize+30, 1, BLACK);
        }

        // 👉 畫黃色圓圈 + 黑色文字（必殺閾值）
        int radius = 14;
        int centerX = ultra_btn.x + ultra_btn.width + 20;
        int centerY = ultra_btn.y + ultra_btn.height / 2;

        // 畫圓形
        DrawCircle(centerX, centerY, radius, YELLOW);

        // 畫數字（黑色，置中）
        char threshold_text[8];
        sprintf(threshold_text, "%d", p->specialGate);

        int fontSize = 16;
        int textWidth = MeasureText(threshold_text, fontSize);
        DrawText(threshold_text, centerX - textWidth / 2, centerY - fontSize / 2, fontSize, BLACK);

        // 點擊後切換狀態（只有能用時才有效）
        if (ultra_click && can_use_ultra1) {
            ((Game*)game)->current_state = GAME_STATE_ULTRA;     // ✅ 保留轉型
        }
            
    
    }




}

//介面參數
const float CARD_BTN_X = 200.0f;
const float CARD_BTN_Y = 280.0f;
const float CARD_BTN_W = 160;
const float CARD_BTN_H = 80;
const float ROW_GAP = 200.0f;
const float COL_GAP = 180.0f;

void DrawCharSelection(Texture2D character_images[10]) {
    DrawTexture(backgroundTexture, 0, 0, WHITE);
    DrawTextEx(font, "Select Your Hero", (Vector2){ (float)GetScreenWidth()/2 - MeasureTextEx(font, "Select Your Hero", 60, 2).x/2, 100 }, 60, 2, WHITE);
    for (int i = 0; i < 10; i++) {
        int row = i / 5;
        float extra_y = (row == 1) ? 40.0f : 0.0f;  // 第二排整體下移 40px

        Rectangle btn_bounds = {
            CARD_BTN_X + (i % 5) * COL_GAP,
            CARD_BTN_Y + row * ROW_GAP + extra_y,
            CARD_BTN_W,
            CARD_BTN_H
        };

        bool hover = CheckCollisionPointRec(GetMousePosition(), btn_bounds);
        DrawRectangleRec(btn_bounds, hover ? SKYBLUE : LIGHTGRAY);
        DrawRectangleLinesEx(btn_bounds, 2, BLACK);
        DrawTextEx(font, character_names[i], (Vector2){ btn_bounds.x + 20, btn_bounds.y + 30 }, 20, 1, BLACK);

        // ✅👉 就在這裡底下加上這段：圖片等比例縮放畫法
        // === 加入圖片顯示區 ===
        float target_w = CARD_BTN_W;
        float target_h = 80;
        float img_w = character_images[i].width;
        float img_h = character_images[i].height;

        float scale = fminf(target_w / img_w, target_h / img_h);

        // 👇 特別縮小花木蘭
        if (i == 4) { // e.g.花木蘭
            scale *= 0.94f;
        }


        float final_w = img_w * scale;
        float final_h = img_h * scale;


        float draw_x = btn_bounds.x + (target_w - final_w) / 2;
        float draw_y = btn_bounds.y + 90 + (target_h - final_h) / 2;

        if (i == 4) { // 花木蘭
            draw_y -= 9.0f; // 往上移動 10 像素
        }

        DrawTexturePro(character_images[i],
            (Rectangle){0, 0, img_w, img_h},
            (Rectangle){draw_x, draw_y, final_w, final_h},
            (Vector2){0, 0},
            0,
            WHITE
        );
    }
    Rectangle exit_btn = { GetScreenWidth() - 180.0f, GetScreenHeight() - 70.0f, 160, 50 };
    bool btn_hover = CheckCollisionPointRec(GetMousePosition(), exit_btn);
    DrawRectangleRec(exit_btn, btn_hover ? RED : MAROON);
    DrawTextEx(font, "Exit Game", (Vector2){ exit_btn.x + 40, exit_btn.y + 15 }, 20, 1, WHITE);
}

// DrawGameBoard 函式 - 保持不變
void DrawGameBoard(const Game* game) {
    float screenWidth = GetScreenWidth();
    float screenHeight = GetScreenHeight();

    const player* human = &game->inner_game.players[0];
    const player* bot = &game->inner_game.players[1];

    const int gameplay_slots = 11;
    const float slot_w = 80.0f;
    const float slot_h = 110.0f;
    const float spacing = 15.0f;
    float gameplay_area_w = gameplay_slots * (slot_w + spacing) - spacing;
    float start_x = (screenWidth - gameplay_area_w) / 2.0f;
    
    float screen_center_y = screenHeight / 2.0f;
    float board_offset_y = 40.0f;
    float mid_y = screen_center_y - (slot_h / 2.0f) + board_offset_y;

    // (Notice) 繪製遊戲板上的圓形位置和卡牌插槽
    const float circle_radius = 35.0f;
    const float vertical_gap = 95.0f;
    float top_row_y = screen_center_y - vertical_gap + board_offset_y;
    float bottom_row_y = screen_center_y + vertical_gap + board_offset_y;

    for (int i = 0; i < gameplay_slots; i++) {
        float current_x = start_x + i * (slot_w + spacing);
        float circle_center_x = current_x + (slot_w / 2.0f);
        Color color = (i == 2 || i == 8) ? Fade(LIME, 0.7f) : Fade(DARKGRAY, 0.7f);
        float thickness = (i == 5) ? 5.0f : 3.0f;

        Vector2 top_center = { circle_center_x, top_row_y };
        DrawCircleV(top_center, circle_radius, color);
        DrawCircleLinesV(top_center, circle_radius, BLACK);
        if(i == 5) DrawRing(top_center, circle_radius - thickness, circle_radius, 0, 360, 36, BLACK);

        Vector2 bottom_center = { circle_center_x, bottom_row_y };
        DrawCircleV(bottom_center, circle_radius, color);
        DrawCircleLinesV(bottom_center, circle_radius, BLACK);
        if(i == 5) DrawRing(bottom_center, circle_radius - thickness, circle_radius, 0, 360, 36, BLACK);
        
        for(int p_idx = 0; p_idx < 2; p_idx++){
            if(game->inner_game.players[p_idx].locate[0] == i) {
                Color token_color = (p_idx == 0) ? BLUE : RED;
                DrawCircleV(bottom_center, circle_radius * 0.6f, token_color);
                DrawCircleLines(bottom_center.x, bottom_center.y, circle_radius * 0.6f, BLACK);
            }
        }
    }
    
    for (int i = 0; i < gameplay_slots; i++) {
        float current_x = start_x + i * (slot_w + spacing);
        Rectangle card_slot = {current_x, mid_y, slot_w, slot_h};
        DrawRectangleRounded(card_slot, 0.1f, 10, Fade(BLACK, 0.6f));
        DrawRectangleRoundedLinesEx(card_slot, 0.1f, 10, 2, LIME);
    }
    
    // (Notice) 牌堆繪製邏輯
    Rectangle deck_rect = { start_x - slot_w - spacing * 3, mid_y, slot_w, slot_h };
    DrawRectangleRounded(deck_rect, 0.1f, 10, Fade(DARKBLUE, 0.8f));
    DrawRectangleRoundedLinesEx(deck_rect, 0.1f, 10, 2, SKYBLUE);
    DrawTextEx(font, "Deck", (Vector2){deck_rect.x + 20, deck_rect.y - 25}, 20, 1, WHITE);
    
    // 繪製分隔線
    DrawLine(deck_rect.x, deck_rect.y + deck_rect.height/2, deck_rect.x + deck_rect.width, deck_rect.y + deck_rect.height/2, LIGHTGRAY);
    
    // 繪製對手牌數 (上方)
    const char* bot_deck_text = TextFormat("%d", bot->deck.SIZE);
    Vector2 bot_deck_text_size = MeasureTextEx(font, bot_deck_text, 24, 1);
    DrawTextEx(font, bot_deck_text, (Vector2){deck_rect.x + (deck_rect.width - bot_deck_text_size.x)/2, deck_rect.y + deck_rect.height*0.25f - bot_deck_text_size.y/2}, 24, 1, ORANGE);

    // 繪製我方牌數 (下方)
    const char* human_deck_text = TextFormat("%d", human->deck.SIZE);
    Vector2 human_deck_text_size = MeasureTextEx(font, human_deck_text, 24, 1);
    DrawTextEx(font, human_deck_text, (Vector2){deck_rect.x + (deck_rect.width - human_deck_text_size.x)/2, deck_rect.y + deck_rect.height*0.75f - human_deck_text_size.y/2}, 24, 1, WHITE);


    Rectangle discard_rect = { start_x + gameplay_area_w + spacing * 3, mid_y, slot_w, slot_h };
    DrawRectangleRounded(discard_rect, 0.1f, 10, Fade(MAROON, 0.8f));
    DrawRectangleRoundedLinesEx(discard_rect, 0.1f, 10, 2, RED);
    DrawTextEx(font, "Discard", (Vector2){discard_rect.x + 5, discard_rect.y - 25}, 20, 1, WHITE);
    
    // 繪製分隔線
    DrawLine(discard_rect.x, discard_rect.y + discard_rect.height/2, discard_rect.x + discard_rect.width, discard_rect.y + discard_rect.height/2, LIGHTGRAY);

    // 繪製對手棄牌數 (上方)
    const char* bot_discard_text = TextFormat("%d", bot->graveyard.SIZE);
    Vector2 bot_discard_text_size = MeasureTextEx(font, bot_discard_text, 24, 1);
    DrawTextEx(font, bot_discard_text, (Vector2){discard_rect.x + (discard_rect.width - bot_discard_text_size.x)/2, discard_rect.y + discard_rect.height*0.25f - bot_discard_text_size.y/2}, 24, 1, ORANGE);

    // 繪製我方棄牌數 (下方)
    const char* human_discard_text = TextFormat("%d", human->graveyard.SIZE);
    Vector2 human_discard_text_size = MeasureTextEx(font, human_discard_text, 24, 1);
    DrawTextEx(font, human_discard_text, (Vector2){discard_rect.x + (discard_rect.width - human_discard_text_size.x)/2, discard_rect.y + discard_rect.height*0.75f - human_discard_text_size.y/2}, 24, 1, WHITE);

    if (human->character == SNOW_WHITE) {
        float poison_pile_h = slot_h / 2;
        Rectangle poison_deck_rect = { deck_rect.x, deck_rect.y + deck_rect.height + 20, slot_w, poison_pile_h };
        DrawRectangleRounded(poison_deck_rect, 0.1f, 10, Fade(DARKGREEN, 0.8f));
        DrawRectangleRoundedLinesEx(poison_deck_rect, 0.1f, 10, 2, LIME);
        DrawTextEx(font, "Poison", (Vector2){poison_deck_rect.x + 15, poison_deck_rect.y - 25}, 20, 1, WHITE);

        if (human->snowWhite.remindPosion.SIZE > 0) {
            int top_poison_id = human->snowWhite.remindPosion.array[human->snowWhite.remindPosion.SIZE - 1];
            const Card* top_poison_card = get_card_info(top_poison_id);
            if (top_poison_card) {
                const char* poison_level_text = TextFormat("LV: %d", top_poison_card->level);
                Vector2 text_size = MeasureTextEx(font, poison_level_text, 24, 1);
                DrawTextEx(font, poison_level_text, (Vector2){poison_deck_rect.x + (poison_deck_rect.width - text_size.x)/2, poison_deck_rect.y + (poison_deck_rect.height - text_size.y)/2}, 24, 1, WHITE);
            }
        } else {
            const char* empty_text = "Empty";
            Vector2 text_size = MeasureTextEx(font, empty_text, 24, 1);
            DrawTextEx(font, empty_text, (Vector2){poison_deck_rect.x + (poison_deck_rect.width - text_size.x)/2, poison_deck_rect.y + (poison_deck_rect.height - text_size.y)/2}, 24, 1, GRAY);
        }
    }
}

#define HAND_SCALE 0.8f  // 玩家手牌縮放比例
#define CARD_SCALE 0.8f
void DrawBattleInterface(const Game* game) {
    const player* human = &game->inner_game.players[0];
    const player* bot = &game->inner_game.players[1];
    int distance = abs(human->locate[0] - bot->locate[0]);

    int scaled_card_width = CARD_WIDTH * HAND_SCALE;
    int scaled_card_height = CARD_HEIGHT * HAND_SCALE;
    int spacing = 15 * HAND_SCALE;

    int hand_width = human->hand.SIZE * (scaled_card_width + spacing) - spacing;
    float hand_start_x = (GetScreenWidth() - hand_width) / 2.0f;
    float hand_y = GetScreenHeight() - scaled_card_height - 20;

    // 繪製玩家手牌
    for (uint32_t i = 0; i < human->hand.SIZE; ++i) {
        Rectangle card_bounds = {
            hand_start_x + i * (scaled_card_width + spacing),
            hand_y,
            scaled_card_width,
            scaled_card_height
        };

        bool is_hovered = (game->current_state != GAME_STATE_PASSIVE_INFO) &&
                        CheckCollisionPointRec(GetMousePosition(), card_bounds);
        const Card* card_info = get_card_info(human->hand.array[i]);

        DrawCard(card_info, card_bounds, is_hovered, false);

        // [修改] 檢查可玩性並在需要時繪製遮罩
        bool is_playable = true;
        if (card_info) {
            if (card_info->type == ATTACK) {
                if (distance > 1) { // 基礎攻擊牌的射程檢查
                    is_playable = false;
                }
            } else if (card_info->type == SKILL) {
                int skill_subtype = card_info->id % 10;
                int range = 0;
                bool range_check_needed = false;

                if (human->character == RED_HOOD) {
                    if (skill_subtype == 1 || skill_subtype == 2 || skill_subtype == 3) {
                        range = card_info->level;
                        range_check_needed = true;
                    }
                } else if (human->character == SNOW_WHITE) {
                    // (Notice) 白雪公主的攻擊或防禦技能射程皆為 1
                    if (skill_subtype == 1 || skill_subtype == 2) {
                        range = 1;
                        range_check_needed = true;
                    }
                }else if (human->character == SLEEPING_BEAUTY) {
                    if (skill_subtype == 1) { // 攻擊技能
                        range = 1;
                        range_check_needed = true;
                    }
                }

                if (range_check_needed && distance > range) {
                    is_playable = false;
                }
            }
        }

        if (!is_playable) {
            DrawRectangleRec(card_bounds, Fade(BLACK, 0.6f));
            DrawRectangleLinesEx(card_bounds, 3, RED); // 加上紅色邊框以示警告
        }
    }
    
    // 繪製 Bot 手牌
    int bot_hand_width = bot->hand.SIZE * (CARD_WIDTH/1.5f + 10) - 10;
    float bot_hand_start_x = (GetScreenWidth() - bot_hand_width) / 2.0f;
    for (uint32_t i = 0; i < bot->hand.SIZE; ++i) {
        Rectangle bot_card = {bot_hand_start_x + i * (CARD_WIDTH/1.5f + 10), 80, CARD_WIDTH/1.5f, CARD_HEIGHT/1.5f};
        DrawRectangleRounded(bot_card, 0.08f, 10, DARKBLUE);
        DrawRectangleRoundedLinesEx(bot_card, 0.08f, 10, 4, BLUE);
    }
    
    // 繪製操作按鈕
    // === End Turn 按鈕 ===
    Rectangle end_turn_btn = { GetScreenWidth() - 200.0f, GetScreenHeight() - 60.0f, 180, 50 };
    bool et_hover = false;
    if (game->current_state == GAME_STATE_HUMAN_TURN) {
        et_hover = CheckCollisionPointRec(GetMousePosition(), end_turn_btn);
    }
    DrawRectangleRec(end_turn_btn, et_hover ? LIME : GREEN);
    DrawTextEx(font, "End Turn", (Vector2){ end_turn_btn.x + 50, end_turn_btn.y + 15 }, 20, 1, BLACK);

    // === Focus 按鈕 ===
    Rectangle focus_btn = { GetScreenWidth() - 200.0f, GetScreenHeight() - 120.0f, 180, 50 };
    bool focus_hover = false;
    if (game->current_state == GAME_STATE_HUMAN_TURN && !game->player_has_acted) {
        focus_hover = CheckCollisionPointRec(GetMousePosition(), focus_btn);
    }
    DrawRectangleRec(focus_btn, game->player_has_acted ? GRAY : (focus_hover ? YELLOW : GOLD));
    DrawTextEx(font, "Focus", (Vector2){ focus_btn.x + 60, focus_btn.y + 15 }, 20, 1, BLACK);

    // === Shop 按鈕 ===
    Rectangle shop_btn = { GetScreenWidth() - 200.0f, GetScreenHeight() - 180.0f, 180, 50 };
    bool shop_hover = false;
    if (game->current_state == GAME_STATE_HUMAN_TURN) {
        shop_hover = CheckCollisionPointRec(GetMousePosition(), shop_btn);
    }
    DrawRectangleRec(shop_btn, shop_hover ? SKYBLUE : BLUE);
    DrawTextEx(font, "Shop", (Vector2){ shop_btn.x + 65, shop_btn.y + 15 }, 20, 1, WHITE);


    // 繪製回合提示
    const char* turn_text = "";
    if (game->inner_game.now_turn_player_id == 0) {
        turn_text = "Your Turn";
    } else {
        turn_text = "Opponent's Turn";
    }
    Vector2 message_size = MeasureTextEx(font, turn_text, 40, 2);
    DrawTextEx(font, turn_text, (Vector2){ (GetScreenWidth() - message_size.x)/2, GetScreenHeight() / 2.0f }, 40, 2, WHITE);
}

// DrawFocusSelection 函式 - 保持不變
void DrawFocusSelection(const Game* game) {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.85f));
    DrawTextEx(font, "Focus: Remove a Card", (Vector2){ (float)GetScreenWidth()/2 - MeasureTextEx(font, "Focus: Remove a Card", 40, 2).x/2, 50 }, 40, 2, WHITE);
    DrawTextEx(font, "Select a card from your Hand or Graveyard to permanently remove it.", (Vector2){ (float)GetScreenWidth()/2 - MeasureTextEx(font, "Select a card from your Hand or Graveyard to permanently remove it.", 20, 1).x/2, 100 }, 20, 1, LIGHTGRAY);
    
    const player* p = &game->inner_game.players[0];

    DrawTextEx(font, "Your Hand:", (Vector2){50, 150}, 24, 1, WHITE);
    for (uint32_t i = 0; i < p->hand.SIZE; i++) {
        Rectangle card_bounds = { 50 + i * (CARD_WIDTH + 15), 180, CARD_WIDTH, CARD_HEIGHT };
        bool is_hovered = CheckCollisionPointRec(GetMousePosition(), card_bounds);
        const Card* card_info = get_card_info(p->hand.array[i]);
        DrawCard(card_info, card_bounds, is_hovered, false);
    }
    
    DrawTextEx(font, "Your Graveyard:", (Vector2){50, 400}, 24, 1, WHITE);
    for (uint32_t i = 0; i < p->graveyard.SIZE; i++) {
        Rectangle card_bounds = { 50 + i * (CARD_WIDTH + 15), 430, CARD_WIDTH, CARD_HEIGHT };
        bool is_hovered = CheckCollisionPointRec(GetMousePosition(), card_bounds);
        const Card* card_info = get_card_info(p->graveyard.array[i]);
        DrawCard(card_info, card_bounds, is_hovered, false);
    }

    Rectangle cancel_btn = { GetScreenWidth() - 180.0f, GetScreenHeight() - 70.0f, 160, 50 };
    bool btn_hover = CheckCollisionPointRec(GetMousePosition(), cancel_btn);
    DrawRectangleRec(cancel_btn, btn_hover ? ORANGE : DARKGRAY);
    DrawTextEx(font, "Cancel", (Vector2){ cancel_btn.x + 45, cancel_btn.y + 15 }, 20, 1, WHITE);
}

void DrawGame(Game* game, Texture2D character_images[10]) {
    DrawTexture(backgroundTexture, 0, 0, WHITE);
    
    if (game->current_state == GAME_STATE_CHOOSE_CHAR) {
        DrawCharSelection(character_images);
        return;
    }

    // --- 繪製主要遊戲介面 ---
    DrawGameBoard(game);
    DrawPlayerInfo(game, true);
    DrawPlayerInfo(game, false);
    if (game->current_state != GAME_STATE_SHOP && game->current_state != GAME_STATE_FOCUS_REMOVE) {
        DrawBattleInterface(game);
    }

    // [新] 繪製對局中的退出按鈕
    if (game->current_state != GAME_STATE_CHOOSE_CHAR && game->current_state != GAME_STATE_GAME_OVER) {
        Rectangle exit_btn = { 20, 20, 100, 40 };
        bool hover = CheckCollisionPointRec(GetMousePosition(), exit_btn);
        DrawRectangleRec(exit_btn, hover ? RED : MAROON);
        DrawRectangleLinesEx(exit_btn, 2, BLACK);
        DrawTextEx(font, "Exit", (Vector2){ exit_btn.x + 30, exit_btn.y + 10 }, 20, 1, WHITE);
    }

    // --- 繪製疊加層 (Overlays) ---
    switch (game->current_state) {
        case GAME_STATE_SHOP:
            DrawShop(game);
            break;
        case GAME_STATE_FOCUS_REMOVE:
            DrawFocusSelection(game);
            break;
        case GAME_STATE_AWAITING_BASIC_FOR_SKILL:
            DrawSkillPairingOverlay(game);
            break;
        case GAME_STATE_CHOOSE_MOVE_DIRECTION: {
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.5f));
            DrawTextEx(font, "Choose Direction", (Vector2){480, 300}, 40, 1, WHITE);
            Rectangle leftBtn = {480, 350, 120, 50};
            Rectangle rightBtn = {680, 350, 120, 50};
            DrawRectangleRec(leftBtn, CheckCollisionPointRec(GetMousePosition(), leftBtn) ? SKYBLUE : BLUE);
            DrawText("Left", leftBtn.x + 35, leftBtn.y + 15, 20, WHITE);
            DrawRectangleRec(rightBtn, CheckCollisionPointRec(GetMousePosition(), rightBtn) ? SKYBLUE : BLUE);
            DrawText("Right", rightBtn.x + 30, rightBtn.y + 15, 20, WHITE);
            break;
        }
        case GAME_STATE_GAME_OVER: {
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.6f));
            Vector2 message_size = MeasureTextEx(font, game->message, 60, 2);
            DrawTextEx(font, game->message, (Vector2){ (GetScreenWidth() - message_size.x)/2, (GetScreenHeight()/2) - 80 }, 60, 2, WHITE);
            Rectangle back_btn = { (float)GetScreenWidth()/2 - 125, (float)GetScreenHeight()/2 + 40, 250, 50 };
            bool back_hover = CheckCollisionPointRec(GetMousePosition(), back_btn);
            DrawRectangleRec(back_btn, back_hover ? SKYBLUE : BLUE);
            DrawTextEx(font, "Return to Menu", (Vector2){ back_btn.x + 50, back_btn.y + 15 }, 20, 1, WHITE);
            break;
        }

        case GAME_STATE_ULTRA: {
            DrawUltraOverlay(game);
        }
        
        
        default:
            break;
    }

    if (game->current_state == GAME_STATE_PASSIVE_INFO) {
        DrawPassiveInfoOverlay(game);
    }
}

// [修改] DrawShop 函式，將技能牌分頁改為堆疊顯示相同卡牌
void DrawShop(const Game* game) {
    float screenWidth = GetScreenWidth();
    float screenHeight = GetScreenHeight();
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.85f));
    DrawTextEx(font, "Shop", (Vector2){screenWidth / 2 - MeasureTextEx(font, "Shop", 60, 2).x / 2, 40}, 60, 2, GOLD);
    DrawTextEx(font, TextFormat("Your Energy: %d", game->inner_game.players[0].energy), (Vector2){40, 40}, 30, 1, WHITE);
    
    // --- 繪製頁籤按鈕 ---
    Rectangle basic_tab = { screenWidth / 2.0f - 210, 150, 200, 40 };
    Rectangle skill_tab = { screenWidth / 2.0f + 10, 150, 200, 40 };

    bool basic_hover = CheckCollisionPointRec(GetMousePosition(), basic_tab);
    DrawRectangleRec(basic_tab, game->shop_page == 0 ? GOLD : (basic_hover ? LIGHTGRAY : DARKGRAY));
    const char* basic_text = "Basic";
    Vector2 basic_text_size = MeasureTextEx(font, basic_text, 20, 1);
    DrawTextEx(font, basic_text, (Vector2){ basic_tab.x + (basic_tab.width - basic_text_size.x) / 2, basic_tab.y + 10 }, 20, 1, BLACK);
    DrawRectangleLinesEx(basic_tab, 2, BLACK);

    bool skill_hover = CheckCollisionPointRec(GetMousePosition(), skill_tab);
    DrawRectangleRec(skill_tab, game->shop_page == 1 ? GOLD : (skill_hover ? LIGHTGRAY : DARKGRAY));
    const char* skill_text = "Skill";
    Vector2 skill_text_size = MeasureTextEx(font, skill_text, 20, 1);
    DrawTextEx(font, skill_text, (Vector2){ skill_tab.x + (skill_tab.width - skill_text_size.x) / 2, skill_tab.y + 10 }, 20, 1, BLACK);
    DrawRectangleLinesEx(skill_tab, 2, BLACK);

    // --- 根據當前頁面繪製對應內容 ---
    if (game->shop_page == 0) {
        // --- 基礎牌頁面 (採用 3x3 動態置中網格) ---
        float column_width = 300; 
        float total_content_width = 3 * column_width;
        float startX = (screenWidth - total_content_width) / 2.0f;
        
        float top_bound = 210;
        float bottom_bound = screenHeight - 90;
        float total_area_height = bottom_bound - top_bound;
        float row_spacing = total_area_height / 3.0f;

        for (int type = 0; type < 3; type++) { 
            float row_center_y = top_bound + (row_spacing * type) + (row_spacing / 2.0f);
            float card_start_y = row_center_y - (CARD_HEIGHT / 2.0f);

            for (int level = 0; level < 3; level++) {
                const vector* pile = &game->shop_piles[type][level];
                if (pile->SIZE > 0) {
                    const Card* card = get_card_info(pile->array[0]);
                    if(card) {
                        float card_start_x = startX + (level * column_width);
                        Rectangle bounds = {card_start_x, card_start_y, CARD_WIDTH, CARD_HEIGHT};
                        bool hovered = CheckCollisionPointRec(GetMousePosition(), bounds);
                        DrawCard(card, bounds, hovered, false);
                        
                        float text_x = bounds.x + CARD_WIDTH + 20;
                        DrawTextEx(font, TextFormat("Cost: %d", card->cost), (Vector2){text_x, bounds.y + 40}, 20, 1, WHITE);
                        DrawTextEx(font, TextFormat("Left: %d", pile->SIZE), (Vector2){text_x, bounds.y + 70}, 20, 1, WHITE);
                    }
                }
            }
        }
    } 

    if (game->shop_page == 1) {
        float offsetX = 200;  // 往右移動 200px
        float offsetY = 100;  // 往下移動 100px
        int chara = game->inner_game.players[0].character;

        if (chara >= 0 && chara < 10) {
            DrawTextEx(font, "攻擊技能", (Vector2){ 100 + offsetX, 110 + offsetY }, 22, 1, RED);
            DrawTextEx(font, "防禦技能", (Vector2){ 400 + offsetX, 110 + offsetY }, 22, 1, DARKGREEN);
            DrawTextEx(font, "移動技能", (Vector2){ 700 + offsetX, 110 + offsetY }, 22, 1, PURPLE);

            for (int type = 0; type < 3; ++type) {
                const vector* pile = &game->shop_skill_piles[chara][type];
                int lv3_count = 0, lv2_count = 0;
                int lv3_index = -1, lv2_index = -1;
                bool lv2_empty = true;

                // 找出 LV3 卡最上層
                for (int i = pile->SIZE - 1; i >= 0; --i) {
                    int id = pile->array[i];
                    if (id >= 700 && id < 800 && lv3_index == -1) lv3_index = i;
                    if (id >= 600 && id < 700) lv2_empty = false;
                }

                // 顯示 LV3 卡
                for (uint32_t i = 0; i < pile->SIZE; ++i) {
                    int card_id = pile->array[i];
                    if (card_id >= 700 && card_id < 800) {
                        Rectangle rect = {
                            80 + offsetX + type * 300,
                            150 + offsetY + lv3_count * 40,
                            CARD_WIDTH, CARD_HEIGHT
                        };

                        const Card* card = get_card_by_id(card_id);
                        bool is_top = ((int)i == lv3_index);
                        bool can_hover = is_top && lv2_empty;
                        bool hovered = can_hover && CheckCollisionPointRec(GetMousePosition(), rect);

                        if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                            apply_buy_card((Game*)game, card_id);
                        }

                        DrawCard(card, rect, hovered, false);
                        lv3_count++;
                    }
                }

                // 找出 LV2 最上層
                for (int i = pile->SIZE - 1; i >= 0; --i) {
                    int id = pile->array[i];
                    if (id >= 600 && id < 700) {
                        lv2_index = i;
                        break;
                    }
                }

                // 顯示 LV2 卡
                for (uint32_t i = 0; i < pile->SIZE; ++i) {
                    int card_id = pile->array[i];
                    if (card_id >= 600 && card_id < 700) {
                        Rectangle rect = {
                            80 + offsetX + type * 300,
                            150 + offsetY + (lv3_count + lv2_count) * 40,
                            CARD_WIDTH, CARD_HEIGHT
                        };

                        const Card* card = get_card_by_id(card_id);
                        bool is_top = ((int)i == lv2_index);
                        bool hovered = is_top && CheckCollisionPointRec(GetMousePosition(), rect);

                        if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                            apply_buy_card((Game*)game, card_id);
                        }

                        DrawCard(card, rect, hovered, false);
                        lv2_count++;
                    }
                }

                // ➕ 顯示 cost
                int cost = (lv2_count > 0) ? 2 : 4;
                char cost_text[32];
                sprintf(cost_text, "Cost: %d", cost);
                Vector2 cost_pos = {
                    100 + offsetX + type * 300 + 10,
                    300 + offsetY + (lv3_count + lv2_count) * 40 + 5
                };
                DrawTextEx(font, cost_text, cost_pos, 20, 1, ORANGE);
            }
        } else {
            DrawTextEx(font, "技能商店尚未開放", (Vector2){ 330 + offsetX, 300 + offsetY }, 28, 1, RED);
        }
    }

    
    // --- 繪製關閉按鈕 ---
    Rectangle close_btn = { screenWidth - 160, screenHeight - 70, 140, 50 };
    bool hover = CheckCollisionPointRec(GetMousePosition(), close_btn);
    DrawRectangleRec(close_btn, hover ? RED : MAROON);
    DrawTextEx(font, "Close", (Vector2){close_btn.x + 45, close_btn.y + 15}, 20, 1, WHITE);
}

void DrawSkillPairingOverlay(const Game* game) {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.8f));
    Vector2 msg_size = MeasureTextEx(font, game->message, 30, 1);
    DrawTextEx(font, game->message, (Vector2){(GetScreenWidth() - msg_size.x)/2, GetScreenHeight()/2 - 40}, 30, 1, WHITE);
    
    Rectangle cancel_btn = { GetScreenWidth() / 2.0f - 100, GetScreenHeight() / 2.0f + 50, 200, 50 };
    bool hover = CheckCollisionPointRec(GetMousePosition(), cancel_btn);
    DrawRectangleRec(cancel_btn, hover ? RED : MAROON);
    DrawTextEx(font, "Cancel", (Vector2){ cancel_btn.x + 65, cancel_btn.y + 15 }, 20, 1, WHITE);

    const player* human = &game->inner_game.players[0];
    
    // [修改] 根據已選擇的技能牌，判斷需要高亮的基礎牌類型
    const Card* pending_skill_card = get_card_info(human->hand.array[game->pending_skill_card_index]);
    if (!pending_skill_card) return; // 安全檢查

    CardType required_type;
    switch (pending_skill_card->id % 10) {
        case 1: required_type = ATTACK; break;
        case 2: required_type = DEFENSE; break;
        case 3: required_type = MOVE; break;
        default: required_type = GENERIC; break; // 無效類型
    }

    int hand_width = human->hand.SIZE * (CARD_WIDTH + 15) - 15;
    float hand_start_x = (GetScreenWidth() - hand_width) / 2.0f;
    float hand_y = GetScreenHeight() - CARD_HEIGHT - 20;

    for (uint32_t i = 0; i < human->hand.SIZE; ++i) {
        Rectangle card_bounds = { hand_start_x + i * (CARD_WIDTH + 15), hand_y, CARD_WIDTH, CARD_HEIGHT };
        const Card* card_info = get_card_info(human->hand.array[i]);
        if (!card_info) continue;

        if ((int)i == game->pending_skill_card_index) {
            DrawRectangleRoundedLinesEx(card_bounds, 0.08f, 10, 5, YELLOW);
        } else {
            // [修改] 只高亮顯示類型相符的基礎牌
            if (card_info->type == required_type) {
                DrawCard(card_info, card_bounds, false, false);
                DrawRectangleRoundedLinesEx(card_bounds, 0.08f, 10, 3, LIME);
            }
        }
    }
}

void DrawPassiveInfoOverlay(const Game* game) {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.8f));
    DrawTextEx(font, "角色被動技能", (Vector2){ 100, 80 }, 40, 2, WHITE);

    const player* p = &game->inner_game.players[0];
    int chara = p->character;

    const vector* atk_pile = &game->shop_skill_piles[chara][0];
    const vector* def_pile = &game->shop_skill_piles[chara][1];
    const vector* mov_pile = &game->shop_skill_piles[chara][2];

    int atk_lv2 = 0, atk_lv3 = 0;
    int def_lv2 = 0, def_lv3 = 0;
    int mov_lv2 = 0, mov_lv3 = 0;

    for (uint32_t i = 0; i < atk_pile->SIZE; ++i) {
        int id = atk_pile->array[i];
        if (id >= 600 && id < 700) atk_lv2++;
        if (id >= 700 && id < 800) atk_lv3++;
    }
    for (uint32_t i = 0; i < def_pile->SIZE; ++i) {
        int id = def_pile->array[i];
        if (id >= 600 && id < 700) def_lv2++;
        if (id >= 700 && id < 800) def_lv3++;
    }
    for (uint32_t i = 0; i < mov_pile->SIZE; ++i) {
        int id = mov_pile->array[i];
        if (id >= 600 && id < 700) mov_lv2++;
        if (id >= 700 && id < 800) mov_lv3++;
    }

    int atk_bought_lv2 = 2 - atk_lv2;    
    int def_bought_lv2 = 2 - def_lv2;
    int mov_bought_lv2 = 2 - mov_lv2;
    

    // 是否解鎖
    bool unlock_atk_lv2 = atk_bought_lv2 >= 2;
    bool unlock_atk_cache = atk_lv3 == 0;
    bool unlock_def_lv2 = def_bought_lv2 >= 2;
    bool unlock_def_cache = def_lv3 == 0;
    bool unlock_mov_lv2 = mov_bought_lv2 >= 2;
    bool unlock_mov_cache = mov_lv3 == 0;

    int x = 150, y = 180;  // 左上角開始座標
    int line_gap = 28;     // 每行高度
    
    switch (chara) {
        case 0: // 小紅帽
            DrawTextEx(font, "- 暴打別人一頓", (Vector2){ 120, 140 }, 24, 1, SKYBLUE);
            // 🟦 攻擊被動
            DrawTextEx(font, "攻擊被動", (Vector2){x, y}, 20, 1, RED); y += line_gap;

            DrawTextEx(font,
                unlock_atk_lv2 ? "過載燃燒: 使用移動或攻擊技能時，可捨棄最多1張技能牌，攻擊+x(x為捨棄的技能牌等級)"
                            : "過載燃燒(未解鎖): 使用移動或攻擊技能時，可捨棄最多1張技能牌，攻擊+x(x為捨棄的技能牌等級)",
                (Vector2){x, y}, 20, 1, unlock_atk_lv2 ? YELLOW : GRAY); y += line_gap;
            if (!unlock_atk_lv2) {
                DrawTextEx(font, "解鎖條件: 購買兩張技能2的攻擊技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            DrawTextEx(font,
                unlock_atk_cache ? TextFormat("板載緩存A: 把最多一張牌留到下個回合")
                                : "板載緩存A(未解鎖): 把最多一張牌留到下個回合",
                (Vector2){x, y}, 20, 1, unlock_atk_cache ? YELLOW : GRAY); y += line_gap;
            if (!unlock_atk_cache) {
                DrawTextEx(font, "解鎖條件: 購買三張等級3的攻擊技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            // 🟩 防禦被動
            y += 12;
            DrawTextEx(font, "防禦被動", (Vector2){x, y}, 20, 1, GREEN); y += line_gap;

            DrawTextEx(font,
                unlock_def_lv2 ? "兜帽系統: 當對手對我造成傷害時，可捨棄最多1張技能牌，傷害抵免x(x為捨棄的技能牌等級)"
                            : "兜帽系統(未解鎖): 當對手對我造成傷害時，可捨棄最多1張技能牌，傷害抵免x(x為捨棄的技能牌等級)",
                (Vector2){x, y}, 20, 1, unlock_def_lv2 ? YELLOW : GRAY); y += line_gap;
            if (!unlock_def_lv2) {
                DrawTextEx(font, "解鎖條件: 購買兩張技能2的防禦技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            DrawTextEx(font,
                unlock_def_cache ? "板載緩存B: 把最多一張牌留到下個回合"
                                : "板載緩存B(未解鎖): 把最多一張牌留到下個回合",
                (Vector2){x, y}, 20, 1, unlock_def_cache ? YELLOW : GRAY); y += line_gap;
            if (!unlock_def_cache) {
                DrawTextEx(font, "解鎖條件: 購買三張等級3的防禦技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            // 🟪 移動被動
            y += 12;
            DrawTextEx(font, "移動被動", (Vector2){x, y}, 20, 1, PURPLE); y += line_gap;

            DrawTextEx(font,
                unlock_mov_lv2 ? "變異感應: 使用移動或攻擊技能時，可捨棄最多1張技能牌，射程+x(x為捨棄的技能牌等級)"
                            : "變異感應(未解鎖): 使用移動或攻擊技能時，可捨棄最多1張技能牌，射程+x(x為捨棄的技能牌等級)",
                (Vector2){x, y}, 20, 1, unlock_mov_lv2 ? YELLOW : GRAY); y += line_gap;
            if (!unlock_mov_lv2) {
                DrawTextEx(font, "解鎖條件: 購買兩張技能2的移動技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            DrawTextEx(font,
                unlock_mov_cache ? "板載緩存C: 把最多一張牌留到下個回合"
                                : "板載緩存C(未解鎖): 把最多一張牌留到下個回合",
                (Vector2){x, y}, 20, 1, unlock_mov_cache ? YELLOW : GRAY); y += line_gap;
            if (!unlock_mov_cache) {
                DrawTextEx(font, "解鎖條件: 購買三張等級3的移動技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }
            break;


        case 1: // 白雪公主
            DrawTextEx(font, "- 中毒標記會持續傷害敵人", (Vector2){ 120, 140 }, 24, 1, SKYBLUE);
            // 🟥 攻擊被動
            DrawTextEx(font, "攻擊被動", (Vector2){x, y}, 20, 1, RED); y += line_gap;
            DrawTextEx(font,
                unlock_atk_lv2 ? "水晶之棺: 對對手造成2點以上傷害時，將1張中毒牌放入其棄牌堆"
                            : "水晶之棺(未解鎖): 對對手造成2點以上傷害時，將1張中毒牌放入其棄牌堆",
                (Vector2){x, y}, 20, 1, unlock_atk_lv2 ? YELLOW : GRAY); y += line_gap;
            if (!unlock_atk_lv2) {
                DrawTextEx(font, "解鎖條件: 購買兩張技能2的攻擊技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            DrawTextEx(font,
                unlock_atk_cache ? "至純之毒A: 每張中毒牌進入對手棄牌堆時，對手額外失去1點生命"
                                : "至純之毒A(未解鎖): 每張中毒牌進入對手棄牌堆時，對手額外失去1點生命",
                (Vector2){x, y}, 20, 1, unlock_atk_cache ? YELLOW : GRAY); y += line_gap;
            if (!unlock_atk_cache) {
                DrawTextEx(font, "解鎖條件: 購買三張等級3的攻擊技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            // 🟩 防禦被動
            y += 12;
            DrawTextEx(font, "防禦被動", (Vector2){x, y}, 20, 1, GREEN); y += line_gap;

            DrawTextEx(font,
                unlock_def_lv2 ? "墮落之劫: 使用防禦技能放中毒牌時，可選擇改為洗入對手牌庫"
                            : "墮落之劫(未解鎖): 使用防禦技能放中毒牌時，可選擇改為洗入對手牌庫",
                (Vector2){x, y}, 20, 1, unlock_def_lv2 ? YELLOW : GRAY); y += line_gap;
            if (!unlock_def_lv2) {
                DrawTextEx(font, "解鎖條件: 購買兩張技能2的防禦技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            DrawTextEx(font,
                unlock_def_cache ? "至純之毒B: 每張中毒牌進入對手棄牌堆時，對手額外失去1點生命"
                                : "至純之毒B(未解鎖): 每張中毒牌進入對手棄牌堆時，對手額外失去1點生命",
                (Vector2){x, y}, 20, 1, unlock_def_cache ? YELLOW : GRAY); y += line_gap;
            if (!unlock_def_cache) {
                DrawTextEx(font, "解鎖條件: 購買三張等級3的防禦技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            // 🟪 移動被動
            y += 12;
            DrawTextEx(font, "移動被動", (Vector2){x, y}, 20, 1, PURPLE); y += line_gap;

            DrawTextEx(font,
                unlock_mov_lv2 ? "劇毒之蝕: 移動行動穿過對手時，將1張中毒牌放入其棄牌堆"
                            : "劇毒之蝕(未解鎖): 移動行動穿過對手時，將1張中毒牌放入其棄牌堆",
                (Vector2){x, y}, 20, 1, unlock_mov_lv2 ? YELLOW : GRAY); y += line_gap;
            if (!unlock_mov_lv2) {
                DrawTextEx(font, "解鎖條件: 購買兩張技能2的移動技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            DrawTextEx(font,
                unlock_mov_cache ? "至純之毒C: 每張中毒牌進入對手棄牌堆時，對手額外失去1點生命"
                                : "至純之毒C(未解鎖): 每張中毒牌進入對手棄牌堆時，對手額外失去1點生命",
                (Vector2){x, y}, 20, 1, unlock_mov_cache ? YELLOW : GRAY); y += line_gap;
            if (!unlock_mov_cache) {
                DrawTextEx(font, "解鎖條件: 購買三張等級3的移動技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }
            break;

        case 2: // 睡美人
            DrawTextEx(font, "攻擊被動", (Vector2){x, y}, 20, 1, RED); y += line_gap;
            DrawTextEx(font, "- 流血才能變強", (Vector2){ 120, 140 }, 24, 1, SKYBLUE);

            DrawTextEx(font,
                unlock_atk_lv2 ? "放血療法: 每回合限1次，技能或行動需捨基本牌時，可\"失去\"2/4/6生命當作Lv1/2/3"
                                : "放血療法(未解鎖): 每回合限1次，技能/行動需捨基本牌時，可\"失去\"2/4/6生命當作Lv1/2/3",
                (Vector2){x, y}, 20, 1, unlock_atk_lv2 ? YELLOW : GRAY); y += line_gap;
            if (!unlock_atk_lv2) {
                DrawTextEx(font, "解鎖條件: 購買兩張技能2的攻擊技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            DrawTextEx(font,
                unlock_atk_cache ? "強制治療A: 立即回復5點生命，使用後將此效果移除"
                                : "強制治療A(未解鎖): 立即回復5點生命，使用後將此效果移除",
                (Vector2){x, y}, 20, 1, unlock_atk_cache ? YELLOW : GRAY); y += line_gap;
            if (!unlock_atk_cache) {
                DrawTextEx(font, "解鎖條件: 購買三張等級3的攻擊技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            y += 12;
            DrawTextEx(font, "防禦被動", (Vector2){x, y}, 20, 1, GREEN); y += line_gap;

            DrawTextEx(font,
                unlock_def_lv2 ? "血祭之禮: 每回合限一次，當你造成至少2/4/6點傷害時，可從棄牌堆中挑一張不超過lv1/2/3的攻擊牌加到手牌"
                                : "血祭之禮(未解鎖): 每回合限一次，當你造成至少2/4/6點傷害時，可以從棄牌堆中挑一張不超過lv1/2/3的攻擊牌加到手牌",
                (Vector2){x, y}, 20, 1, unlock_def_lv2 ? YELLOW : GRAY); y += line_gap;
            if (!unlock_def_lv2) {
                DrawTextEx(font, "解鎖條件: 購買兩張技能2的防禦技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            DrawTextEx(font,
                unlock_def_cache ? "強制治療B: 立即回復5點生命，使用後將此效果移除"
                                : "強制治療B(未解鎖): 立即回復5點生命，使用後將此效果移除",
                (Vector2){x, y}, 20, 1, unlock_def_cache ? YELLOW : GRAY); y += line_gap;
            if (!unlock_def_cache) {
                DrawTextEx(font, "解鎖條件: 購買三張等級3的防禦技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            y += 12;
            DrawTextEx(font, "移動被動", (Vector2){x, y}, 20, 1, PURPLE); y += line_gap;

            DrawTextEx(font,
                unlock_mov_lv2 ? "精神屏障: 使用移動技能時，獲得等同等級的防禦"
                                : "精神屏障(未解鎖): 使用移動技能時，獲得等同等級的防禦",
                (Vector2){x, y}, 20, 1, unlock_mov_lv2 ? YELLOW : GRAY); y += line_gap;
            if (!unlock_mov_lv2) {
                DrawTextEx(font, "解鎖條件: 購買兩張技能2的移動技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            DrawTextEx(font,
                unlock_mov_cache ? "強制治療C: 立即回復5點生命，使用後將此效果移除"
                                : "強制治療C(未解鎖): 立即回復5點生命，使用後將此效果移除",
                (Vector2){x, y}, 20, 1, unlock_mov_cache ? YELLOW : GRAY); y += line_gap;
            if (!unlock_mov_cache) {
                DrawTextEx(font, "解鎖條件: 購買三張等級3的移動技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }
            break;
        
        case 3: // 愛麗絲
            DrawTextEx(font, "- 更換身份觸發不同能力", (Vector2){ 120, 140 }, 24, 1, SKYBLUE);

            // 🟥 攻擊被動
            DrawTextEx(font, "攻擊被動", (Vector2){x, y}, 20, 1, RED); y += line_gap;

            DrawTextEx(font,
                unlock_atk_lv2 ? "砍掉她的頭: 紅心皇后狀態下，攻擊技能將牌放入棄牌堆時，可改為加入手牌"
                               : "砍掉她的頭(未解鎖): 紅心皇后狀態下，攻擊技能將牌放入棄牌堆時，可改為加入手牌",
                (Vector2){x, y}, 20, 1, unlock_atk_lv2 ? YELLOW : GRAY); y += line_gap;
            if (!unlock_atk_lv2) {
                DrawTextEx(font, "解鎖條件: 購買兩張技能2的攻擊技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            DrawTextEx(font,
                unlock_atk_cache ? "開始我的表演A: 回合結束抽牌時，額外抽1張(可疊加)"
                                : "開始我的表演A(未解鎖): 回合結束抽牌時，額外抽1張(可疊加)",
                (Vector2){x, y}, 20, 1, unlock_atk_cache ? YELLOW : GRAY); y += line_gap;
            if (!unlock_atk_cache) {
                DrawTextEx(font, "解鎖條件: 購買三張等級3的攻擊技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            // 🟩 防禦被動
            y += 12;
            DrawTextEx(font, "防禦被動", (Vector2){x, y}, 20, 1, GREEN); y += line_gap;

            DrawTextEx(font,
                unlock_def_lv2 ? "仙境降臨: 瘋帽子狀態下，防禦技能將牌放入棄牌堆時，可改為加入手牌"
                               : "仙境降臨(未解鎖): 瘋帽子狀態下，防禦技能將牌放入棄牌堆時，可改為加入手牌",
                (Vector2){x, y}, 20, 1, unlock_def_lv2 ? YELLOW : GRAY); y += line_gap;
            if (!unlock_def_lv2) {
                DrawTextEx(font, "解鎖條件: 購買兩張技能2的防禦技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            DrawTextEx(font,
                unlock_def_cache ? "開始我的表演B: 回合結束抽牌時，額外抽1張(可疊加)"
                                : "開始我的表演B(未解鎖): 回合結束抽牌時，額外抽1張(可疊加)",
                (Vector2){x, y}, 20, 1, unlock_def_cache ? YELLOW : GRAY); y += line_gap;
            if (!unlock_def_cache) {
                DrawTextEx(font, "解鎖條件: 購買三張等級3的防禦技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            // 🟪 移動被動
            y += 12;
            DrawTextEx(font, "移動被動", (Vector2){x, y}, 20, 1, PURPLE); y += line_gap;

            DrawTextEx(font,
                unlock_mov_lv2 ? "我們全是瘋子: 柴郡貓狀態下，穿過對手或被穿過時可抽1張牌"
                               : "我們全是瘋子(未解鎖): 柴郡貓狀態下，穿過對手或被穿過時可抽1張牌",
                (Vector2){x, y}, 20, 1, unlock_mov_lv2 ? YELLOW : GRAY); y += line_gap;
            if (!unlock_mov_lv2) {
                DrawTextEx(font, "解鎖條件: 購買兩張技能2的移動技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            DrawTextEx(font,
                unlock_mov_cache ? "開始我的表演C: 回合結束抽牌時，額外抽1張(可疊加)"
                                : "開始我的表演C(未解鎖): 回合結束抽牌時，額外抽1張(可疊加)",
                (Vector2){x, y}, 20, 1, unlock_mov_cache ? YELLOW : GRAY); y += line_gap;
            if (!unlock_mov_cache) {
                DrawTextEx(font, "解鎖條件: 購買三張等級3的移動技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }
            break;

        case 4: // 花木蘭
            DrawTextEx(font, "- 累積氣提升爆發力", (Vector2){ 120, 140 }, 24, 1, SKYBLUE);

            // 🟥 攻擊被動
            DrawTextEx(font, "攻擊被動", (Vector2){x, y}, 20, 1, RED); y += line_gap;

            DrawTextEx(font,
                unlock_atk_lv2 ? "氣貫全身: 使用攻擊技能或行動時，可消耗x點氣使傷害+x (最多3)"
                               : "氣貫全身(未解鎖): 使用攻擊技能或行動時，可消耗x點氣使傷害+x (最多3)",
                (Vector2){x, y}, 20, 1, unlock_atk_lv2 ? YELLOW : GRAY); y += line_gap;
            if (!unlock_atk_lv2) {
                DrawTextEx(font, "解鎖條件: 購買兩張技能2的攻擊技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            DrawTextEx(font,
                unlock_atk_cache ? "暴風前夕A: 回合開始時，獲得1點氣 (可疊加)"
                                : "暴風前夕A(未解鎖): 回合開始時，獲得1點氣 (可疊加)",
                (Vector2){x, y}, 20, 1, unlock_atk_cache ? YELLOW : GRAY); y += line_gap;
            if (!unlock_atk_cache) {
                DrawTextEx(font, "解鎖條件: 購買三張等級3的攻擊技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            // 🟩 防禦被動
            y += 12;
            DrawTextEx(font, "防禦被動", (Vector2){x, y}, 20, 1, GREEN); y += line_gap;

            DrawTextEx(font,
                unlock_def_lv2 ? "主宰命運: 在你回合結束階段抽取卡牌之後，你可以棄掉1張手牌，之後再抽取1張牌"
                               : "主宰命運(未解鎖): 在你回合結束階段抽取卡牌之後，你可以棄掉1張手牌，之後再抽取1張牌",
                (Vector2){x, y}, 20, 1, unlock_def_lv2 ? YELLOW : GRAY); y += line_gap;
            if (!unlock_def_lv2) {
                DrawTextEx(font, "解鎖條件: 購買兩張技能2的防禦技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            DrawTextEx(font,
                unlock_def_cache ? "暴風前夕B: 回合開始時，獲得1點氣 (可疊加)"
                                : "暴風前夕B(未解鎖): 回合開始時，獲得1點氣 (可疊加)",
                (Vector2){x, y}, 20, 1, unlock_def_cache ? YELLOW : GRAY); y += line_gap;
            if (!unlock_def_cache) {
                DrawTextEx(font, "解鎖條件: 購買三張等級3的防禦技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            // 🟪 移動被動
            y += 12;
            DrawTextEx(font, "移動被動", (Vector2){x, y}, 20, 1, PURPLE); y += line_gap;

            DrawTextEx(font,
                unlock_mov_lv2 ? "長驅直入: 使用移動行動時，可消耗x點氣額外移動x格 (最多3)"
                               : "長驅直入(未解鎖): 使用移動行動時，可消耗x點氣額外移動x格 (最多3)",
                (Vector2){x, y}, 20, 1, unlock_mov_lv2 ? YELLOW : GRAY); y += line_gap;
            if (!unlock_mov_lv2) {
                DrawTextEx(font, "解鎖條件: 購買兩張技能2的移動技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            DrawTextEx(font,
                unlock_mov_cache ? "暴風前夕C: 回合開始時，獲得1點氣 (可疊加)"
                                : "暴風前夕C(未解鎖): 回合開始時，獲得1點氣 (可疊加)",
                (Vector2){x, y}, 20, 1, unlock_mov_cache ? YELLOW : GRAY); y += line_gap;
            if (!unlock_mov_cache) {
                DrawTextEx(font, "解鎖條件: 購買三張等級3的移動技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }
            break;

        case 5: // 輝夜姬
            DrawTextEx(font, "- 月之力會吞噬一切來犯之敵", (Vector2){ 120, 140 }, 24, 1, SKYBLUE);

            // 🟥 攻擊被動
            DrawTextEx(font, "攻擊被動", (Vector2){x, y}, 20, 1, RED); y += line_gap;

            DrawTextEx(font,
                unlock_atk_lv2 ? "懲戒時刻: 防禦上限+1，每回合限1次，可將防禦牌當作同等級攻擊牌"
                               : "懲戒時刻(未解鎖): 防禦上限+1，每回合限1次，可將防禦牌當作同等級攻擊牌",
                (Vector2){x, y}, 20, 1, unlock_atk_lv2 ? YELLOW : GRAY); y += line_gap;
            if (!unlock_atk_lv2) {
                DrawTextEx(font, "解鎖條件: 購買兩張技能2的攻擊技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            DrawTextEx(font,
                unlock_atk_cache ? "月下沉思A: 回合結束時，獲得2點防禦"
                                : "月下沉思A(未解鎖): 回合結束時，獲得2點防禦",
                (Vector2){x, y}, 20, 1, unlock_atk_cache ? YELLOW : GRAY); y += line_gap;
            if (!unlock_atk_cache) {
                DrawTextEx(font, "解鎖條件: 購買三張等級3的攻擊技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            // 🟩 防禦被動
            y += 12;
            DrawTextEx(font, "防禦被動", (Vector2){x, y}, 20, 1, GREEN); y += line_gap;

            DrawTextEx(font,
                unlock_def_lv2 ? "血色月光: 防禦上限+1，清理每3點防禦可抽1張牌"
                               : "血色月光(未解鎖): 防禦上限+1，清理每3點防禦可抽1張牌",
                (Vector2){x, y}, 20, 1, unlock_def_lv2 ? YELLOW : GRAY); y += line_gap;
            if (!unlock_def_lv2) {
                DrawTextEx(font, "解鎖條件: 購買兩張技能2的防禦技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            DrawTextEx(font,
                unlock_def_cache ? "月下沉思B: 回合結束時，獲得2點防禦"
                                : "月下沉思B(未解鎖): 回合結束時，獲得2點防禦",
                (Vector2){x, y}, 20, 1, unlock_def_cache ? YELLOW : GRAY); y += line_gap;
            if (!unlock_def_cache) {
                DrawTextEx(font, "解鎖條件: 購買三張等級3的防禦技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            // 🟪 移動被動
            y += 12;
            DrawTextEx(font, "移動被動", (Vector2){x, y}, 20, 1, PURPLE); y += line_gap;

            DrawTextEx(font,
                unlock_mov_lv2 ? "靈性本能: 防禦上限+1，每回合限1次，若防禦高於敵可推動他1格"
                               : "靈性本能(未解鎖): 防禦上限+1，每回合限1次，若防禦高於敵可推動他1格",
                (Vector2){x, y}, 20, 1, unlock_mov_lv2 ? YELLOW : GRAY); y += line_gap;
            if (!unlock_mov_lv2) {
                DrawTextEx(font, "解鎖條件: 購買兩張技能2的移動技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            DrawTextEx(font,
                unlock_mov_cache ? "月下沉思C: 回合結束時，獲得2點防禦"
                                : "月下沉思C(未解鎖): 回合結束時，獲得2點防禦",
                (Vector2){x, y}, 20, 1, unlock_mov_cache ? YELLOW : GRAY); y += line_gap;
            if (!unlock_mov_cache) {
                DrawTextEx(font, "解鎖條件: 購買三張等級3的移動技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }
            break;

        case 6: // 美人魚
            DrawTextEx(font, "- 操控觸手進行遠距打擊", (Vector2){ 120, 140 }, 24, 1, SKYBLUE);

            // 🟥 攻擊被動
            DrawTextEx(font, "攻擊被動", (Vector2){x, y}, 20, 1, RED); y += line_gap;

            DrawTextEx(font,
                unlock_atk_lv2 ? "暴風之蝕: 對手在觸手上時，可無視距離發動攻擊行動(基本牌)"
                               : "暴風之蝕(未解鎖): 對手在觸手上時，可無視距離發動攻擊行動(基本牌)",
                (Vector2){x, y}, 20, 1, unlock_atk_lv2 ? YELLOW : GRAY); y += line_gap;
            if (!unlock_atk_lv2) {
                DrawTextEx(font, "解鎖條件: 購買兩張技能2的攻擊技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            DrawTextEx(font,
                unlock_atk_cache ? "暗潮湧動A(已生效): 獲得一個觸手放在任意位置，若遠古甦醒生效則放腳下並移除此效果"
                                : "暗潮湧動A(未解鎖): 獲得一個觸手放在任意位置，若遠古甦醒生效則放腳下並移除此效果",
                (Vector2){x, y}, 20, 1, unlock_atk_cache ? YELLOW : GRAY); y += line_gap;
            if (!unlock_atk_cache) {
                DrawTextEx(font, "解鎖條件: 購買三張等級3的攻擊技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            // 🟩 防禦被動
            y += 12;
            DrawTextEx(font, "防禦被動", (Vector2){x, y}, 20, 1, GREEN); y += line_gap;

            DrawTextEx(font,
                unlock_def_lv2 ? "神秘共鳴: 位於觸手上時，對手對你造成的傷害-1"
                               : "神秘共鳴(未解鎖): 位於觸手上時，對手對你造成的傷害-1",
                (Vector2){x, y}, 20, 1, unlock_def_lv2 ? YELLOW : GRAY); y += line_gap;
            if (!unlock_def_lv2) {
                DrawTextEx(font, "解鎖條件: 購買兩張技能2的防禦技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            DrawTextEx(font,
                unlock_def_cache ? "暗潮湧動B(已生效): 獲得一個觸手放在任意位置，若遠古甦醒生效則放腳下並移除此效果"
                                : "暗潮湧動B(未解鎖): 獲得一個觸手放在任意位置，若遠古甦醒生效則放腳下並移除此效果",
                (Vector2){x, y}, 20, 1, unlock_def_cache ? YELLOW : GRAY); y += line_gap;
            if (!unlock_def_cache) {
                DrawTextEx(font, "解鎖條件: 購買三張等級3的防禦技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            // 🟪 移動被動
            y += 12;
            DrawTextEx(font, "移動被動", (Vector2){x, y}, 20, 1, PURPLE); y += line_gap;

            DrawTextEx(font,
                unlock_mov_lv2 ? "海的女兒: 移動後可將一個觸手向任意位置移動x/2格(向下取整)"
                               : "海的女兒(未解鎖): 移動後可將一個觸手向任意位置移動x/2格(向下取整)",
                (Vector2){x, y}, 20, 1, unlock_mov_lv2 ? YELLOW : GRAY); y += line_gap;
            if (!unlock_mov_lv2) {
                DrawTextEx(font, "解鎖條件: 購買兩張技能2的移動技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            DrawTextEx(font,
                unlock_mov_cache ? "暗潮湧動C(已生效): 獲得一個觸手放在任意位置，若遠古甦醒生效則放腳下並移除此效果"
                                : "暗潮湧動C(未解鎖): 獲得一個觸手任意位置，若遠古甦醒生效則放腳下並移除此效果",
                (Vector2){x, y}, 20, 1, unlock_mov_cache ? YELLOW : GRAY); y += line_gap;
            if (!unlock_mov_cache) {
                DrawTextEx(font, "解鎖條件: 購買三張等級3的移動技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }
            break;

        case 7: // 火柴女孩
            DrawTextEx(font, "- 操控火柴改變戰局", (Vector2){ 120, 140 }, 24, 1, SKYBLUE);

            // 🟥 攻擊被動
            DrawTextEx(font, "攻擊被動", (Vector2){x, y}, 20, 1, RED); y += line_gap;

            DrawTextEx(font,
                unlock_atk_lv2 ? "痛苦的儀式: 攻擊技能或行動時，可將對手棄牌1張放回火柴牌庫，傷害+2"
                               : "痛苦的儀式(未解鎖): 攻擊技能或行動時，可將對手棄牌1張放回火柴牌庫，傷害+2",
                (Vector2){x, y}, 20, 1, unlock_atk_lv2 ? YELLOW : GRAY); y += line_gap;
            if (!unlock_atk_lv2) {
                DrawTextEx(font, "解鎖條件: 購買兩張技能2的攻擊技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            DrawTextEx(font,
                unlock_atk_cache ? "火焰的捉弄: 對手用火柴牌時，你+1能量，火柴牌不能作為攻擊"
                                : "火焰的捉弄(未解鎖): 對手用火柴牌時，你+1能量，火柴牌不能作為攻擊",
                (Vector2){x, y}, 20, 1, unlock_atk_cache ? YELLOW : GRAY); y += line_gap;
            if (!unlock_atk_cache) {
                DrawTextEx(font, "解鎖條件: 購買三張等級3的攻擊技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            // 🟩 防禦被動
            y += 12;
            DrawTextEx(font, "防禦被動", (Vector2){x, y}, 20, 1, GREEN); y += line_gap;

            DrawTextEx(font,
                unlock_def_lv2 ? "放縱的渴望: 使用防禦技能時，可回收對手棄牌1張火柴並抽1牌"
                               : "放縱的渴望(未解鎖): 使用防禦技能時，可回收對手棄牌1張火柴並抽1牌",
                (Vector2){x, y}, 20, 1, unlock_def_lv2 ? YELLOW : GRAY); y += line_gap;
            if (!unlock_def_lv2) {
                DrawTextEx(font, "解鎖條件: 購買兩張技能2的防禦技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            DrawTextEx(font,
                unlock_def_cache ? "欲望的捉弄: 對手用火柴牌時，你+1能量，火柴牌不能作為防禦"
                                : "欲望的捉弄(未解鎖): 對手用火柴牌時，你+1能量，火柴牌不能作為防禦",
                (Vector2){x, y}, 20, 1, unlock_def_cache ? YELLOW : GRAY); y += line_gap;
            if (!unlock_def_cache) {
                DrawTextEx(font, "解鎖條件: 購買三張等級3的防禦技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            // 🟪 移動被動
            y += 12;
            DrawTextEx(font, "移動被動", (Vector2){x, y}, 20, 1, PURPLE); y += line_gap;

            DrawTextEx(font,
                unlock_mov_lv2 ? "魔鬼的凝視: 承受傷害失血時，可回收對手棄牌1張火柴並移動1格"
                               : "魔鬼的凝視(未解鎖): 承受傷害失血時，可回收對手棄牌1張火柴並移動1格",
                (Vector2){x, y}, 20, 1, unlock_mov_lv2 ? YELLOW : GRAY); y += line_gap;
            if (!unlock_mov_lv2) {
                DrawTextEx(font, "解鎖條件: 購買兩張技能2的移動技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            DrawTextEx(font,
                unlock_mov_cache ? "命運的捉弄: 對手用火柴牌時，你+1能量，火柴牌不能作為移動"
                                : "命運的捉弄(未解鎖): 對手用火柴牌時，你+1能量，火柴牌不能作為移動",
                (Vector2){x, y}, 20, 1, unlock_mov_cache ? YELLOW : GRAY); y += line_gap;
            if (!unlock_mov_cache) {
                DrawTextEx(font, "解鎖條件: 購買三張等級3的移動技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }
            break;

        case 8: // 桃樂絲
            DrawTextEx(font, "- 高速連擊獲得 token", (Vector2){ 120, 140 }, 24, 1, SKYBLUE);

            // 🟥 攻擊被動
            DrawTextEx(font, "攻擊被動", (Vector2){x, y}, 20, 1, RED); y += line_gap;

            DrawTextEx(font,
                unlock_atk_lv2 ? "殺戮指令: 若連續攻擊傷害遞增，完成連擊並獲得1連擊token"
                               : "殺戮指令(未解鎖): 若連續攻擊傷害遞增，完成連擊並獲得1連擊token",
                (Vector2){x, y}, 20, 1, unlock_atk_lv2 ? YELLOW : GRAY); y += line_gap;
            if (!unlock_atk_lv2) {
                DrawTextEx(font, "解鎖條件: 購買兩張技能2的攻擊技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            DrawTextEx(font,
                unlock_atk_cache ? "無所遁形A: 每完成1次連擊，額外+1連擊token"
                                : "無所遁形A(未解鎖): 每完成1次連擊，額外+1連擊token",
                (Vector2){x, y}, 20, 1, unlock_atk_cache ? YELLOW : GRAY); y += line_gap;
            if (!unlock_atk_cache) {
                DrawTextEx(font, "解鎖條件: 購買三張等級3的攻擊技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            // 🟩 防禦被動
            y += 12;
            DrawTextEx(font, "防禦被動", (Vector2){x, y}, 20, 1, GREEN); y += line_gap;

            DrawTextEx(font,
                unlock_def_lv2 ? "超越機器: 若後者技能等級大於前者，完成連擊並獲得1連擊token"
                               : "超越機器(未解鎖): 若後者技能等級大於前者，完成連擊並獲得1連擊token",
                (Vector2){x, y}, 20, 1, unlock_def_lv2 ? YELLOW : GRAY); y += line_gap;
            if (!unlock_def_lv2) {
                DrawTextEx(font, "解鎖條件: 購買兩張技能2的防禦技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            DrawTextEx(font,
                unlock_def_cache ? "無所遁形B: 每完成1次連擊，額外+1連擊token"
                                : "無所遁形B(未解鎖): 每完成1次連擊，額外+1連擊token",
                (Vector2){x, y}, 20, 1, unlock_def_cache ? YELLOW : GRAY); y += line_gap;
            if (!unlock_def_cache) {
                DrawTextEx(font, "解鎖條件: 購買三張等級3的防禦技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            // 🟪 移動被動
            y += 12;
            DrawTextEx(font, "移動被動", (Vector2){x, y}, 20, 1, PURPLE); y += line_gap;

            DrawTextEx(font,
                unlock_mov_lv2 ? "獲准極刑: 使用任一技能後，可棄移動牌使對手被移動x格"
                               : "獲准極刑(未解鎖): 使用任一技能後，可棄移動牌使對手被移動x格",
                (Vector2){x, y}, 20, 1, unlock_mov_lv2 ? YELLOW : GRAY); y += line_gap;
            if (!unlock_mov_lv2) {
                DrawTextEx(font, "解鎖條件: 購買兩張技能2的移動技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            DrawTextEx(font,
                unlock_mov_cache ? "無所遁形C: 每完成1次連擊，額外+1連擊token"
                                : "無所遁形C(未解鎖): 每完成1次連擊，額外+1連擊token",
                (Vector2){x, y}, 20, 1, unlock_mov_cache ? YELLOW : GRAY); y += line_gap;
            if (!unlock_mov_cache) {
                DrawTextEx(font, "解鎖條件: 購買三張等級3的移動技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }
            break;

        case 9: // 山魯佐德
            DrawTextEx(font, "- 操弄命運與藍色token", (Vector2){ 120, 140 }, 24, 1, SKYBLUE);

            // 🟥 攻擊被動
            DrawTextEx(font, "攻擊被動", (Vector2){x, y}, 20, 1, RED); y += line_gap;

            DrawTextEx(font,
                unlock_atk_lv2 ? "命運之手: 攻擊造成3點以上傷害時，可將1枚藍色token翻為紅色"
                               : "命運之手(未解鎖): 攻擊造成3點以上傷害時，可將1枚藍色token翻為紅色",
                (Vector2){x, y}, 20, 1, unlock_atk_lv2 ? YELLOW : GRAY); y += line_gap;
            if (!unlock_atk_lv2) {
                DrawTextEx(font, "解鎖條件: 購買兩張技能2的攻擊技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            DrawTextEx(font,
                unlock_atk_cache ? "童話編織者A: 獲得1枚藍色token並放置至任一對手供應排庫上"
                                : "童話編織者A(未解鎖): 獲得1枚藍色token並放置至任一對手供應牌庫上",
                (Vector2){x, y}, 20, 1, unlock_atk_cache ? YELLOW : GRAY); y += line_gap;
            if (!unlock_atk_cache) {
                DrawTextEx(font, "解鎖條件: 購買三張等級3的攻擊技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            // 🟩 防禦被動
            y += 12;
            DrawTextEx(font, "防禦被動", (Vector2){x, y}, 20, 1, GREEN); y += line_gap;

            DrawTextEx(font,
                unlock_def_lv2 ? "改寫欲望: 對手購牌時，該牌庫每有1枚token就失去1點生命"
                               : "改寫欲望(未解鎖): 對手購牌時，該牌庫每有1枚token就失去1點生命",
                (Vector2){x, y}, 20, 1, unlock_def_lv2 ? YELLOW : GRAY); y += line_gap;
            if (!unlock_def_lv2) {
                DrawTextEx(font, "解鎖條件: 購買兩張技能2的防禦技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            DrawTextEx(font,
                unlock_def_cache ? "童話編織者B: 獲得1枚藍色token並放置至任一對手供應排庫上"
                                : "童話編織者B(未解鎖): 獲得1枚藍色token並放置至任一對手供應牌庫上",
                (Vector2){x, y}, 20, 1, unlock_def_cache ? YELLOW : GRAY); y += line_gap;
            if (!unlock_def_cache) {
                DrawTextEx(font, "解鎖條件: 購買三張等級3的防禦技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            // 🟪 移動被動
            y += 12;
            DrawTextEx(font, "移動被動", (Vector2){x, y}, 20, 1, PURPLE); y += line_gap;

            DrawTextEx(font,
                unlock_mov_lv2 ? "重組思想: 對手抽牌前展示其牌庫頂2張，有命運token則損血並棄該牌"
                               : "重組思想(未解鎖): 對手抽牌前展示其牌庫頂2張，有命運token則損血並棄該牌",
                (Vector2){x, y}, 20, 1, unlock_mov_lv2 ? YELLOW : GRAY); y += line_gap;
            if (!unlock_mov_lv2) {
                DrawTextEx(font, "解鎖條件: 購買兩張技能2的移動技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }

            DrawTextEx(font,
                unlock_mov_cache ? "童話編織者C: 獲得1枚藍色token並放置至任一對手供應排庫上"
                                : "童話編織者C(未解鎖): 獲得1枚藍色token並放置至任一對手供應牌庫上",
                (Vector2){x, y}, 20, 1, unlock_mov_cache ? YELLOW : GRAY); y += line_gap;
            if (!unlock_mov_cache) {
                DrawTextEx(font, "解鎖條件: 購買三張等級3的移動技能牌", (Vector2){x + 20, y}, 18, 1, GRAY); y += line_gap;
            }
            break;


        // 其他角色依需求增加
        default:
            DrawTextEx(font, "- 無資料", (Vector2){ 120, 140 }, 24, 1, SKYBLUE);
            break;
    }

    Rectangle back_btn = { GetScreenWidth() - 180.0f, GetScreenHeight() - 70.0f, 160, 50 };
    bool hover = CheckCollisionPointRec(GetMousePosition(), back_btn);
    DrawRectangleRec(back_btn, hover ? ORANGE : DARKGRAY);
    DrawTextEx(font, "返回", (Vector2){ back_btn.x + 50, back_btn.y + 15 }, 20, 1, WHITE);

    if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Game* g = (Game*)game;
        g->current_state = GAME_STATE_HUMAN_TURN;
    }
}

void DrawPassiveButton(Rectangle bounds, const char* text, bool isHovered, bool isSelected) {
    // 🎀 粉紅主題配色
    Color borderColor = WHITE;
    Color fillColor   = isSelected ? RED :
                        (isHovered ? MAGENTA : PINK);
    Color textColor   = WHITE;

    // 底色 + 邊框
    DrawRectangleRounded(bounds, 0.3f, 6, fillColor);
    DrawRectangleRoundedLines(bounds, 0.3f, 6, borderColor);

    // 文字置中
    Vector2 textSize = MeasureTextEx(font, text, 20, 1);
    DrawTextEx(font, text,
        (Vector2){
            bounds.x + (bounds.width  - textSize.x) / 2,
            bounds.y + (bounds.height - textSize.y) / 2
        },
        20, 1, textColor);
}

void DrawUltraOverlay(const Game* game) {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.85f));
    DrawTextEx(font, "角色必殺技", (Vector2){100, 80}, 40, 2, WHITE);
    float screenWidth = GetScreenWidth();
    float screenHeight = GetScreenHeight();

    const player* p = &game->inner_game.players[0];
    int chara = p->character;

    int base_id = 801 + chara * 10;  // ex: 小紅帽 801~803

    for (int i = 0; i < 3; ++i) {
        int card_id = base_id + i;
        const Card* c = get_card_info(card_id);
        if (!c) continue;

        float spacing = 190; // 卡片間距
        float total_width = 3 * ULTRA_CARD_WIDTH + 2 * spacing;
        float start_x = (GetScreenWidth() - total_width) / 2;

        Rectangle rect = {
            start_x + i * (ULTRA_CARD_WIDTH + spacing),
            240,  // ✅ Y 軸往下移一點（原本 180）
            ULTRA_CARD_WIDTH,
            ULTRA_CARD_HEIGHT
        };

        bool hovered = CheckCollisionPointRec(GetMousePosition(), rect);
        DrawCard(c, rect, hovered, false);


        // 說明文字
        const char* desc = "";

        switch (card_id) {
            // 小紅帽
            case 801:
                desc = "將任意技能供應牌庫頂部\n兩張加入手牌\n這不是一個購買行動";
                break;
            case 802:
                desc = "重複你剛使用的\n攻擊、移動技能或行動效果\n(可重複蛻變牌效果)";
                break;
            case 803:
                desc = "射程3 傷害3 \n擊退對手至多三格\n之後對手捨棄三張手牌";
                break;

            // 白雪公主
            case 811:
                desc = "射程1 對手失去X點生命\nX為其棄牌堆中中毒牌數量";
                break;
            case 812:
                desc = "射程1 傷害3\n對手棄掉全部手牌後抽4張";
                break;
            case 813:
                desc = "射程3 傷害3\n可將對手棄牌堆中至多3張牌洗回牌庫";
                break;

            // 睡美人 Sleeping Beauty
            case 821:
                desc = "無論處於何種狀態\n立即獲得3個覺醒TOKEN（可超過上限）\n若仍為沉睡狀態，可立即覺醒";
                break;
            case 822:
                desc = "[持續]直到下回合開始\n每當你因傷害失去生命時\n抽等量張數的牌（最多6張）";
                break;
            case 823:
                desc = "無論處於何種狀態\n花費所有覺醒TOKEN\n每花費1個，恢復1點生命";
                break;

            // 愛麗絲 Alice
            case 831:
                desc = "在本回合結束階段抽牌後\n將手牌棄至4張 並立即開始新回合\n（最多連續使用3次）";
                break;
            case 832:
                desc = "直到回合結束前\n你的攻擊、防禦、移動牌數值 +1\n（可與其他效果累積）";
                break;
            case 833:
                desc = "射程3 傷害3\n將手牌與/或棄牌堆中\n最多5張基本牌洗入對手牌庫";
                break;
            
            // 花木蘭 Mulan
            case 841:
                desc = "在本回合結束階段抽牌時\n額外抽取4張牌";
                break;
            case 842:
                desc = "將你移動至與對手相鄰的格子\n並立即獲得3點氣";
                break;
            case 843:
                desc = "射程1\n立即花費所有氣來造成X點傷害\nX等於你花費的氣的量";
                break;

            // 輝夜姬 Kaguya
            case 851:
                desc = "[持續]直到你下回合開始前\n你不會承受傷害，也不會失去生命";
                break;
            case 852:
                desc = "防禦6\n[持續]下回合開始階段\n若對手防禦低於你，他失去等量生命";
                break;
            case 853:
                desc = "將對手移動到與你相鄰的格子並造成3傷害\n[持續]下回合開始時\n再次將對手拉近並造成3傷害";
                break;

            // 美人魚 Mermaid
            case 861:
                desc = "你的生命上限變為18\n在你每個回合結束時恢復2點生命";
                break;
            case 862:
                desc = "將所有觸手移動到你的位置上\n對手對你造成的傷害-X，\nX為你身上的觸手數\n你的技能永久變為對應等級的基本牌\n每回合結束時你失去1點生命";
                break;
            case 863:
                desc = "若觸手進入對手格子：對手失去1點生命\n若對手進入有觸手的格子：他失去X點生命\nX為該格觸手數量";
                break;

            // 火柴女孩 Match Girl
            case 871:
                desc = "射程1 傷害X\nX為你目前能量數的一半（向上取整）";
                break;
            case 872:
                desc = "射程3\n棄掉對手牌庫頂6張\n每有一張火柴，對手失去1點生命";
                break;
            case 873:
                desc = "從火柴牌庫中選擇最多3張火柴\n放置到對手牌庫頂部";
                break;

            // 逃樂絲 Dorothy
            case 881:
                desc = "射程1\n花費X個連擊TOKEN\n造成X點傷害並獲得X點能量";
                break;
            case 882:
                desc = "花費X個連擊TOKEN\n恢復X點生命";
                break;
            case 883:
                desc = "花費X個連擊TOKEN\n抽取 X / 2 張牌（向上取整）";
                break;

            // 山魯佐德 Scheherazade
            case 891:
                desc = "射程3 傷害3\n選擇一個有命運TOKEN的供應牌庫\n棄掉對手牌庫中所有來自該牌庫的牌\n每棄1張造成1傷害，最後重洗其牌庫";
                break;
            case 892:
                desc = "射程3 傷害3\n選擇一個有命運TOKEN的供應牌庫\n對手棄牌堆每有1張來自該牌庫的牌\n此攻擊+1 傷害（紅色TOKEN則+2）";
                break;
            case 893:
                desc = "射程3 傷害X\n將至多3枚藍色命運TOKEN翻為紅色\nX等於此次翻轉的數量";
                break;

            default:
                desc = "";
                break;
        }

        // 分行顯示說明（黃色、大字、偏左）
        int line_height = 22;
        int font_size = 20;
        float margin_x = -10;  // 偏左一點
        int y_offset = 0;
        char buffer[128];
        const char* ptr = desc;

        while (*ptr) {
            int i = 0;
            while (ptr[i] && ptr[i] != '\n') i++;
            strncpy(buffer, ptr, i);
            buffer[i] = '\0';

            DrawTextEx(font, buffer,
                (Vector2){
                    rect.x + margin_x,
                    rect.y + ULTRA_CARD_HEIGHT + 12 + y_offset
                },
                font_size, 1, YELLOW);

            y_offset += line_height;
            ptr += i;
            if (*ptr == '\n') ptr++;
        }

    }

    // --- 繪製關閉按鈕 ---
    Rectangle close_btn = { screenWidth - 160, screenHeight - 70, 140, 50 };
    bool hover = CheckCollisionPointRec(GetMousePosition(), close_btn);
    DrawRectangleRec(close_btn, hover ? RED : MAROON);
    DrawTextEx(font, "Close", (Vector2){close_btn.x + 45, close_btn.y + 15}, 20, 1, WHITE);

    if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    
        ((Game*)game)->current_state = GAME_STATE_HUMAN_TURN;

    }

}
