#include "poison.h"

Poison::Poison(SDL_Point position, std::chrono::milliseconds lifetime) : Item(position), _expires_at(std::chrono::steady_clock::now() + lifetime) {}

bool Poison::IsExpired() const {
  return std::chrono::steady_clock::now() >= _expires_at;
}

void Poison::Apply(Snake &snake, int &score) const {
  score--;
  snake.Shrink();
}

const std::string &Poison::SpriteName() const {
  static const std::string sprite_name = "poison";
  return sprite_name;
}
