#include "vector.h"
#include <string.h> // For memmove

// 初始化一個新的 vector
vector initVector() {
    vector v;
    v.SIZE = 0;
    return v;
}

// 在 vector 尾端加入一個元素
void pushbackVector(vector* vec, int32_t val) {
    if (vec->SIZE >= 256) {
        fprintf(stderr, "Error: Vector is full. Cannot push back more elements.\n");
        return;
    }
    vec->array[vec->SIZE] = val;
    vec->SIZE++;
}

// 移除 vector 的最後一個元素
void popbackVector(vector* vec) {
    if (vec->SIZE == 0) {
        fprintf(stderr, "Error: Vector is empty. Cannot pop back.\n");
        return;
    }
    vec->SIZE--;
}

// 清空整個 vector
void clearVector(vector* vec) {
    vec->SIZE = 0;
}

// 移除 vector 中指定索引的元素
void eraseVector(vector* vec, int index) {
    // 安全性檢查：確認 index 是否在合法範圍內 [0, SIZE-1]。
    // [修正] 將 vec->SIZE 強制轉換為 (int) 來進行比較，以避免 signed/unsigned 警告。
    if (index < 0 || index >= (int)vec->SIZE) {
        fprintf(stderr, "Error: Erase index %d is out of bounds for vector of size %u.\n", index, vec->SIZE);
        return;
    }

    // 使用 memmove 將 index 後面的所有元素往前移動一格。
    // [修正] 同樣地，這裡也使用強制轉換。
    if (index < (int)vec->SIZE - 1) {
        memmove(&vec->array[index], &vec->array[index + 1], (vec->SIZE - index - 1) * sizeof(int32_t));
    }

    // 將大小減一
    vec->SIZE--;
}

void freeVector(vector* v) {
    v->SIZE = 0;
}

void removeVectorAt(vector* v, int index) {
    if (index < 0 || index >= (int)v->SIZE) return;
    for (int i = index; i < (int)v->SIZE - 1; ++i) {
        v->array[i] = v->array[i + 1];
    }
    v->SIZE--;
}