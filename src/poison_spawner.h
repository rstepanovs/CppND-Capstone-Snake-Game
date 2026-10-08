#ifndef POISON_SPAWNER_H
#define POISON_SPAWNER_H

#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <thread>
#include "poison_field.h"

// Runs a background thread that, at random moments, asks the game to put a new
// poison on a random cell. The spawner does not know the board: the game
// decides whether the cell is free (not the food, not the snake, not another
// poison) and places the poison itself, see the `place` callback. Expired
// poisons are removed by the game thread (PoisonField::RemoveExpired).
class PoisonSpawner {
 public:
  // Called by the spawner thread with a candidate cell and the lifetime of the
  // poison. Returns true if the poison was placed, false if the cell is taken.
  using PlaceFunction =
      std::function<bool(const SDL_Point &, std::chrono::milliseconds)>;

  // `field` is only used to count the poisons. The callback is called from the
  // spawner thread, so everything it touches must be protected.
  PoisonSpawner(PoisonField &field, PlaceFunction place, int grid_width,
                int grid_height);

  ~PoisonSpawner();

  PoisonSpawner(const PoisonSpawner &) = delete;
  PoisonSpawner &operator=(const PoisonSpawner &) = delete;

  // While the spawner is not active the thread keeps running but adds no
  // poisons. Called by the game thread, read by the spawner thread, hence the
  // atomic. Not active after construction.
  void SetActive(bool active);

 private:
  void Run();

  PoisonField &_field;
  PlaceFunction _place;
  std::atomic<bool> _active{false};
  int _grid_width;
  int _grid_height;
  std::promise<void> _stop;
  std::future<void> _stop_future;
  std::thread _thread;
};

#endif
