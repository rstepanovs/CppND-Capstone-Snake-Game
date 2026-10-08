#ifndef FRUIT_H
#define FRUIT_H

#include <string>
#include "item.h"

// A harmless fruit: eating it makes the snake longer and faster.
class Fruit : public Item {
 public:
  // `sprite_name` is one of the file names in assets/fruits/ without ".bmp"
  // (apple, banana, cherry, grapes, lemon, orange, pear, strawberry,
  // watermelon).
  Fruit(SDL_Point position, std::string sprite_name);

  void Apply(Snake &snake, int &score) const override;
  const std::string &SpriteName() const override;

 private:
  std::string sprite_name;
};

#endif
