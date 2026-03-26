
#include "main.h"



int render_and_clear_sdl_surface(SDL_Renderer* renderer, SDL_Surface* surface) {
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!texture) {
    return -1;
  }

  // texture space
  SDL_FRect* src_rect = NULL;

  // screen space
  SDL_FRect dst_rect = {
    .x = 0.0f,
    .y = 0.0f,
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

   int success = render_and_clear_sdl_surface(renderer, surface);
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


int main(void) {
  WinData win_data;
  win_data.width = 512;
  win_data.height = 512;

  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window* window = SDL_CreateWindow("pluto", win_data.width, win_data.height, 0);
  SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

  Circle circle;
  circle.center_x = 10.0f;
  circle.center_y = 10.0f;
  circle.radius = 10.0f;

  int running = 1;

  Uint64 prev = SDL_GetTicks();
  float dt = 0;

  while(running == 1) {

    Uint64 now = SDL_GetTicks();
    dt = (now - prev) / 1000.0f;
    prev = now;

    float fps = 1 / (dt);
    printf("%f\n", fps);





    SDL_Event evt;
    while(SDL_PollEvent(&evt)) {
      if (evt.type == SDL_EVENT_QUIT) {
        running = 0;
      }
    }

    circle.center_y += 0.1;
    SDL_RenderClear(renderer);
    render_and_clear_pluto_surface(renderer, pluto_render(win_data, circle));

    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  return 0;
}
