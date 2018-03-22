#include <ncurses.h>
// memcpy()
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
int countneighbour(int row, int col);

int **board;
int **tempboard;
int boardsize;
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
  init_pair(2, COLOR_WHITE, COLOR_WHITE);
  // Death
  init_pair(3, COLOR_GREEN, COLOR_GREEN);

  // Stop getch() from blocking
  nodelay(stdscr, TRUE);
  // Need this for mouse input ?
  keypad(stdscr, TRUE);

  mousemask(ALL_MOUSE_EVENTS, NULL);

  getmaxyx(stdscr, screen_rows, screen_cols);

  game_columns = screen_cols / 2;

  boardsize = screen_rows * sizeof(int*);
  board = malloc(screen_rows * sizeof(int*));
  tempboard = malloc(screen_rows * sizeof(int*));

  for (int i = 0; i < screen_rows; i++) {
    boardsize += game_columns * sizeof(int);
    board[i] = malloc(game_columns * sizeof(int));
    tempboard[i] = malloc(game_columns * sizeof(int));

      for (int x = 0; x < game_columns; x++) {
        board[i][x] = 0;
      }
      memcpy(tempboard[i], board[i], game_columns * sizeof(int));
  }

  mvprintw(0,0, "%zu", boardsize);
  mvprintw(0,10, "%d", screen_rows);
  mvprintw(0,20, "%d", screen_cols);
  mvprintw(0,30, "%d", game_columns);

  // Plant the seed of life
  addrandomlife(2000);

  while (running) {
    c = getch();
    if (~c >> 31)
      handleinput(c);
      
    if (!pause)
      update();

    printgame();
    refresh();
    nanosleep (&ts, NULL);
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
      move (0, 0);
      clrtoeol();
      if (pause)
        mvprintw(0, screen_cols / 2 - 3, "Paused");
      break;
    case 'q':
      running = 0;
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

  for (int p = 0; p < screen_rows; p++) {
    memcpy(board[p], tempboard[p], game_columns * sizeof(int));
  }
}

void printgame() {
  int c;
  for (int y = 0; y < screen_rows; y++) {
    for (int x = 0; x < game_columns; x++) {
      c = board[y][x] ^ 3;
      attron(COLOR_PAIR(c));
      if (board[y][x]) {
        mvaddch(y + 1, x * 2, '.');
        mvaddch(y + 1, x * 2 + 1, '.');
      } else {
        mvaddch(y + 1, x * 2, '-');
        mvaddch(y + 1, x * 2 + 1, '-');
      }
      attroff(COLOR_PAIR(c));
    }
  }
}

int countneighbour(int row, int col) {
  int count = 0;
  for (int y = row - 1; y < row + 2; y++) {
    for (int x = col - 1; x < col + 2; x++) {
      if (
        (y | x) >> 31 ||
        y > screen_rows -1 ||
        x > game_columns - 1 ||
        (y == row && x == col)
      ) continue;
      if (board[y][x] == 1) count++;
    }
  }
  return count;
}

void togglelife(int row, int col) {
  int brow = row - 1;
  int bcol = col / 2;
  if ( ~(brow | bcol) >> 31 ) 
    board[brow][bcol] ^= 1;
}

void addrandomlife(int sperm_count) {
  int r, c;
  for (int i = 0; i < sperm_count; i++) {
    r = rand() % screen_rows;
    c = rand() % game_columns;
    board[r][c] = 1;
  }
}
