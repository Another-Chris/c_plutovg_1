#include <plutovg.h>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
  int width;
  int height;
} WinData;

typedef struct {
  float x;
  float y;
  float r;
  float vy;
  float vx;
  SDL_Color color;
} Circle;


#define GRAVITY 980.0f
#define RESTITUTION 0.8f
#define FRICTION 64.0f
#define NUM_CIRCLES 16



void init_circles(Circle* circles);


void resolve_ball_ball_collision(Circle *circle1, Circle *circle2, float dt);
void resolve_wall_ball_collision(Circle *circle, WinData win_data, float dt);



int render_and_clear_sdl_surface(
    SDL_Renderer* renderer, SDL_Surface* surface, float x, float y);
int render_and_clear_pluto_surface(
    SDL_Renderer* renderer, plutovg_surface_t* pluto_surface);
int render_text(
    char* text, TTF_Font* font, SDL_Renderer* renderer, float x, float y);

plutovg_surface_t* pluto_render(WinData win_data, Circle* circles);



