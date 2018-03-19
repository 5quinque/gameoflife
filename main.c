#include <ncurses.h>
#include <string.h> 

#include <time.h>
#include <stdlib.h>

#define ROWS 20
#define COLS 40

void newgame();
void update();
void printgame();
void addrandomlife(int s);
int countneighbour(int row, int col);

int row_start, col_start;
char board[ROWS][COLS];

/*
 * Any live cell with fewer than two live neighbours dies, as if caused by underpopulation.
 * Any live cell with two or three live neighbours lives on to the next generation.
 * Any live cell with more than three live neighbours dies, as if by overpopulation.
 * Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
 *
 */
int main() {
  int screen_rows, screen_cols;
  char mesg[]="CENTER";
  char empty = '|';
  int ticks = 0;
  srand(time(NULL));

  struct timespec ts = {0, 100000000L };

  initscr();
  getmaxyx(stdscr, screen_rows, screen_cols);

  row_start = (screen_rows / 2 - ROWS / 2);
  col_start = (screen_cols / 2 - COLS / 2);

  newgame();
  addrandomlife(100);

  for (int y = 0; y < ROWS; y++) {
    for (int x = 0; x < COLS; x++) {
      mvaddch(row_start + y, col_start + x, empty);
    }
  }
  mvprintw(screen_rows / 2, (screen_cols - strlen(mesg))/2, "%s", mesg);

  while (1) {
    ticks++;
    mvprintw(screen_rows - 1, 2, "Ticks: %d", ticks);

    update();
    printgame();

    refresh();
    nanosleep (&ts, NULL);
  }
  endwin();

  int count = countneighbour(4, 4);
  printf("count: %d\n", count);

  return 0;
}

void update() {
  int count;

  for (int y = 0; y < ROWS; y++) {
    for (int x = 0; x < COLS; x++) {
      count = countneighbour(y, x);
      //printf("count: %d\n", count);

      if (board[y][x] == 1) {
        if (count < 2) {
          board[y][x] = 0;
        } else if(count == 2 || count == 3) {
          board[y][x] = 1;
        } else if (count > 3) {
          board[y][x] = 0;
        }
      } else {
        if (count == 3) {
          board[y][x] = 1;
        }
      }

    }
  }
}

void printgame() {
  char c;
  for (int y = 0; y < ROWS; y++) {
    for (int x = 0; x < COLS; x++) {
      switch (board[y][x]) {
        case 0:
          c = ' ';
          break;
        case 1:
          c = 'x';
          break;
      }
      mvaddch(y + row_start, x + col_start, c);
    }
  }
}

void newgame() {
  for (int y = 0; y < ROWS; y++)
    for (int x = 0; x < COLS; x++)
      board[y][x] = 0;
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

void addrandomlife(int sperm_count) {
  int r, c;
  for (int i = 0; i < sperm_count; i++) {
    r = rand() % ROWS;
    c = rand() % COLS;

    board[r][c] = 1;
  }
}
