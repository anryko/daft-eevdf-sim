#include <stdlib.h>
#include <stddef.h>
#include <limits.h>
#include <SDL2/SDL.h>
#include <time.h>

#include "src/rbtree.c"

static int compare(const void *a, const void *b) {
    int x = *(const int *)a;
    int y = *(const int *)b;
    return (x > y) - (x < y);  // returns 1, 0, or -1
}

static void node_draw(rbnode_s *self, SDL_Renderer *renderer, int xres, int x, int y, int width) {
    SDL_Event e;
    SDL_PollEvent(&e);
    if(e.type == SDL_QUIT)
        exit(EXIT_FAILURE);

    int half = width / 2;
    int xa = x - half;
    int xb = x + half;
    int ya = y + 24;
    bool red = rbnode_is_red(self);
    SDL_SetRenderDrawColor(renderer, red ? 0xFF : 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderDrawLine(renderer, x, y, xa, ya);
    SDL_RenderDrawLine(renderer, x, y, xb, ya);
    if (self->kids[0]) {
        node_draw(self->kids[0], renderer, xres, xa, ya, half);
    }
    if (self->kids[1]) {
        node_draw(self->kids[1], renderer, xres, xb, ya, half);
    }
}

static void render(SDL_Renderer* renderer, int xres, int yres, rbtree_s *t) {
    SDL_SetRenderDrawColor(renderer, 0xC8, 0xC8, 0xC8, 0xFF);
    SDL_RenderClear(renderer);
    node_draw(t->head, renderer, xres / 1, xres / 2, yres / 4, xres / 3);
    SDL_RenderPresent(renderer);
    SDL_Delay(1);
}

int main(void) {
    int xres = 1024;
    int yres = 768;
    int cycles = 4096;
    srand(time(NULL));

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_CreateWindowAndRenderer(xres, yres, 0, &window, &renderer);
    SDL_RenderPresent(renderer);
    SDL_Delay(100);

    rbtree_s *t = rbtree_new(compare);
    for(int i = 0; i < cycles; i++) {
        int number = rand();
        rbtree_insert(t, &number, sizeof(int));
        render(renderer, xres, yres, t);
    }

    SDL_Delay(100);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    exit(EXIT_SUCCESS);
}
