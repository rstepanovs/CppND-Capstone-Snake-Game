#include "fruit.h"
#include <utility>

namespace {
// How much faster the snake gets after every fruit.
constexpr float kSpeedStep{0.005f};
}  // namespace

Fruit::Fruit(SDL_Point position, std::string sprite_name)
    : Item(position), sprite_name(std::move(sprite_name)) {}

void Fruit::Apply(Snake &snake, int &score) const {
  score++;
  snake.GrowBody();
  snake.speed += kSpeedStep;
}

const std::string &Fruit::SpriteName() const {
  return sprite_name;
}
