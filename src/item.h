#ifndef ITEM_H
#define ITEM_H

#include <string>
#include "SDL.h"
#include "snake.h"

// Abstract base class for everything the snake can eat (fruits, poison).
// Derived classes decide what happens when the snake reaches the item and
// which sprite is drawn.
class Item {
 public:
  explicit Item(SDL_Point position) : _position(position) {}


  virtual ~Item() = default;

  virtual void Apply(Snake &snake, int &score) const = 0;
  virtual const std::string &SpriteName() const = 0;

  const SDL_Point &Position() const { return _position; }

 protected:
  // Protected so derived classes can read it but outside code cannot change it.
  SDL_Point _position;
};

#endif
