#include <bits/stdc++.h>
#include <raylib.h>
using namespace std;
#define HEIGHT 800
#define WIDTH 800
#define TILE_SIZE 40
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
    Image img = LoadImageFromTexture(textures[i]);
    int width = textures[i].width;
    int height = textures[i].height;
    vector<int> up;
    vector<int> right;
    vector<int> down;
    vector<int> left;
    for (int j = 0; j < width; j++)
      up.push_back(
          GetSocketType(ColorToInt(GetImageColor(img, 0, j)), SocketTypes));

    for (int j = 0; j < height; j++) 
      right.push_back(
          GetSocketType(ColorToInt(GetImageColor(img, j, width - 1)), SocketTypes));

    for (int j = width - 1; j >= 0; j--) 
      down.push_back(
          GetSocketType(ColorToInt(GetImageColor(img, height - 1, j)), SocketTypes));

    for (int j = height - 1; j >= 0; j--) 
      left.push_back(
          GetSocketType(ColorToInt(GetImageColor(img, j, 0)), SocketTypes));

    // for (int i : up) {
    //   cout << i << " ";
    // }
    Sockets[i] = {up, right, down, left};
    // cout << endl;
    UnloadImage(img);
  }
}

void Setup() {
  // Load each texture
  textures.push_back(LoadTexture("./assets/tiles/roads/blank.png"));
  textures.push_back(LoadTexture("./assets/tiles/roads/up.png"));
  // Add the rotated textures
  vector<int> rotated = {1};
  // Take their sides and calculate sockets for each UP RIGHT DOWN LEFT
  Rotations(rotated);
  CalculateSockets();
}

class Element {
  vector<int> choices;
  bool collapsed;
  pair<int, int> location;
  public:
  Element(){
    // Choices are the indexes of the textures 
    for (int i = 0; i < textures.size(); i++) choices.push_back(i);
    collapsed = false;
    location = {-1, -1};
  }
};

Element grid[CELL_COUNT][CELL_COUNT];


int main(int argc, char *argv[]) {
  InitWindow(WIDTH, HEIGHT, "Tileset Generator");
  Setup();
  SetTargetFPS(FRAMES_PER_SECOND);
  while (!WindowShouldClose()) {
    BeginDrawing();
    // Draw Each loaded texture
    for (int i = 0; i < textures.size(); i++) {
      DrawTexturePro(
          textures[i],
          {0, 0, (float)textures[i].width, (float)textures[i].height},
          {(float)i * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE}, {0, 0}, 0, WHITE);
    }
    ClearBackground(BLACK);
    EndDrawing();
  }
  for (Texture2D t : textures) {
    UnloadTexture(t);
  }
  return 0;
}
