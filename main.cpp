#include <bits/stdc++.h>
#include <raylib.h>
using namespace std;
#define HEIGHT 800
#define WIDTH 800
#define TILE_SIZE 40
#define FRAMES_PER_SECOND 60
#define GAP 5
#define ACTIVE 1279810559

enum Choice {
  EMPTY,
  DOWN,
  LEFT,
  RIGHT,
  UP,
};

// Load Textures in an unordered map
unordered_map<Choice, Texture2D> textures;
unordered_map<Choice, int> HashMap;

int CalculateHash(Choice s) {
  Texture2D *tex = &textures[s];
  int tileWidth = tex->width / 2 - 1;
  Image Temp = LoadImageFromTexture(*tex);
  int hash = 0;
  int dx[] = {tileWidth, 2 * tileWidth, tileWidth, 0};
  int dy[] = {0, tileWidth, 2 * tileWidth, tileWidth};
  for (int k = 0; k < 4; k++) {
    int x = dx[k];
    int y = dy[k];
    // cout << "x: " << x << " y: " << y << endl;
    int val = ColorToInt(GetImageColor(Temp, x, y));
    if (val == ACTIVE) {
      hash |= 1 << (3 - k);
    }
  }
  // cout << "Hash: " << hash << endl;
  UnloadImage(Temp);
  return hash;
}

void AllocateHashes() {
  Choice array[] = {EMPTY, UP, DOWN, LEFT, RIGHT};
  for (auto i : array) {
    HashMap[i] = CalculateHash(i);
  }
}

void LoadTiles() {
  // Load textures
  textures[EMPTY] = LoadTexture("./assets/tiles/roads/blank.png");
  textures[DOWN] = LoadTexture("./assets/tiles/roads/down.png");
  textures[LEFT] = LoadTexture("./assets/tiles/roads/left.png");
  textures[RIGHT] = LoadTexture("./assets/tiles/roads/right.png");
  textures[UP] = LoadTexture("./assets/tiles/roads/up.png");
}

// Make the grid
class Element {
  int collapsed;
  pair<int, int> location;
  int hash;
  Texture2D *tex;
  vector<Choice> options;

public:
  Element() {
    // Indicating not set
    hash = HashMap[UP] | HashMap[DOWN] | HashMap[LEFT] | HashMap[RIGHT];
    collapsed = -1;
    options = {EMPTY, UP, DOWN, LEFT, RIGHT};
  }
  void Reset() {
    hash = HashMap[UP] | HashMap[DOWN] | HashMap[LEFT] | HashMap[RIGHT];
    collapsed = -1;
    options = {EMPTY, UP, DOWN, LEFT, RIGHT};
  }
  void SetLocation(int x, int y) { location = {x, y}; }
  // A function to add a new option to the vector
  // A function to reduce the options in the vector by a give hash
  int Decrypt(int hash) {
    for (int i = 0; i < 2; i++) {
      int lastBit = hash & 1;
      hash >>= 1;
      hash |= lastBit << 3;
    }

    return hash;
  }
  void PopulateOptions(int hash, int mask) {
    // Called from the propogate function of another element
    // Decrypt this hash by shifting it 4 bits to the right while rotating the
    // last bit
    int h = Decrypt(hash);
    int m = Decrypt(mask);
    // cout << location.first << " " << location.second << endl;
    // cout << "Propogated Hash: " << h << endl;
    int shift = 0;
    vector<Choice> newOptions;
    for (Choice c : options) {
      // Apply the mask to the hash of the option and check if it is equal to h
      if ((HashMap[c] & m) == h) {
        newOptions.push_back(c);
      }
    }
    if (options.size() == newOptions.size())
      return;
    options = newOptions;
    UpdateHash();
    // cout << hash << endl;
    // Propogate();
  }
  void UpdateHash() {
    // Based on the options update the hash
    int mask = (1 << 3) - 1;
    for (Choice c : options) {
      mask &= HashMap[c];
    }
    hash = mask;
  }
  void Propogate();
  int get_collapsed() { return collapsed; }
  Texture2D *get_tex() { return tex; }
  // Random Collapse: Among the choices in the list randomly choose one
  // and set it as the collapsed
  void RandomCollapse() {
    if (options.size() == 0)
      return;
    int i = GetRandomValue(0, options.size() - 1);
    SetCollapsed(i);
    Propogate();
  }
  void SetCollapsed(int i) {
    collapsed = i;
    SetTex(&textures[options[i]]);
    hash = HashMap[options[i]];
  }
  vector<Choice> getOptions() { return options; }
  void SetTex(Texture2D *t) { tex = t; }
  int getHash() { return hash; }
};

Element grid[HEIGHT / TILE_SIZE][WIDTH / TILE_SIZE];

void Element::Propogate() {
  // UP RIGHT DOWN LEFT
  int dx[] = {0, 1, 0, -1};
  int dy[] = {-1, 0, 1, 0};
  int numBits = 3;
  int bitPosition = 8;
  int mask = 1 << numBits;
  // Print this mask value
  // cout << "Mask: " << mask << endl;
  int gridSize = WIDTH / TILE_SIZE;
  for (int i = 0; i < 4; i++) {
    // Grab the coded data using the mask;
    int propogateHash = (hash & mask);
    int nextX = location.first + dx[i];
    int nextY = location.second + dy[i];
    // Check if the next location is within the grid
    if (nextX >= 0 && nextX < gridSize && nextY >= 0 && nextY < gridSize &&
        grid[nextY][nextX].get_collapsed() == -1) {
      // Set the new hash
      grid[nextY][nextX].PopulateOptions(propogateHash, mask);
    }
    mask >>= 1;
  }
}

// Draw grid based on collapsed
class Helper {
public:
  static void draw() {
    for (int i = 0; i < HEIGHT / TILE_SIZE; i++) {
      for (int j = 0; j < WIDTH / TILE_SIZE; j++) {
        if (grid[i][j].get_collapsed() != -1) {
          Texture2D t = *grid[i][j].get_tex();
          Rectangle dest = {(float)j * TILE_SIZE, (float)i * TILE_SIZE,
                            TILE_SIZE, TILE_SIZE};
          Rectangle src = {0, 0, (float)t.width, (float)t.height};
          DrawTexturePro(t, src, dest, {0, 0}, 0, WHITE);
        } else {
          DrawRectangle(j * TILE_SIZE + GAP, i * TILE_SIZE + GAP,
                        TILE_SIZE - GAP, TILE_SIZE - GAP, GRAY);
          // Draw small blocks of all the options for that element
          int options_size = grid[i][j].getOptions().size();
          // Draw this number at the center of the tile
          DrawText(TextFormat("%d", options_size),
                   j * TILE_SIZE + TILE_SIZE / 2, i * TILE_SIZE + TILE_SIZE / 2,
                   20, ORANGE);
        }
      }
    }
  }
  static void Setup() {
    for (int i = 0; i < HEIGHT / TILE_SIZE; i++) {
      for (int j = 0; j < WIDTH / TILE_SIZE; j++) {
        grid[i][j] = Element();
        grid[i][j].SetLocation(j, i);
      }
    }
    // grid[0][0].SetCollapsed(0);
  }
  static void HandleKeys() {
    if (IsKeyPressed(KEY_R)) {
      // int i = GetRandomValue(0, HEIGHT / TILE_SIZE - 1);
      // int j = GetRandomValue(0, WIDTH / TILE_SIZE - 1);
      // int cnt = 0;
      // while (grid[i][j].get_collapsed() != -1 && cnt++ != 10) {
      //   i = GetRandomValue(0, HEIGHT / TILE_SIZE - 1);
      //   j = GetRandomValue(0, WIDTH / TILE_SIZE - 1);
      // }
      // grid[i][j].RandomCollapse();
      ResetBoard();
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      Vector2 mouse = GetMousePosition();
      int i = mouse.y / TILE_SIZE;
      int j = mouse.x / TILE_SIZE;
      grid[i][j].RandomCollapse();
    }
    if (IsKeyPressed(KEY_SPACE)) {
      Iteration();
    }
  }
  static void ResetBoard() {
    for (int i = 0; i < HEIGHT / TILE_SIZE; i++) {
      for (int j = 0; j < WIDTH / TILE_SIZE; j++) {
        grid[i][j].Reset();
      }
    }
  }
  static void Iteration() {
    vector<Element *> elements;
    for (int i = 0; i < HEIGHT / TILE_SIZE; i++) {
      for (int j = 0; j < WIDTH / TILE_SIZE; j++) {
        elements.push_back(&grid[i][j]);
      }
    }
    sort(elements.begin(), elements.end(), [](Element *a, Element *b) {
      return a->getOptions().size() < b->getOptions().size();
    });
    vector<Element *> choices;
    int mini_size = INT_MAX;
    for (int i = 0; i < elements.size(); i++) {
      // Find the minimum value that is not collapsed
      if (elements[i]->get_collapsed() == -1 &&
          elements[i]->getOptions().size() <= mini_size) {
        // Add it to the list
        choices.push_back(elements[i]);
        mini_size = elements[i]->getOptions().size();
        // cout << elements[i]->getOptions().size() << endl;
      } else if (mini_size != INT_MAX)
        break;
    }
    // Choose from these randomly to collapse
    // cout << choices.size() << endl;
    if (choices.size() == 0)
      return;
    int i = GetRandomValue(0, choices.size() - 1);
    choices[i]->RandomCollapse();
  }
};

int main(int argc, char *argv[]) {
  InitWindow(HEIGHT, WIDTH, "WFC Demo");
  SetTargetFPS(FRAMES_PER_SECOND);
  SetExitKey(0);
  LoadTiles();
  AllocateHashes();
  Helper::Setup();
  // Element e;
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);
    Helper::draw();
    Helper::HandleKeys();
    Helper::Iteration();
    // e.CalculateHash(UP);
    EndDrawing();
  }
  return 0;
}
