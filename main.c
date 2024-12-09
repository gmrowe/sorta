#include <stdio.h>
#include <raylib.h>
#include <stdint.h>
#include <stdlib.h>

#define WIDTH 400
#define HEIGHT 400
#define ROWS 40
#define COLS 40
#define RADIUS 5
#define FPS 30

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

typedef struct Cell {
  Image img;
} Cell;

typedef struct CellMat {
  Cell *cells;
  int count;
  int *ordering;
  int rows;
  int cols;
} CellMat;

void render_cell_mat(CellMat *mat) {
  for (int i = 0; i < mat->count; ++i) {
    Cell cell = mat->cells[mat->ordering[i]];
    Image img = cell.img;
    int row = i / mat->rows;
    int col = i % mat->cols;
    Texture2D t = LoadTextureFromImage(cell.img);
    DrawTexture(t, col * img.width, row * img.height, WHITE);
  }
}

CellMat* create_cell_mat(Image src, int rows, int cols) {
  int cell_width = (int) (src.width / cols);
  int cell_height = (int) (src.height/ rows);
  int cell_count = rows * cols;
  Cell *cells = (Cell *)malloc(cell_count * sizeof(Cell));
  for (int row = 0; row < rows; ++row) {
    for (int col = 0; col < cols; ++col) {
      Rectangle cell_bounds = {
	(float) col * cell_width,
	(float)row * cell_height,
	(float) cell_width,
	(float) cell_height,
      };
      cells[(row * COLS) + col] = (Cell) {
	.img = ImageFromImage(src, cell_bounds),
      };
    }
  }

  int *order = malloc(cell_count * sizeof(int));
  for (int i = 0; i < cell_count; ++i) order[i] = i;
  CellMat *mat = malloc(sizeof(CellMat));
  mat->cells = cells;
  mat->count = cell_count;
  mat->ordering = order;
  mat->rows = rows;
  mat->cols = cols;
  return mat;
}

void free_cell_mat(CellMat *mat) {
  free(mat->cells);
  free(mat->ordering);
  free(mat);
}

void dump_array(int *ns, int ns_size) {
  printf("dump_array:\n");
  for (int i = 0; i < ns_size; ++i) {
    printf("  [%d] %d\n", i, ns[i]);
  }
}

int main(void) {
  Image src_img = (color_gradient_image(DARKBLUE, RED));
  CellMat *mat = create_cell_mat(src_img, ROWS, COLS);
  InitWindow(WIDTH, HEIGHT, "Hello Raylib!");
  SetTargetFPS(FPS);
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(WHITE);
    render_cell_mat(mat);
    EndDrawing();
  }
  free_cell_mat(mat);
  CloseWindow();
  return 0;
}
