#ifndef DATABASE_H
#define DATABASE_H

#include "definitions.h"  // 確保這行在你的專案裡是正確的

// 函式宣告
const Card* get_card_info(int id);

// 若你要用 get_card_by_id 名稱，也可以加上：
#define get_card_by_id get_card_info

#endif
