#include "vector.h"

// ### 1. 初始化一個新的 vector ###
// 功能：回傳一個新的、大小為 0 的 vector 實體。
vector initVector() {
    vector v;
    v.SIZE = 0; // 將大小設為 0，表示此向量為空
    return v;
}

// ### 2. 在 vector 尾端加入一個元素 ###
// 功能：將一個 int32_t 的值 `val` 加入到 `vec` 的末端。
// 注意：參數是 `vector*` (指標)，這樣才能修改原始的 vector。
void pushbackVector(vector* vec, int32_t val) {
    // 安全性檢查：確認 vector 是否已滿。
    // 陣列最大索引為 255，所以當 SIZE 為 256 時表示已滿。
    if (vec->SIZE >= 256) {
        fprintf(stderr, "Error: Vector is full. Cannot push back more elements.\n");
        return; // 已滿，直接返回，不執行任何操作
    }

    // 將新元素放在目前大小所指向的位置 (也就是最後一個空位)
    vec->array[vec->SIZE] = val;
    // 將大小加一
    vec->SIZE++;
}

// ### 3. 移除 vector 的最後一個元素 ###
// 功能：將 `vec` 的最後一個元素移除。
void popbackVector(vector* vec) {
    // 安全性檢查：確認 vector 是否為空。
    if (vec->SIZE == 0) {
        fprintf(stderr, "Error: Vector is empty. Cannot pop back.\n");
        return; // 為空，直接返回
    }

    // 只需將大小減一，邏輯上就代表最後一個元素被移除了。
    // 原本的資料不需要清除，下次 pushback 時會被覆蓋。
    vec->SIZE--;
}

// ### 4. 清空整個 vector ###
// 功能：移除 `vec` 中的所有元素。
void clearVector(vector* vec) {
    // 最有效率的清空方式就是把大小設為 0。
    vec->SIZE = 0;
}

// ### 5. 移除 vector 中指定索引的元素 ###
// 功能：移除 `vec` 在 `index` 位置的元素。
void eraseVector(vector* vec, int index) {
    // 安全性檢查：確認 index 是否在合法範圍內 [0, SIZE-1]。
    if (index < 0 || index >= vec->SIZE) {
        fprintf(stderr, "Error: Erase index %d is out of bounds for vector of size %u.\n", index, vec->SIZE);
        return; // 索引無效，直接返回
    }

    // 使用 memmove 將 index 後面的所有元素往前移動一格。
    // memmove 比 for 迴圈更安全也可能更有效率，它能正確處理記憶體重疊的情況。
    //
    // 來源 (source): index + 1 的記憶體位址
    // 目標 (destination): index 的記憶體位址
    // 移動的位元組數: (SIZE - index - 1) 個元素的總大小
    if (index < vec->SIZE - 1) { // 如果不是刪除最後一個元素才需要移動
        memmove(&vec->array[index], &vec->array[index + 1], (vec->SIZE - index - 1) * sizeof(int32_t));
    }

    // 將大小減一
    vec->SIZE--;
}