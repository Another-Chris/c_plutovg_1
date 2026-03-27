
#include "main.h"

void init_circles(Circle *circles) {
  circles[0].x = 10.0f;
  circles[0].y = 10.0f;
  circles[0].r = 10.0f;
  circles[0].vx = 160.0f;
  circles[0].vy = 0.0f;
  SDL_Color c1 = {.r = 0.0, .g = 255, .b = 0, .a = 255};
  circles[0].color = c1;

  circles[1].x = 400.0f;
  circles[1].y = 10.0f;
  circles[1].r = 10.0f;
  circles[1].vx = -160.0f;
  circles[1].vy = 0.0f;
  SDL_Color c2 = {.r = 255, .g = 0, .b = 0, .a = 255};
  circles[1].color = c2;
}

void resolve_ball_ball_collision(Circle *c1, Circle *c2, float dt) {
  float dx = c2->x - c1->x;
  float dy = c2->y - c1->y;
  float dist = sqrtf(dx * dx + dy * dy);
  if (dist >= c1->r + c2->r || dist == 0) {
    return;
  }

  // for every 1 unit on collision normal, nx in x, ny in y
  float nx = dx / dist;
  float ny = dy / dist;

  // for every overlap unit on collision normal, nx * overlap on x axis
  float overlap = (c1->r + c2->r) - dist;
  c1->x -= nx * overlap * 0.5f;
  c1->y -= ny * overlap * 0.5f;
  c2->x += nx * overlap * 0.5f;
  c2->y += ny * overlap * 0.5f;

  // dvn: scalar, how much of the relative velocity is along the collision axis
  float dvx = c2->vx - c1->vx;
  float dvy = c2->vy - c1->vy;
  float dvn = dvx * nx + dvy * ny;

  // separate
  if (dvn > 0) {
    return;
  }

  // v2'-v1' = v2 + J*n - (v1 - J*n)
  // dvn' = dvn + 2*J
  // dvn' = -e * dvn
  // J (impuse) is the scalar
  float impuse = -(1.0f + RESTITUTION) * dvn / 2.0f;

  // printf("collide! dvn: %.4f, impuse: %.4f\n", dvn, impuse);

  c1->vx -= impuse * nx;
  c1->vy -= impuse * ny;
  c2->vx += impuse * nx;
  c2->vy += impuse * ny;
}

void resolve_wall_ball_collision(Circle *circle, WinData win_data, float dt) {
  circle->vy += (GRAVITY * dt);

  // vertical moving
  circle->y += (circle->vy * dt);
  if (circle->y + circle->r >= win_data.height) {
    circle->y = win_data.height - circle->r;
    circle->vy = -circle->vy * RESTITUTION;
    if (SDL_fabsf(circle->vy) < 10) {
      circle->vy = 0;
      circle->y = win_data.height - circle->r;
    }
  }

  // horizontal moving
  // friction
  if (circle->y + circle->r == win_data.height) {
    if (circle->vx > 0) {
      circle->vx -= FRICTION * dt;
    } else if (circle->vx < 0) {
      circle->vx += FRICTION * dt;
    }
  }

  // moving
  circle->x += (circle->vx * dt);
  if (circle->x + circle->r >= win_data.width) {
    circle->x = win_data.width - circle->r;
    circle->vx = -circle->vx * RESTITUTION;
    if (SDL_fabsf(circle->vx) < 10) {
      circle->vx = 0;
    }
  } else if (circle->x - circle->r <= 0) {
    circle->x = circle->r;
    circle->vx = -circle->vx * RESTITUTION;
    if (SDL_fabsf(circle->vx) < 10) {
      circle->vx = 0;
    }
  }
}

int render_text(char *text, TTF_Font *font, SDL_Renderer *renderer, float x,
                float y) {
  SDL_Color color = {255, 255, 255, 255};
  SDL_Surface *text_surface = TTF_RenderText_Blended(font, text, 0, color);
  return render_and_clear_sdl_surface(renderer, text_surface, x, y);
}

int render_and_clear_sdl_surface(SDL_Renderer *renderer, SDL_Surface *surface,
                                 float x, float y) {
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!texture) {
    return -1;
  }

  // texture space
  SDL_FRect *src_rect = NULL;

  // screen space
  SDL_FRect dst_rect = {
      .x = x, .y = y, .w = (float)surface->w, .h = (float)surface->h};

  SDL_RenderTexture(renderer, texture, src_rect, &dst_rect);
  SDL_DestroyTexture(texture);
  SDL_DestroySurface(surface);

  return 0;
}

int render_and_clear_pluto_surface(SDL_Renderer *renderer,
                                   plutovg_surface_t *pluto_surface) {
  unsigned char *pixels = plutovg_surface_get_data(pluto_surface);
  int stride = plutovg_surface_get_stride(pluto_surface);
  int width = plutovg_surface_get_width(pluto_surface);
  int height = plutovg_surface_get_height(pluto_surface);
  SDL_Surface *surface = SDL_CreateSurfaceFrom(
      width, height, SDL_PIXELFORMAT_ARGB8888, pixels, stride);
  if (surface == NULL) {
    plutovg_surface_destroy(pluto_surface);
    return -1;
  }

  int success = render_and_clear_sdl_surface(renderer, surface, 0.0f, 0.0f);
  plutovg_surface_destroy(pluto_surface);

  return success;
}

plutovg_surface_t *pluto_render(WinData win_data, Circle *circles) {
  // store data in surface
  plutovg_surface_t *surface =
      plutovg_surface_create(win_data.width, win_data.height);
  // draw things in canvas
  plutovg_canvas_t *canvas = plutovg_canvas_create(surface);

  //[TODO] you don't render the background everytime
  plutovg_color_t bg;
  plutovg_color_init_rgb(&bg, 0.5f, 0.8f, 1.0f);
  plutovg_surface_clear(surface, &bg);

  for (int i = 0; i < NUM_CIRCLES; ++i) {
    if (circles[i].r != 0) {
      plutovg_canvas_save(canvas);
      plutovg_canvas_arc(canvas, circles[i].x, circles[i].y, circles[i].r, 0,
                         PLUTOVG_TWO_PI, 0);
      plutovg_canvas_set_rgb(canvas, circles[i].color.r / 255.0f,
                             circles[i].color.g / 255.0f,
                             circles[i].color.b / 255.0f);
      plutovg_canvas_fill(canvas);
      plutovg_canvas_restore(canvas);
    }
  }

  return surface;
}

void update(Circle *circles, WinData win_data, float dt) {
  for (int i = 0; i < NUM_CIRCLES; ++i) {
    for (int j = i + 1; j < NUM_CIRCLES; ++j) {
      if (circles[i].r != 0 && circles[j].r != 0) {
        resolve_ball_ball_collision(&circles[i], &circles[j], dt);
      }
    }
  }
  for (int i = 0; i < NUM_CIRCLES; ++i) {
    if (circles[i].r != 0) {
      resolve_wall_ball_collision(&circles[i], win_data, dt);
    }
  }
}

int main(void) {
  WinData win_data;
  win_data.width = 512;
  win_data.height = 512;

  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *window =
      SDL_CreateWindow("pluto", win_data.width, win_data.height, 0);
  SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);

  TTF_Init();
  TTF_Font *font = TTF_OpenFont("font.ttf", 16.0f);
  if (!font) {
    fprintf(stderr, "Can't open font!\n");
    return 1;
  }

  Circle circles[NUM_CIRCLES] = {0};
  init_circles(circles);

  int running = 1;

  Uint64 prev = SDL_GetTicks();
  float dt = 0;
  float fps_acc = 0;
  char *fps_str = NULL;

  while (running == 1) {

    Uint64 now = SDL_GetTicks();
    dt = (now - prev) / 1000.0f;
    prev = now;
    fps_acc += dt;

    SDL_Event evt;
    while (SDL_PollEvent(&evt)) {
      if (evt.type == SDL_EVENT_QUIT) {
        running = 0;
      }
    }

    update(circles, win_data, dt);

    SDL_RenderClear(renderer);

    plutovg_surface_t *pluto_surface = pluto_render(win_data, circles);
    render_and_clear_pluto_surface(renderer, pluto_surface);

    if (fps_acc > 0.5) {
      float fps = 1 / (dt);
      if (fps_str) {
        free(fps_str);
      }
      asprintf(&fps_str, "FPS: %.4f", fps);
      fps_acc = 0;
    }

    // always render, except for the first -1.5 seconds
    if (fps_str != NULL) {
      render_text(fps_str, font, renderer, 365.0, 5.0);
    }

    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  return 0;
}
