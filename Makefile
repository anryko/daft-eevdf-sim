all:
	clang-19 -g --std=c23 main.c -lSDL2 -Wall -Wextra -Wpedantic -Wshadow -o main
