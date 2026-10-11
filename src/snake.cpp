#include "snake.h"
#include <cmath>
#include "SDL-extras.h"

float Snake::GetSpeed() const {
  return _speed;
}

void Snake::IncreaseSpeed() {
  _speed += kSpeedStep;
}

int Snake::GetSize() const {
  return _size;
}

bool Snake::IsAlive() const {
  return _alive;
}

const SDL_Point& Snake::Head() const {
  return _head;
}

const std::vector<SDL_Point>& Snake::Body() const {
  return _body;
}

void Snake::Update() {

  _progress += _speed;

  // Do we need to move?
  if (_progress < 1.0f)
    return;

  _progress -= 1.0f;


  SDL_Point prev = _head;  // We first capture the head's cell before updating.
  MoveHead();
  UpdateBody(_head, prev);
}

void Snake::MoveHead() {
  switch (direction) {
    case Direction::kUp:
      _head.y--;
      break;

    case Direction::kDown:
      _head.y++;
      break;

    case Direction::kLeft:
      _head.x--;
      break;

    case Direction::kRight:
      _head.x++;
      break;
  }

  // Wrap the Snake around to the beginning if going off of the screen.
  _head.x = (_head.x + grid_width) % grid_width;
  _head.y = (_head.y + grid_height) % grid_height;
}

void Snake::UpdateBody(SDL_Point &current_head_cell, SDL_Point &prev_head_cell) {
  // Add previous head location to vector
  _body.push_back(prev_head_cell);

  if (!growing) {
    // Remove the tail from the vector.
    _body.erase(_body.begin());
  } else {
    growing = false;
    _size++;
  }

  // Check if the snake has died.
  for (auto const &item : _body) {
    if (current_head_cell == item) {
      _alive = false;
    }
  }
}

void Snake::GrowBody() { growing = true; }

// Inefficient method to check if cell is occupied by snake.
bool Snake::SnakeCell(SDL_Point p) {
  if (p == _head) {
    return true;
  }
  for (auto const &item : _body) {
    if (item == p) {
      return true;
    }
  }
  return false;
}

void Snake::Shrink() {
  if (_size > kMinSize) {
    _size--;
    _body.erase(_body.begin());
  }
}