# Asteroids – LAB AED I Project

**Asteroids Shapes** is a simple game inspired by the classic Asteroids, developed in C using the raylib graphics library as part of the course *Laboratory of Algorithms and Data Structures I* - Computer Science.

## Description

The game consists of controlling a spaceship that can move and shoot at asteroids moving across the screen. The objective is to destroy the asteroids and survive as long as possible without losing all lives.

## Features

- Spaceship control using **WASD** keys for movement.
- Shooting with the left mouse button.
- Asteroids of different sizes that split when hit.
- Scoring system that increases when destroying asteroids.
- Lives system (starting with 3 lives).
- Simple menu screen to start the game or exit.
- When all lives are lost, the game returns to the main menu.

## Code Structure

- **Asteroid:** Structure representing asteroids, with position, velocity, size, collision radius, and active state.
- **Ship:** Structure representing the player’s ship, with position, velocity, rotation, acceleration, and active state.
- **Shot:** Structure representing shots fired by the ship, with position, velocity, and active state.
- **Game States:** The game has three main states — MENU, GAME, and EXIT — controlled by an enum `EstadoJogo`.

## Controls

- **W:** Accelerate forward.
- **S:** Accelerate backward.
- **A:** Move left.
- **D:** Move right.
- **Left Mouse Button:** Shoot.
- **In the menu:**
  - **1:** Start game.
  - **2:** Exit game.

## How to Compile

Make sure you have the raylib library installed.

You will need a MAKEFILE to run this game. Make sure in the archive make file is written the follow lines:

CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -Wshadow -Wformat=2 -Wcast-align -Wconversion -Wsign-conversion -Wnull-dereference -g3 -O0
LDFLAGS = -lraylib -lm -ldl -lpthread -lrt -lX11

BUILD_DIR = ./build/Debug

all: $(BUILD_DIR)/outDebug

$(BUILD_DIR)/outDebug: $(BUILD_DIR)/main.o
	$(CC) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/main.o: main.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf ./build

 after this, go in the bash on your IDE ( I use VsCode ).
 make clean ( if you already have a makefile)
 make
 ./build/Debug/outDebug

 May the force be with you!
 Enjoy :)

 att,
 devlaw. 
