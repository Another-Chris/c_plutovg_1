
#include "main.h"


int render_text(
    char* text, TTF_Font* font, SDL_Renderer* renderer, float x, float y) 
{
  SDL_Color color = {255,255,255,255};
  SDL_Surface *text_surface = TTF_RenderText_Blended(font, text, 0, color);
  return render_and_clear_sdl_surface(renderer, text_surface, x, y);
}




int render_and_clear_sdl_surface(
    SDL_Renderer* renderer, SDL_Surface* surface, float x, float y)
{
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!texture) {
    return -1;
  }

  // texture space
  SDL_FRect* src_rect = NULL;

  // screen space
  SDL_FRect dst_rect = {
    .x = x,
    .y = y,
    .w = (float) surface->w,
    .h = (float) surface->h
  };

  SDL_RenderTexture(renderer, texture, src_rect, &dst_rect);
  SDL_DestroyTexture(texture);
  SDL_DestroySurface(surface);

  return 0;
}



int render_and_clear_pluto_surface(
    SDL_Renderer* renderer, plutovg_surface_t* pluto_surface) {
   unsigned char* pixels = plutovg_surface_get_data(pluto_surface);                                
   int stride = plutovg_surface_get_stride(pluto_surface);
   int width = plutovg_surface_get_width(pluto_surface);                          
   int height = plutovg_surface_get_height(pluto_surface);  
   SDL_Surface* surface = SDL_CreateSurfaceFrom(
       width, height,
       SDL_PIXELFORMAT_ARGB8888,
       pixels,
       stride
   );
   if (surface == NULL) {
     plutovg_surface_destroy(pluto_surface);                                 
     return -1;
   }

   int success = render_and_clear_sdl_surface(renderer, surface, 0.0f, 0.0f);
   plutovg_surface_destroy(pluto_surface);                                 

   return success;

}

plutovg_surface_t* pluto_render(WinData win_data, Circle circle) {
  // store data in surface
  plutovg_surface_t *surface =
      plutovg_surface_create(win_data.width, win_data.height);
  // draw things in canvas
  plutovg_canvas_t *canvas = plutovg_canvas_create(surface);


  plutovg_color_t bg;
  plutovg_color_init_rgb(&bg, 0.5f, 0.8f, 1.0f);
  plutovg_surface_clear(surface, &bg);

  plutovg_canvas_save(canvas);
  plutovg_canvas_arc(canvas, circle.center_x, circle.center_y, circle.radius, 0,
                     PLUTOVG_TWO_PI, 0);
  plutovg_canvas_set_rgb(canvas, 1, 0, 1);
  plutovg_canvas_fill_preserve(canvas);
  plutovg_canvas_restore(canvas);

  return surface;
}

void update(Circle* circle, WinData win_data, float dt) {
  circle->vy += (GRAVITY * dt);

  if (circle->center_y + circle->radius == win_data.height) {
    if (circle->vx > 0) {
      circle->vx -= FRICTION * dt;
    } else if (circle -> vx < 0) {
      circle->vx += FRICTION * dt;
    }
  }

  circle->center_y += (circle->vy * dt);
  if (circle->center_y + circle->radius >= win_data.height) {
    circle->center_y = win_data.height - circle->radius;
    circle->vy = -circle->vy * RESTITUTION;
    if (SDL_fabsf(circle->vy) < 10) {
      circle->vy = 0;
      circle->center_y = win_data.height - circle->radius;
    }
  }

  circle->center_x += (circle->vx * dt);
  if (circle->center_x + circle->radius >= win_data.width) {
    circle->center_x = win_data.width - circle->radius;
    circle->vx = -circle->vx * RESTITUTION;
    if (SDL_fabsf(circle->vx) < 10) {
      circle->vx = 0;
    }
  }
  else if (circle->center_x - circle->radius <= 0) {
    circle->center_x = circle->radius;
    circle->vx = -circle->vx * RESTITUTION;
    if (SDL_fabsf(circle->vx) < 10) {
      circle->vx = 0;
    }
  }






}


int main(void) {
  WinData win_data;
  win_data.width = 512;
  win_data.height = 512;

  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window* window = SDL_CreateWindow("pluto", win_data.width, win_data.height, 0);
  SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

  TTF_Init();
  TTF_Font *font = TTF_OpenFont("font.ttf", 16.0f);
  if (!font) {
    fprintf(stderr, "Can't open font!\n");
    return 1;
  }


  Circle circle;
  circle.center_x = 10.0f;
  circle.center_y = 10.0f;
  circle.radius = 10.0f;
  circle.vx = 600.0f;
  circle.vy = 0.0f;

  int running = 1;

  Uint64 prev = SDL_GetTicks();
  float dt = 0;
  float fps_acc = 0;
  char* fps_str = NULL;

  while(running == 1) {

    Uint64 now = SDL_GetTicks();
    dt = (now - prev) / 1000.0f;
    prev = now;
    fps_acc += dt;

    SDL_Event evt;
    while(SDL_PollEvent(&evt)) {
      if (evt.type == SDL_EVENT_QUIT) {
        running = 0;
      }
    }


    update(&circle, win_data, dt);

    SDL_RenderClear(renderer);
    render_and_clear_pluto_surface(renderer, pluto_render(win_data, circle));

    if (fps_acc > 0.5) {
      float fps = 1 / (dt);
      if (fps_str) {
        free(fps_str);
      }
      asprintf(&fps_str, "FPS: %.4f", fps);
      fps_acc = 0;
    }

    // always render, except for the first 0.5 seconds
    if (fps_str != NULL) {
      render_text(fps_str, font, renderer, 365.0, 5.0);
    }


    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  return 0;
}
