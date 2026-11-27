CC = gcc
CFLAGS = -std=c11 -fPIC -O2 -Wall -Wextra -Igdextension
LDFLAGS = -shared -lm

SRCS = src/gd_helper.c \
       src/player.c \
       src/enemy.c \
       src/coin.c \
       src/question_block.c \
       src/flagpole.c \
       src/hud.c \
       src/main.c

TARGET = bin/libmario_game.so
ANDROID_TARGET = bin/libmario_game.android.arm64.so
ANDROID_CC ?= $(HOME)/.local/share/android-ndk/android-ndk-r27c/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android24-clang

all: $(TARGET)

$(TARGET): $(SRCS)
	@mkdir -p bin
	$(CC) $(CFLAGS) $(SRCS) $(LDFLAGS) -o $(TARGET)

android: $(ANDROID_TARGET)

$(ANDROID_TARGET): $(SRCS)
	@mkdir -p bin
	$(ANDROID_CC) $(CFLAGS) $(SRCS) $(LDFLAGS) -o $(ANDROID_TARGET)

clean:
	rm -rf bin

.PHONY: all android clean
