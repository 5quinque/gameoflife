#include <unistd.h>
#include <ncurses.h>
#include <string.h> 

#include <time.h>
#include <stdlib.h>
#include <limits.h>

#define ROWS 20
#define COLS 20

void addrandomlife(int s);

int row_start, col_start;
char board[ROWS][COLS];

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

  for (int y = 0; y < ROWS; y++) {
    for (int x = 0; x < COLS; x++) {
      mvaddch(row_start + y, col_start + x, empty);
    }
  }

  mvprintw(screen_rows / 2, (screen_cols - strlen(mesg))/2, "%s", mesg);

  addrandomlife(100);
  while (1) {
    ticks++;
    mvprintw(screen_rows - 1, 2, "Ticks: %d", ticks);

    refresh();
    nanosleep (&ts, NULL);
  }

  endwin();

  return 0;
}

void addrandomlife(int sperm_count) {
  int r, c;
  for (int i = 0; i < sperm_count; i++) {
    r = rand() % ROWS;
    c = rand() % COLS;

    mvaddch(r + row_start, c + col_start, '1');
    board[r][c] = 1;
  }
}
