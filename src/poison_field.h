#ifndef POISON_FIELD_H
#define POISON_FIELD_H

#include <cstddef>
#include <memory>
#include <mutex>
#include <vector>
#include "SDL.h"
#include "poison.h"

// Thread-safe storage for the poisons that are currently on the board.
// The spawner thread adds poisons, the game thread removes expired ones,
// eats them and reads their positions for drawing. Every public function
// locks `mutex` for its whole body.
class PoisonField {
 public:
  void Add(std::unique_ptr<Poison> poison);

  void RemoveExpired();

  std::unique_ptr<Poison> TakeAt(const SDL_Point &cell);

  std::vector<SDL_Point> Positions() const;

  std::size_t Count() const;

  // True if there is a poison on `cell`.
  bool Contains(const SDL_Point &cell) const;

  // Removes all poisons (a new game starts).
  void Clear();

 private:
  mutable std::mutex _mutex;
  std::vector<std::unique_ptr<Poison>> _poisons;
};

#endif
