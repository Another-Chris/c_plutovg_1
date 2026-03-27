cc main.c -o0 -g -Wall \
  $(pkg-config --cflags --libs plutovg sdl3 sdl3-ttf) \
  -o main
