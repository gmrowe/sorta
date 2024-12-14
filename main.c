#include <stdio.h>
#include <raylib.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#define WIDTH 400
#define HEIGHT 400
#define ROWS 12
#define COLS 12
#define CELL_WIDTH WIDTH / COLS
#define CELL_HEIGHT HEIGHT / ROWS
#define CELL_COUNT ROWS * COLS
#define FPS 60
#define ACTIVE_COLOR MAGENTA
#define SWAP_COLOR DARKGREEN

void shuffle_fy(int *ns, int ns_size) {
  for (int i = ns_size - 1; i > 0; --i) {
    int rnd_idx = rand() % i;
    int temp = ns[rnd_idx];
    ns[rnd_idx] = ns[i];
    ns[i] = temp;
  }
}

void swap(int *xs, int i, int j) {
  int temp = xs[i];
  xs[i] =  xs[j];
  xs[j] = temp;
}

typedef struct CellMatrix {
  Texture texs[CELL_COUNT];
  int rows;
  int cols;
  int width;
  int height;
} CellMatrix;

CellMatrix create_cell_mat(Image src, int rows, int cols) {
  int cell_width = (int) (src.width / cols);
  int cell_height = (int) (src.height/ rows);
  int cell_count = rows * cols;
  CellMatrix mat = {0};
  for (int i = 0; i < cell_count; ++i) {
    int row = i / cols;
    int col = i % cols;
    Rectangle cell_bounds = {
      .x = (float) col * cell_width,
      .y = (float) row * cell_height,
      .width = (float) cell_width,
      .height = (float) cell_height,
    };
    mat.texs[i] = LoadTextureFromImage(ImageFromImage(src, cell_bounds));
  }
  mat.rows = rows;
  mat.cols = cols;
  mat.width = cell_width;
  mat.height = cell_height;
  return mat;
}

void free_cell_mat(CellMatrix *mat) {
  free(mat->texs);
  free(mat);
}

void dump_array(int *ns, int ns_size) {
  printf("dump_array:\n");
  for (int i = 0; i < ns_size; ++i) {
    printf("  [%d] %d\n", i, ns[i]);
  }
}

typedef struct BubbleSortState {
  int ns[CELL_COUNT];
  int count;
  int is_sorted;
  int active_index;
  int updated;
  Color highlights[CELL_COUNT];
  int full_iterations;
} BubbleSortState;

void reset_colors(Color *colors, int color_count, Color color) {
  for (int i = 0; i < color_count; i++) {
    colors[i] = color;
  }
}

BubbleSortState create_bubble_sort_state(void) {
  BubbleSortState step = {0};
  for (int i = 0; i < CELL_COUNT; ++i) step.ns[i] = i;
  shuffle_fy(step.ns, CELL_COUNT);
  reset_colors(step.highlights, CELL_COUNT, WHITE);
  step.count = CELL_COUNT;
  step.is_sorted = false;
  step.active_index = 0;
  step.updated = false;
  step.full_iterations = 0;
  return step;
}

void bubble_sort_step(BubbleSortState *step) {
  reset_colors(step->highlights, step->count, WHITE);
  if (step->is_sorted) {
    return;
  }
  int idx = step->active_index;
  step->highlights[idx] = ACTIVE_COLOR;

  // End of array behavior
  if (idx == (step->count - step->full_iterations - 1)) {
    if (step->updated) {
      step->active_index = 0;
      step->updated = false;
      step->full_iterations += 1;
    } else {
      step->is_sorted = true;
    }
    return;
  }

  // Check whether current element is out of order
  if (step->ns[idx] > step->ns[idx + 1]) {
    swap(step->ns, idx, idx + 1);
    step->updated = true;
    step->highlights[idx] = SWAP_COLOR;
    step->highlights[idx + 1] = SWAP_COLOR;
  }
  step->active_index = idx + 1;
}

void render_bubble_sort_state(CellMatrix *mat, BubbleSortState *step) {
  for (int i = 0; i < CELL_COUNT; ++i) {
    Texture t = mat->texs[step->ns[i]];
    int row = i / mat->rows;
    int col = i % mat->cols;
    DrawTexture(t, col * mat->width, row * mat->height, step->highlights[i]);
  }
}

typedef struct {
  int xs[CELL_COUNT];
  int i;
  int j;
  int count;
  bool done;
  Color highlights[CELL_COUNT];
} InsertionSortState;

InsertionSortState create_insertion_sort_state() {
  InsertionSortState state;
  state.i = 1;
  state.j = 1;
  state.count = CELL_COUNT;
  state.done = false;
  for (int i = 0; i < CELL_COUNT; i++) state.xs[i] = i;
  shuffle_fy(state.xs, CELL_COUNT);
  reset_colors(state.highlights, CELL_COUNT, WHITE);
  return state;
}

void insertion_sort_step(InsertionSortState* state) {
  reset_colors(state->highlights, state->count, WHITE);
  if (state->done) {
    return;
  }

  if (state->i >= state->count) {
    state->done = true;
    return;
  }

  int j = state->j;
  if (j > 0 && state->xs[j - 1] > state->xs[j]) {
    swap(state->xs, j, j - 1);
    state->j -= 1;
    state->highlights[j] = SWAP_COLOR;
    state->highlights[j - 1] = SWAP_COLOR;
  } else {
    state->i += 1;
    state->j = state->i;
  }
}

void render_insertion_sort_state(CellMatrix *mat, const InsertionSortState *step) {
  for (int i = 0; i < CELL_COUNT; ++i) {
    Texture t = mat->texs[step->xs[i]];
    int row = i / mat->rows;
    int col = i % mat->cols;
    DrawTexture(t, col * mat->width, row * mat->height, step->highlights[i]);
  }
}

int main(void) {
  InitWindow(WIDTH, HEIGHT, "Insertion Sort!");
  Image src_img = GenImageGradientLinear(WIDTH, HEIGHT, 5, DARKPURPLE, ORANGE);
  CellMatrix mat = create_cell_mat(src_img, ROWS, COLS);
  InsertionSortState state = create_insertion_sort_state();
  SetTargetFPS(FPS);
  SetTraceLogLevel(LOG_WARNING);
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(WHITE);
    render_insertion_sort_state(&mat, &state);
    insertion_sort_step(&state);
    EndDrawing();
  }
  CloseWindow();
  return EXIT_SUCCESS;
}

int main2(void) {
  InitWindow(WIDTH, HEIGHT, "Bubble Sort!");
  Image src_img = GenImageGradientLinear(WIDTH, HEIGHT, 5, DARKPURPLE, ORANGE);
  CellMatrix mat = create_cell_mat(src_img, ROWS, COLS);
  SetTargetFPS(FPS);
  SetTraceLogLevel(LOG_WARNING);
  BubbleSortState state = create_bubble_sort_state();
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(WHITE);
    render_bubble_sort_state(&mat, &state);
    bubble_sort_step(&state);
    EndDrawing();
  }
  CloseWindow();
  return EXIT_SUCCESS;
}
