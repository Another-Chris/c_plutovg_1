cc main.c -Wall \
  $(pkg-config --cflags --libs plutovg sdl3) \
  -o main
