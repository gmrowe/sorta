#include <stdio.h>
#include <raylib.h>
#include <stdint.h>

#define WIDTH 400
#define HEIGHT 400
const int RADIUS = 5;
const int FPS = 30;

void color_gradient(Color a, Color b) {
  for (int y = 0; y < HEIGHT; ++y) {
    for (int x = 0; x < WIDTH; ++x) {
      Color color;
      float t = (float) y / HEIGHT;
      color.r = (a.r * t) + (b.r * (1.0 - t));
      color.g = (a.g * t) + (b.g * (1.0 - t));
      color.b = (a.b * t) + (b.b * (1.0 - t));
      DrawPixel(x, y, color);
    }
  }
}

Image color_gradient_image(Color a, Color b) {
  uint32_t init[WIDTH * HEIGHT] = {0};
  Image image = {
    .data = init,
    .width = WIDTH,
    .height = HEIGHT,
    .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8,
    .mipmaps = 1
  };

  for (int y = 0; y < HEIGHT; ++y) {
    for (int x = 0; x < WIDTH; ++x) {
      Color color;
      float t = (float) y / HEIGHT;
      color.r = (a.r * (1.0 - t)) + (b.r * t);
      color.g = (a.g * (1.0 - t)) + (b.g * t);
      color.b = (a.b * (1.0 - t)) + (b.b * t);
      ImageDrawPixel(&image,x, y, color);
    }
  }
  return image;
}

int main(void) {
  InitWindow(WIDTH, HEIGHT, "Hello Raylib!");
  SetTargetFPS(FPS);
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(WHITE);
    Texture2D texture = LoadTextureFromImage(color_gradient_image(DARKBLUE, RED));
    DrawTexture(texture, 0, 0, WHITE);
    EndDrawing();
  }
  CloseWindow();
  return 0;
}
