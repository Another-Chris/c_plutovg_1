#include <plutovg.h>
#include <SDL3/SDL.h>

typedef struct {
  int width;
  int height;
} WinData;

typedef struct {
  float center_x;
  float center_y;
  float radius;
} Circle;


int render_and_clear_sdl_surface(
    SDL_Renderer* renderer, SDL_Surface* surface);
int render_and_clear_pluto_surface(
    SDL_Renderer* renderer, plutovg_surface_t* pluto_surface);

plutovg_surface_t* pluto_render(WinData win_data, Circle circle);
