#include <ncurses.h>
// memcpy()
#include <string.h> 
// time(), nanosleep()
#include <time.h>
// srand(), rand()
#include <stdlib.h>

#define ROWS 30
#define COLS 60

void update();
void printgame();
void togglelife(int row, int col);
void addrandomlife(int s);
void handleinput(int c);
int countneighbour(int row, int col);

int row_start, col_start;
char board[ROWS][COLS];
char tempboard[ROWS][COLS];
int tps = 10;
int pause = 0;
int running = 1;
int screen_rows, screen_cols;
MEVENT event;

int main() {
  int ticks = 0;
  struct timespec ts = {0, 10000000L}; //ts.tv_sec = 0; ts.tv_nsec = 10000000L;
  int c;

  srand(time(NULL));

  initscr();
  cbreak();
  noecho();
  start_color();

  // Life
  init_pair(2, COLOR_WHITE, COLOR_WHITE);
  // Death
  init_pair(3, COLOR_GREEN, COLOR_GREEN);

  nodelay(stdscr, TRUE);
  keypad(stdscr, TRUE);
  mousemask(ALL_MOUSE_EVENTS, NULL);

  getmaxyx(stdscr, screen_rows, screen_cols);
  row_start = (screen_rows / 2 - ROWS / 2);
  col_start = (screen_cols / 2 - COLS / 2);

  // Plant the seed of life
  addrandomlife(200);

  while (running) {
    ticks++;

    c = getch();
    if (~c >> 31)
      handleinput(c);
      
    if (ticks % tps == 0 && !pause)
      update();

    printgame();
    mvprintw(screen_rows - 1, 0, "Update Interval: %d\t", tps);
    move (0, 0);

    refresh();
    nanosleep (&ts, NULL);
  }
  endwin();

  return 0;
}

void handleinput(int c) {
  switch (c) {
    case KEY_MOUSE:
      getmouse(&event);
      togglelife(event.y, event.x);
      break;
    case KEY_UP:
      tps = tps < 20 ? tps - 1 : tps - 10;
      tps = tps < 1 ? 1 : tps;
      break;
    case KEY_DOWN:
      tps = tps < 10 ? tps + 1 : tps + 10;
      break;
    case ' ':
      pause ^= 1;
      move (0, 0);
      clrtoeol();
      if (pause)
        mvprintw(0, 1, "PAUSED");
      break;
    case 'q':
      running = 0;
      break;
    }
}

void update() {
  int count;
  for (int y = 0; y < ROWS; y++) {
    for (int x = 0; x < COLS; x++) {
      count = countneighbour(y, x);
      if (board[y][x] == 1) {
        tempboard[y][x] = (count < 2 || count > 3) ? 0 : 1;
      } else if (count == 3) {
        tempboard[y][x] = 1;
      }
    }
  }
  memcpy(board, tempboard, sizeof(tempboard));
}

void printgame() {
  int c;
  for (int y = 0; y < ROWS; y++) {
    for (int x = 0; x < COLS; x++) {
      c = board[y][x] ^ 3;
      attron(COLOR_PAIR(c));
      mvaddch(y + row_start, x + col_start, '.');
      attroff(COLOR_PAIR(c));
    }
  }
}

int countneighbour(int row, int col) {
  int count = 0;
  for (int y = row - 1; y < row + 2; y++) {
    for (int x = col - 1; x < col + 2; x++) {
      if (y == row && x == col) continue;
      if (board[y][x] == 1) count++;
    }
  }
  return count;
}

void togglelife(int row, int col) {
  int brow = row - row_start;
  int bcol = col - col_start;
  if ( ~(brow | bcol) >> 31 ) 
    board[brow][bcol] ^= 1;
}

void addrandomlife(int sperm_count) {
  int r, c;
  for (int i = 0; i < sperm_count; i++) {
    r = rand() % ROWS;
    c = rand() % COLS;
    board[r][c] = 1;
  }
}
