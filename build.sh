cc main.c -Wall \
  $(pkg-config --cflags --libs plutovg sdl3 sdl3-ttf) \
  -o main
