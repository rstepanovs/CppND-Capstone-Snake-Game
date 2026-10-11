#include "fruit.h"
#include <utility>


Fruit::Fruit(SDL_Point position, std::string sprite_name)
    : Item(position), sprite_name(std::move(sprite_name)) {}

void Fruit::Apply(Snake &snake, int &score) const {
  score++;
  snake.GrowBody();
  snake.IncreaseSpeed();
}

const std::string &Fruit::SpriteName() const {
  return sprite_name;
}
