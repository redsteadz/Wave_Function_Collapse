#include <algorithm>
#include <bits/stdc++.h>
#include <raylib.h>
using namespace std;
#define HEIGHT 800
#define WIDTH 800
#define TILE_SIZE 200
#define FRAMES_PER_SECOND 60
#define GAP 5
#define CELL_COUNT WIDTH / TILE_SIZE

vector<Texture2D> textures;
unordered_map<int, vector<vector<int>>> Sockets;
unordered_map<int, int> SocketTypes;
// Allocate the textures and their rotations, calculate their Sockets
void Rotations(vector<int> rotated) {
  for (int i : rotated) {
    // Rotate this texture and add it to the textures
    Image original = LoadImageFromTexture(textures[i]);
    for (int j = 0; j < 3; j++) {
      ImageRotateCW(&original);
      textures.push_back(LoadTextureFromImage(original));
    }
    UnloadImage(original);
  }
}

int GetSocketType(int hex, unordered_map<int, int> &SocketTypes) {
  if (SocketTypes.find(hex) == SocketTypes.end())
    SocketTypes[hex] = SocketTypes.size();

  return SocketTypes[hex];
}

void CalculateSockets() {
  // For each texture calculate its pixel sockets
  for (int i = 0; i < textures.size(); i++) {
    // Calculate for the four sides and place the data
    // UP RIGHT BOTTOM LEFT all read CW
    cout << "TEXTURE " << i << endl;
    Image img = LoadImageFromTexture(textures[i]);
    int width = textures[i].width;
    int height = textures[i].height;
    vector<int> up;
    vector<int> right;
    vector<int> down;
    vector<int> left;
    for (int j = 0; j < width; j++) {
      up.push_back(
          GetSocketType(ColorToInt(GetImageColor(img, j, 0)), SocketTypes));
      // cout << j << " "
      //      << GetSocketType(ColorToInt(GetImageColor(img, j, 0)),
      //                       SocketTypes)
      //      << endl;
    }

    for (int j = 0; j < height; j++) {
      right.push_back(GetSocketType(
          ColorToInt(GetImageColor(img, width - 1, j)), SocketTypes));
      // cout << j << " "
      //      << GetSocketType(ColorToInt(GetImageColor(img, width - 1, j)),
      //                       SocketTypes)
      //      << endl;
    }

    for (int j = width - 1; j >= 0; j--)
      down.push_back(GetSocketType(
          ColorToInt(GetImageColor(img, j, height - 1)), SocketTypes));

    for (int j = height - 1; j >= 0; j--)
      left.push_back(
          GetSocketType(ColorToInt(GetImageColor(img, 0, j)), SocketTypes));

    // for (int i : up) {
    //   cout << i << " ";
    // }
    UnloadImage(img);
    Sockets[i] = {up, right, down, left};
    for (int i = 0; i < right.size(); i++){
      if (up[i] != right[i]){
        cout << "Loc " << i << endl;
        cout << "UP: " << up[i] << " RIGHT: " << right[i] << endl;
      }
    }
    // continue;
    // reverse(right.begin(), right.end());
    // reverse(down.begin(), down.end());
    // reverse(left.begin(), left.end());
    cout << "UP: " << endl;
    for (int i : up)
      cout << i << " ";
    cout << endl;
    cout << "RIGHT: " << endl;
    for (int i : right)
      cout << i << " ";
    cout << endl;
    cout << "DOWN: " << endl;
    for (int i : down)
      cout << i << " ";
    cout << endl;
    cout << "LEFT: " << endl;
    for (int i : left)
      cout << i << " ";
    cout << endl;
    if (up == right) {
      cout << "UP RIGHT" << endl;
    };
    if (up == left) {
      cout << "UP LEFT" << endl;
    };
    if (down == right) {
      cout << "DOWN RIGHT" << endl;
    };
    if (down == left) {
      cout << "DOWN LEFT" << endl;
    };
    // cout << endl;
  }
}

void Setup() {
  // Load each texture
  textures.push_back(LoadTexture("./assets/tiles/roads/blank.png"));
  textures.push_back(LoadTexture("./assets/tiles/roads/up.png"));
  // textures.push_back(LoadTexture("./assets/tiles/roads/right.png"));
  // textures.push_back(LoadTexture("./assets/tiles/roads/down.png"));
  // textures.push_back(LoadTexture("./assets/tiles/roads/left.png"));
  // Add the rotated textures
  vector<int> rotated = {1};
  // Take their sides and calculate sockets for each UP RIGHT DOWN LEFT
  Rotations(rotated);
  CalculateSockets();
}

class Element {
  vector<int> options;
  int collapsed;
  pair<int, int> location;

public:
  Element() {
    // Choices are the indexes of the textures
    for (int i = 0; i < textures.size(); i++)
      options.push_back(i);
    // cout << options.size() << endl;
    collapsed = -1;
    location = {-1, -1};
  }
  void Propogate();
  void RandomCollapse() {
    int index = GetRandomValue(0, options.size() - 1);
    collapsed = index;
    options = {options[index]};
    Propogate();
  }
  void LimitOptions(vector<int> SpecifiedOptions, int dir) {
    int compareDir = (dir + 2) % 4;
    cout << compareDir << " " << dir << endl;
    vector<int> newOptions;
    for (int SpecifiedOption : SpecifiedOptions) {
      vector<int> hash = Sockets[SpecifiedOption][dir];
      reverse(hash.begin(), hash.end());
      for (int opt : options) {
        if (Sockets[opt][compareDir] == hash)
          newOptions.push_back(opt);
      }
    }
    options = newOptions;
  }
  vector<int> getOptions() { return options; }
  int get_collapsed() { return collapsed; }
  void set_collapsed(int c) { collapsed = c; }
  pair<int, int> get_location() { return location; }
  void set_location(pair<int, int> l) { location = l; }
  Texture2D *get_tex() {
    int index = options[get_collapsed()];
    return &textures[index];
  }
};

// Element *Grid[CELL_COUNT][CELL_COUNT];
vector<vector<Element *>> Grid(CELL_COUNT, vector<Element *>(CELL_COUNT));

void Element::Propogate() {
  // Based on the options in the element propogate to the next element
  // for the elemnt Propogate UP RIGHT DOWN LEFT
  int dx[] = {0, 1, 0, -1};
  int dy[] = {-1, 0, 1, 0};
  for (int i = 0; i < 4; i++) {
    int x = location.first + dx[i];
    int y = location.second + dy[i];
    if (x >= 0 && x < CELL_COUNT && y >= 0 && y < CELL_COUNT) {
      Grid[y][x]->LimitOptions(options, i);
      // cout << "X: " << x << "Y: " << y << endl;
    }
  }
}

class Helper {
public:
  static void SetLocations() {
    for (int i = 0; i < CELL_COUNT; i++) {
      for (int j = 0; j < CELL_COUNT; j++) {
        Grid[i][j]->set_location({j, i});
      }
    }
  }
  static void Draw() {
    for (int i = 0; i < CELL_COUNT; i++) {
      for (int j = 0; j < CELL_COUNT; j++) {
        if (Grid[i][j]->get_collapsed() != -1) {
          Texture2D t = *Grid[i][j]->get_tex();
          Rectangle dest = {(float)j * TILE_SIZE, (float)i * TILE_SIZE,
                            TILE_SIZE, TILE_SIZE};
          Rectangle src = {0, 0, (float)t.width, (float)t.height};
          DrawTexturePro(t, src, dest, {0, 0}, 0, WHITE);
        } else {
          DrawRectangle(j * TILE_SIZE + GAP, i * TILE_SIZE + GAP,
                        TILE_SIZE - GAP, TILE_SIZE - GAP, GRAY);
          // Draw small blocks of all the options for that element
          vector<int> options = Grid[i][j]->getOptions();
          int options_size = options.size();
          // Draw this number at the center of the tile
          for (int k = 0; k < options_size; k++) {
            DrawTexturePro(textures[options[k]],
                           {0, 0, (float)textures[options[k]].width,
                            (float)textures[options[k]].height},
                           {(float)j * TILE_SIZE + (float)k * TILE_SIZE / 5,
                            (float)i * TILE_SIZE, (float)TILE_SIZE / 5,
                            (float)TILE_SIZE / 5},
                           {0, 0}, 0, WHITE);
          }
          // DrawText(TextFormat("%d", options_size),
          //          j * TILE_SIZE + TILE_SIZE / 2, i * TILE_SIZE + TILE_SIZE /
          //          2, 20, ORANGE);
        }
      }
    }
  }
  static void HandleKeys() {
    if (IsKeyPressed(KEY_R)) {
      // Randomly collapse to a value
      int i = GetRandomValue(0, CELL_COUNT - 1);
      int j = GetRandomValue(0, CELL_COUNT - 1);
      cout << "LOCATION COLLAPSED: " << i << " " << j << endl;
      Grid[i][j]->RandomCollapse();
    }
  }
};

int main(int argc, char *argv[]) {
  InitWindow(WIDTH, HEIGHT, "Tileset Generator");
  Setup();
  for (int i = 0; i < CELL_COUNT; i++) {
    for (int j = 0; j < CELL_COUNT; j++) {
      Grid[i][j] = new Element();
    }
  }
  Helper::SetLocations();
  SetTargetFPS(FRAMES_PER_SECOND);
  while (!WindowShouldClose()) {
    BeginDrawing();
    // Draw Each loaded texture
    // for (int i = 0; i < textures.size(); i++) {
    //   DrawTexturePro(
    //       textures[i],
    //       {0, 0, (float)textures[i].width, (float)textures[i].height},
    //       {(float)i * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE}, {0, 0}, 0, WHITE);
    // }
    Helper::Draw();
    Helper::HandleKeys();
    ClearBackground(BLACK);
    EndDrawing();
  }
  for (Texture2D t : textures) {
    UnloadTexture(t);
  }
  for (int i = 0; i < CELL_COUNT; i++) {
    for (int j = 0; j < CELL_COUNT; j++) {
      delete Grid[i][j];
    }
  }

  return 0;
}
