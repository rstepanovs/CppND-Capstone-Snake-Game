#ifndef POISON_FIELD_H
#define POISON_FIELD_H

#include <cstddef>
#include <memory>
#include <vector>
#include "SDL.h"
#include "poison.h"

// Storage for the poisons that are currently on the board. Used by the game
// thread only: the spawner thread sends new poisons through a MessageQueue and
// the game thread adds them here, so no locking is needed.
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
  std::vector<std::unique_ptr<Poison>> _poisons;
};

#endif
