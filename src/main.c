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
void init_ncurses();

int **board;
int **tempboard;
int pause = 0;
int running = 1;
int screen_rows, screen_cols, game_columns;
MEVENT event;

int main()
{
  struct timespec ts = {0, 100000000L};
  int c;

  srand(time(NULL));

  init_ncurses();

  // Plant the seed of life
  addrandomlife((screen_rows * game_columns) / 5);

  // Main loop
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

/*
 * Set up variables and settings for ncurses
 *
 */
void init_ncurses()
{
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

  // Allocate memory for boards
  board = malloc(screen_rows * sizeof(int *));
  tempboard = malloc(screen_rows * sizeof(int *));

  for (int i = 0; i < screen_rows; i++) {
    board[i] = malloc(game_columns * sizeof(int));
    tempboard[i] = malloc(game_columns * sizeof(int));
  }
}

/*
 * handle user input, mouse clicks, pause, quiting
 *
 */
void handleinput(int c)
{
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

/*
 * apply the rules of life:
 *
 *   Any live cell with fewer than two live neighbours dies, as if by underpopulation.
 *   Any live cell with two or three live neighbours lives on to the next generation.
 *   Any live cell with more than three live neighbours dies, as if by overpopulation.
 *   Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
 *
 */
void update()
{
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

/* 
 * Use ncurses to print our board to the terminal
 *
 */
void printgame()
{
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

/*
 * Count the number of alive cells adjacent to the given cell
 *
 * @param int row
 * @param int col
 *
 * @return int count Number of alive cells adjacent
 */
int countneighbour(int row, int col)
{
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

/*
 * toggle the life of given cell
 */
void togglelife(int row, int col)
{
  int brow = row;
  int bcol = col / 2;

  if (~(brow | bcol) >> 31) {
    board[brow][bcol] ^= 1;
    tempboard[brow][bcol] ^= 1;
  }
}

/*
 * god giveth and god giveth away
 * remove all life from the game
 */
void purgelife()
{
  for (int i = 0; i < screen_rows; i++) {
    memset(board[i], 0, game_columns * sizeof(int));
    memset(tempboard[i], 0, game_columns * sizeof(int));
  }
}

/*
 * add random life to the game
 *
 * @param int sperm_count Amount of life to give
 */
void addrandomlife(int sperm_count)
{
  int r, c;
  for (int i = 0; i < sperm_count; i++) {
    r = rand() % screen_rows;
    c = rand() % game_columns;
    board[r][c] = 1;
  }
}
