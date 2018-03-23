#include <ncurses.h>
// memcpy(), memset()
#include <string.h>
// time(), nanosleep()
#include <time.h>
// srand(), rand()
#include <stdlib.h>

void update();
void printgame();
void togglelife(int row, int col);
void addrandomlife(int s);
void handleinput(int c);
void purgelife();
int countneighbour(int row, int col);

int **board;
int **tempboard;
int pause = 0;
int running = 1;
int screen_rows, screen_cols, game_columns;
MEVENT event;

int main() {
  struct timespec ts = {0, 100000000L};
  int c;

  srand(time(NULL));

  initscr();

  // Hide the cursor
  curs_set(0);

  // Don't show keypress on screen
  cbreak();
  noecho();

  // Enable colours
  start_color();
  // Life
  init_pair(2, COLOR_GREEN, COLOR_GREEN);
  // Death
  init_pair(3, COLOR_BLACK, COLOR_BLACK);

  // Stop getch() from blocking
  nodelay(stdscr, TRUE);
  // Need this for mouse input ?
  keypad(stdscr, TRUE);

  mousemask(ALL_MOUSE_EVENTS, NULL);

  getmaxyx(stdscr, screen_rows, screen_cols);
  game_columns = screen_cols / 2;

  board = malloc(screen_rows * sizeof(int *));
  tempboard = malloc(screen_rows * sizeof(int *));

  for (int i = 0; i < screen_rows; i++) {
    board[i] = malloc(game_columns * sizeof(int));
    tempboard[i] = malloc(game_columns * sizeof(int));
  }

  // Plant the seed of life
  addrandomlife((screen_rows * game_columns) / 5);

  while (running) {
    c = getch();
    if (~c >> 31) handleinput(c);

    if (!pause) update();

    printgame();
    refresh();
    nanosleep(&ts, NULL);
  }

  // Clean up after ourselves
  endwin();

  return 0;
}

void handleinput(int c) {
  switch (c) {
    // All mouse events, left/right/scroll
    case KEY_MOUSE:
      getmouse(&event);
      togglelife(event.y, event.x);
      break;
    case ' ':
      // toggle pause
      pause ^= 1;
      break;
    case 'q':
      running = 0;
      break;
    case 'c':
      purgelife();
      break;
  }
}

void update() {
  int count;
  for (int y = 0; y < screen_rows; y++) {
    for (int x = 0; x < game_columns; x++) {
      count = countneighbour(y, x);
      if (board[y][x] == 1) {
        tempboard[y][x] = (count < 2 || count > 3) ? 0 : 1;
      } else if (count == 3) {
        tempboard[y][x] = 1;
      }
    }
  }

  for (int i = 0; i < screen_rows; i++) {
    memcpy(board[i], tempboard[i], game_columns * sizeof(int));
  }
}

void printgame() {
  int c;
  for (int y = 0; y < screen_rows; y++) {
    for (int x = 0; x < game_columns; x++) {
      c = board[y][x] ^ 3;
      attron(COLOR_PAIR(c));
      mvaddch(y, x * 2, '.');
      mvaddch(y, x * 2 + 1, '.');
      attroff(COLOR_PAIR(c));
    }
  }
  if (pause) mvprintw(1, screen_cols / 2 - 3, "Paused");
}

int countneighbour(int row, int col) {
  int count = 0;
  for (int y = row - 1; y < row + 2; y++) {
    for (int x = col - 1; x < col + 2; x++) {
      if ((y | x) >> 31 || y > screen_rows - 1 || x > game_columns - 1 ||
          (y == row && x == col))
        continue;
      if (board[y][x] == 1) count++;
    }
  }
  return count;
}

void togglelife(int row, int col) {
  int brow = row;
  int bcol = col / 2;

  if (~(brow | bcol) >> 31) {
    board[brow][bcol] ^= 1;
    tempboard[brow][bcol] ^= 1;
  }
}

void purgelife() {
  for (int i = 0; i < screen_rows; i++) {
    memset(board[i], 0, game_columns * sizeof(int));
    memset(tempboard[i], 0, game_columns * sizeof(int));
  }
}

void addrandomlife(int sperm_count) {
  int r, c;
  for (int i = 0; i < sperm_count; i++) {
    r = rand() % screen_rows;
    c = rand() % game_columns;
    board[r][c] = 1;
  }
}
