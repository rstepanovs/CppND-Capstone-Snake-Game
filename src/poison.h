#ifndef POISON_H
#define POISON_H

#include <chrono>
#include <string>
#include "item.h"

// Poison: eating it makes the snake shorter and costs a point.
// It is only on the board for a limited time.
class Poison : public Item {
 public:
  explicit Poison(SDL_Point position, std::chrono::milliseconds lifetime);
  bool IsExpired() const;

  void Apply(Snake &snake, int &score) const override;
  const std::string &SpriteName() const override;

  private:
  std::chrono::steady_clock::time_point _expires_at;
};

#endif
