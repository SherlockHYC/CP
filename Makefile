#
# Makefile for Twisted Fables GUI Project
#

# --- 變數定義 (Variables) ---

# 編譯器
CC = gcc

# 執行檔名稱
EXEC = TwistedFablesGUI

# 編譯參數
# -Wall -Wextra: 顯示所有警告訊息，有助於寫出更穩健的程式
# -std=c99:      使用 C99 標準
# -I.:           告訴編譯器在當前目錄尋找頭文件 (.h files)
CFLAGS = -Wall -Wextra -std=c99 -I.

# 連結參數 (Linker Flags)
# 告訴連結器需要哪些外部函式庫
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# 來源檔案 (.c files)
# 將所有需要編譯的 .c 檔案都列在這裡
SRCS = \
    main.c \
    game_logic.c \
    gui.c \
    bot_ai.c \
    database.c \
    vector.c

# 物件檔案 (.o files)
# 這會自動將上面的 .c 列表轉換成 .o 列表 (e.g., main.c -> main.o)
OBJS = $(SRCS:.c=.o)


# --- 規則定義 (Rules) ---

# 預設規則：第一個規則是 `make` 指令預設會執行的目標
# 這條規則的目的是產生最終的執行檔
all: $(EXEC)

# 連結規則：如何從所有 .o 檔案生成執行檔
$(EXEC): $(OBJS)
	@echo "正在連結最終執行檔..."
	$(CC) $(OBJS) -o $(EXEC) $(LDFLAGS)
	@echo "編譯完成！執行檔為: $(EXEC)"

# 編譯規則：如何從單一 .c 檔案生成對應的 .o 檔案
# $<: 代表來源檔案 (e.g., main.c)
# $@: 代表目標檔案 (e.g., main.o)
%.o: %.c
	@echo "正在編譯 $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# 清理規則：用來刪除所有編譯過程中產生的檔案
clean:
	@echo "正在清理編譯檔案..."
	rm -f $(EXEC) $(OBJS)
	@echo "清理完成。"

# 虛擬目標：告訴 make `all` 和 `clean` 並不是真正的檔案名稱
.PHONY: all clean

