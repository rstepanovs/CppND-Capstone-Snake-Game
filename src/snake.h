#ifndef SNAKE_H
#define SNAKE_H

#include <vector>
#include "SDL.h"
#include "SDL-extras.h"

class Snake {
 public:
  enum class Direction { kUp, kDown, kLeft, kRight };

  // The snake never gets shorter than this: head, one body cell and the tail.
  static constexpr int kMinSize{3};

  Snake(int grid_width, int grid_height)
      : head{static_cast<int>(grid_width / 2), static_cast<int>(grid_height / 2)},
        grid_width(grid_width),
        grid_height(grid_height)
         {
          // Adding tail and body
          SDL_Point t = head;
          SDL_Point b = head;
          t.y += 2;
          b.y += 1;
          body.push_back(t);
          body.push_back(b);

        }

  void Update();

  void GrowBody();
  void Shrink();
  bool SnakeCell(SDL_Point p);

  Direction direction = Direction::kUp;

  float speed{0.06f};
  int size{kMinSize};
  bool alive{true};
  SDL_Point head;
  std::vector<SDL_Point> body;

 private:
  void MoveHead();
  void UpdateBody(SDL_Point &current_cell, SDL_Point &prev_cell);

  // How much of the next step has been accumulated: 0 <= _progress < 1.
  float _progress{0.0f};
  bool growing{false};
  int grid_width;
  int grid_height;
};

#endif