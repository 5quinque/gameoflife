#include <ncurses.h>
#include <string.h> 

#include <time.h>
#include <stdlib.h>

#define ROWS 20
#define COLS 40

void update();
void printgame();
void addlife(int row, int col);
void addrandomlife(int s);
void handlekeypress(int c);
void printpause();
int countneighbour(int row, int col);

int row_start, col_start;
char board[ROWS][COLS];
char tempboard[ROWS][COLS];
int tps = 10;
int pause = 0;
int screen_rows, screen_cols;

int main() {
  int ticks = 0;
  struct timespec ts = {0, 10000000L}; //ts.tv_sec = 0; ts.tv_nsec = 10000000L;

  srand(time(NULL));
  int c;

  MEVENT event;

  initscr();
  cbreak();
  noecho();
  start_color();

  // Life
  init_pair(2, COLOR_WHITE, COLOR_WHITE);
  // Death
  init_pair(3, COLOR_WHITE, COLOR_BLACK);

  // Pause
  init_pair(4, COLOR_CYAN, COLOR_MAGENTA);
  init_pair(8, COLOR_CYAN, COLOR_YELLOW);

  timeout(0);
  keypad(stdscr, TRUE);
  mousemask(ALL_MOUSE_EVENTS, NULL);

  getmaxyx(stdscr, screen_rows, screen_cols);

  row_start = (screen_rows / 2 - ROWS / 2);
  col_start = (screen_cols / 2 - COLS / 2);

  addrandomlife(200);

  while (1) {
    c = getch();
    if(c == KEY_MOUSE && getmouse(&event) == OK) {
      addlife(event.y, event.x);
    } else if (c == KEY_UP || c == KEY_DOWN || c == ' ') {
      handlekeypress(c);
    }
      
    ticks++;
    
    if (ticks % tps == 0 && !pause) {
      update();
    }
    printgame();
    mvprintw(screen_rows - 1, 2, "Update Interval: %d\t", tps);

    refresh();
    nanosleep (&ts, NULL);
  }
  endwin();

  return 0;
}

void handlekeypress(int c) {
  switch (c) {
    case KEY_UP:
      tps = tps < 20 ? tps - 1 : tps - 10;
      tps = tps < 1 ? 1 : tps;
      break;
    case KEY_DOWN:
      tps = tps < 10 ? tps + 1 : tps + 10;
      break;
    case ' ':
      pause = pause ? 0 : 1;
      if (pause) {
        printpause();
      } else {
        move (0, 0);
        clrtoeol();
      }
      break;
    }
}

void printpause() {
  int c;
  for (int x = 0; x < (screen_cols / 6) + 1; x++) {
    c = 4 << (1 & x);
    attron(COLOR_PAIR(c));
    mvprintw(0, x * 6, "PAUSED");
    attroff(COLOR_PAIR(c));
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
      mvaddch(y + row_start, x + col_start, ' ');
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

void addlife(int row, int col) {
  board[row - row_start][col - col_start] = 1;
}

void addrandomlife(int sperm_count) {
  int r, c;
  for (int i = 0; i < sperm_count; i++) {
    r = rand() % ROWS;
    c = rand() % COLS;
    board[r][c] = 1;
  }
}
