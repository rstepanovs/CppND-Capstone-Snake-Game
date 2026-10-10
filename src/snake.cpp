#include "snake.h"
#include <cmath>
#include "SDL-extras.h"

void Snake::Update() {

  _progress += speed;

  // Do we need to move?
  if (_progress < 1.0f)
    return;

  _progress -= 1.0f;


  SDL_Point prev = head;  // We first capture the head's cell before updating.
  MoveHead();
  UpdateBody(head, prev);
}

void Snake::MoveHead() {
  switch (direction) {
    case Direction::kUp:
      head.y--;
      break;

    case Direction::kDown:
      head.y++;
      break;

    case Direction::kLeft:
      head.x--;
      break;

    case Direction::kRight:
      head.x++;
      break;
  }

  // Wrap the Snake around to the beginning if going off of the screen.
  head.x = (head.x + grid_width) % grid_width;
  head.y = (head.y + grid_height) % grid_height;
}

void Snake::UpdateBody(SDL_Point &current_head_cell, SDL_Point &prev_head_cell) {
  // Add previous head location to vector
  body.push_back(prev_head_cell);

  if (!growing) {
    // Remove the tail from the vector.
    body.erase(body.begin());
  } else {
    growing = false;
    size++;
  }

  // Check if the snake has died.
  for (auto const &item : body) {
    if (current_head_cell == item) {
      alive = false;
    }
  }
}

void Snake::GrowBody() { growing = true; }

// Inefficient method to check if cell is occupied by snake.
bool Snake::SnakeCell(SDL_Point p) {
  if (p == head) {
    return true;
  }
  for (auto const &item : body) {
    if (item == p) {
      return true;
    }
  }
  return false;
}

void Snake::Shrink() {
  if (size > kMinSize) {
    size--;
    body.erase(body.begin());
  }
}