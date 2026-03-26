#include <plutovg.h>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdlib.h>

typedef struct {
  int width;
  int height;
} WinData;

typedef struct {
  float center_x;
  float center_y;
  float radius;
} Circle;


#define GRAVITY 980.0f
#define RESTITUTION 0.8f


int render_and_clear_sdl_surface(
    SDL_Renderer* renderer, SDL_Surface* surface, float x, float y);
int render_and_clear_pluto_surface(
    SDL_Renderer* renderer, plutovg_surface_t* pluto_surface);
int render_text(
    char* text, TTF_Font* font, SDL_Renderer* renderer, float x, float y);

plutovg_surface_t* pluto_render(WinData win_data, Circle circle);
