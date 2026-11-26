CC = gcc
CFLAGS = -std=c11 -fPIC -O2 -Wall -Wextra -Igodot_headers
LDFLAGS = -shared

SRCS = src/gd_helper.c \
       src/player.c \
       src/enemy.c \
       src/coin.c \
       src/question_block.c \
       src/flagpole.c \
       src/hud.c \
       src/main.c

TARGET = bin/libmario_game.so

all: $(TARGET)

$(TARGET): $(SRCS)
	@mkdir -p bin
	$(CC) $(CFLAGS) $(SRCS) $(LDFLAGS) -o $(TARGET)

clean:
	rm -rf bin

.PHONY: all clean
