/*
This project is an inmplementation of a multi-state cellular automata.
Thanks to Erik Fransson for inspiration: https://www.youtube.com/watch?v=TvZI6Xc0J1Y&t=76s
Author: David Lee
Last Modified Date: Sep 26, 2024 
*/

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

// Pinouts from https://github.com/Xinyuan-LilyGO/TTGO-T-Display
#define TFT_MOSI 19
#define TFT_SCLK 18
#define TFT_CS 5
#define TFT_DC 16
#define TFT_RST 23
#define TFT_BL 4

// I only use green, orange, and red in this program
#define ST77XX_BLACK 0x0000
#define ST77XX_WHITE 0xFFFF
#define ST77XX_RED 0xF800
#define ST77XX_GREEN 0x0564
#define ST77XX_BLUE 0x001F
#define ST77XX_CYAN 0x07FF
#define ST77XX_MAGENTA 0xF81F
#define ST77XX_YELLOW 0xFFE0
#define ST77XX_ORANGE 0xF3E0

// Neumann: 4 neighbors in the four cardinal directions (N, E, S, W)
// Moore: 8 neighbors in the eight cardinal directions (N, NE, E, SE, S, SW, W, NW)
#define NEUMANN 0
#define MOORE 1

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

const int CELL_S = 4;       // Cell size, in pixels. assert CELL_S > 1. I like CELL_S = 4 for the ESP-32.
int* M;                     // Fake 2D matrix. Indexed using [row * size of row + column]
int* T;                     // "Temp" matrix to store new values
int N;                      // Number of elements in M and T
int M_X;                    // Width of M, in number of cells
int M_Y;                    // Height of M, in number of cells


int MARGIN_X;               // Size of width margin, in pixels
int MARGIN_Y;               // Size of height margin, in pixels

const int MODE = MOORE;     // Neumann = 4, Moore = 8
const int THRESH = 2;       // If there are more than or equal to THRESH enemies in the neighboring cells, the current cell becomes the enemy cell.
const int STEP_LIM = 200;   // Max number of simulation steps before the simulation resets. 

// Assert numStates == enemy.length == colors.length
int numStates = 3;          // Total number of states (no death state)
int enemy[] = {1,2,0};      // Enemy[i] = the state that i loses to
uint16_t colors[] = {ST77XX_GREEN, ST77XX_ORANGE, ST77XX_RED};  // colors[state] = corresponding color for that state

// This is an attempt to use functors in Arduino, but I don't know enough about the compiler to know if this 
// makes the functions much faster
struct M_Indexer { 
  const int operator()(int x, int y) const { return x * M_Y + y; }
};
M_Indexer indexer;

void setup() {  
  pinMode(TFT_BL, OUTPUT);     // Enable Backlight
  digitalWrite(TFT_BL, HIGH);  // T-Display Backlight
  tft.init(135, 240);          // Initialize ST7789 240x135
  tft.setRotation(1);          // 1 = landscape, 2 = portrait

  M_X = tft.width() / CELL_S;
  M_Y = tft.height() / CELL_S;
  MARGIN_X = (tft.width() % CELL_S) / 2;
  MARGIN_Y = (tft.height() % CELL_S) / 2;
  M = new int[M_X * M_Y];
  T = new int[M_X * M_Y];
  N = M_X * M_Y;
}

void loop() {
  tft.fillScreen(ST77XX_BLACK);
  initializeGOL();
  playGOL();
}

void initializeGOL() {
  // Fill matrix with random states
  for (int x = 0; x < M_X; ++x){
    for (int y = 0; y < M_Y; ++y){
      setCell(x, y, random(numStates));
    }
  }
}

void playGOL() {
  bool keepPlaying = true;
  int steps = 0;
  while (keepPlaying && steps < STEP_LIM) {
    memcpy(M, T, N * sizeof(M[0])); // Source = T, destination = M
    keepPlaying = false;
    for (int x = 0; x < M_X; ++x){
      for (int y = 0; y < M_Y; ++y){
        if (handleCell(x, y)){
          keepPlaying = true;
        }
      }
    }
    ++steps;
  }
}

// Returns: True if the cell value was changed, False otherwise
bool handleCell(const int x, const int y){
  const int idx = indexer(x,y);
  const int centerState = M[idx];
  const int enemyState = enemy[centerState];
  if (getEnemyNeighbors(x, y, centerState, enemyState) > THRESH){
    setCell(x, y, enemyState);
    return true;
  }
  return false;
}

// Returns: Number of enemies in neighboring cells
const int getEnemyNeighbors(const int x, const int y, const int centerState, const int enemyState){
  int numEnemies = 0;
  int len;
  int* neighbors_x;
  int* neighbors_y;
  switch(MODE){
    case(NEUMANN):
    {
      // Workaround for no tuple support, I didn't want to mess with pointers
      len = 4;
      int neighbors_x[4] = {x, x+1, x, x-1};
      int neighbors_y[4] = {y+1, y, y-1, y};
      for (int i = 0; i < len; ++i){
        int n_x = neighbors_x[i];
        int n_y = neighbors_y[i];
        if (n_x < 0 || n_x >= M_X || n_y < 0 || n_y >= M_Y) continue;
        int n_state = M[indexer(n_x, n_y)];
        if (n_state == enemyState){
          ++numEnemies;
        }
      }
      break; 
    }
    case(MOORE):
    {
      len = 8;
      int neighbors_x[8] = {x, x+1, x+1, x+1, x, x-1, x-1, x-1};
      int neighbors_y[8] = {y+1, y+1, y, y-1, y-1, y-1, y, y+1};
      for (int i = 0; i < len; ++i){
        int n_x = neighbors_x[i];
        int n_y = neighbors_y[i];
        if (n_x < 0 || n_x >= M_X || n_y < 0 || n_y >= M_Y) continue;
        int n_state = M[indexer(n_x, n_y)];
        if (n_state == enemyState){
          ++numEnemies;
        }
      }
      break;
    }
    default:
    {
      break;
    }
  }
  return numEnemies;
}

// Sets the corresponding cell in the matrix and screen.
void setCell(const int x, const int y, const int state){
  T[indexer(x, y)] = state;
  fillCell(x, y, colors[state]);
}

// Fills the corresponding cell on the screen.
void fillCell(const int x, const int y, const uint16_t color) {
  uint16_t x0 = CELL_S * x;
  uint16_t y0 = CELL_S * y;
  tft.fillRect(x0 + MARGIN_X, y0 + MARGIN_Y, CELL_S, CELL_S, color);
}