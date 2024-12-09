#include <stdio.h>
#include <raylib.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#define WIDTH 400
#define HEIGHT 400
#define ROWS 10
#define COLS 10
#define RADIUS 5
#define FPS 60
#define ACTIVE_COLOR MAGENTA
#define SWAP_COLOR DARKGREEN

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

void shuffle_fy(int *ns, int ns_size) {
  for (int i = ns_size - 1; i > 0; --i) {
    int rnd_idx = rand() % i;
    int temp = ns[rnd_idx];
    ns[rnd_idx] = ns[i];
    ns[i] = temp;
  }
}

typedef struct CellMat {
  Image *imgs;
  int count;
  int rows;
  int cols;
} CellMat;

CellMat* create_cell_mat(Image src, int rows, int cols) {
  int cell_width = (int) (src.width / cols);
  int cell_height = (int) (src.height/ rows);
  int cell_count = rows * cols;
  Image *cells = malloc(cell_count * sizeof(Image));
  for (int row = 0; row < rows; ++row) {
    for (int col = 0; col < cols; ++col) {
      Rectangle cell_bounds = {
	(float) col * cell_width,
	(float) row * cell_height,
	(float) cell_width,
	(float) cell_height,
      };
      cells[(row * COLS) + col] = ImageFromImage(src, cell_bounds);
    }
  }

  CellMat *mat = malloc(sizeof(CellMat));
  mat->imgs = cells;
  mat->count = cell_count;
  mat->rows = rows;
  mat->cols = cols;
  return mat;
}

void free_cell_mat(CellMat *mat) {
  free(mat->imgs);
  free(mat);
}

void dump_array(int *ns, int ns_size) {
  printf("dump_array:\n");
  for (int i = 0; i < ns_size; ++i) {
    printf("  [%d] %d\n", i, ns[i]);
  }
}

typedef struct SortStep {
  int *ns;
  int count;
  int is_sorted;
  int active_index;
  int updated;
  Color *highlights;
} SortStep;

void reset_colors(Color *colors, int color_count, Color color) {
  for (int i = 0; i < color_count; i++) {
    colors[i] = color;
  }
}

SortStep* create_sort_step(int count) {
  int *order = malloc(count * sizeof(int));
  for (int i = 0; i < count; ++i) order[i] = i;
  shuffle_fy(order, count);
  Color *highlights = malloc(count * sizeof(Color));
  reset_colors(highlights, count, WHITE);
  SortStep *step = malloc(sizeof(SortStep));
  step->ns = order;
  step->count = count;
  step->is_sorted = false;
  step->active_index = 0;
  step->updated = false;
  step->highlights = highlights;
  return step;
}

void free_sort_step(SortStep *step) {
  free(step->highlights);
  free(step->ns);
  free(step);
}

void bubble_sort_step(SortStep *step) {
  reset_colors(step->highlights, step->count, WHITE);
  if (step->is_sorted) {
    return;
  }
  int idx = step->active_index;
  step->highlights[idx] = ACTIVE_COLOR;

  // End of array behavior
  if (idx == step->count - 1) {
    if (step->updated) {
      step->active_index = 0;
      step->updated = false;
    } else {
      step->is_sorted = true;
    }
    return;
  }

  // Check wheter current element is out of order
  if (step->ns[idx] > step->ns[idx + 1]) {
    int temp = step->ns[idx];
    step->ns[idx] = step->ns[idx + 1];
    step->ns[idx + 1] = temp;
    step->updated = true;
    step->highlights[idx] = SWAP_COLOR;
    step->highlights[idx + 1] = SWAP_COLOR;
  }
  step->active_index = idx + 1;
}

void render_cell_mat(CellMat *mat, SortStep *step) {
  for (int i = 0; i < mat->count; ++i) {
    Image img = mat->imgs[step->ns[i]];
    int row = i / mat->rows;
    int col = i % mat->cols;
    Texture2D t = LoadTextureFromImage(img);
    DrawTexture(t, col * img.width, row * img.height, step->highlights[i]);
  }
}

int main(void) {
  Image src_img = (color_gradient_image(DARKBLUE, RED));
  CellMat *mat = create_cell_mat(src_img, ROWS, COLS);
  SortStep *step = create_sort_step(ROWS * COLS);
  InitWindow(WIDTH, HEIGHT, "Hello Raylib!");
  SetTargetFPS(FPS);
  SetTraceLogLevel(LOG_ALL);
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(WHITE);
    render_cell_mat(mat, step);
    bubble_sort_step(step);
    EndDrawing();
  }
  free_cell_mat(mat);
  free_sort_step(step);
  CloseWindow();
  return 0;
}
