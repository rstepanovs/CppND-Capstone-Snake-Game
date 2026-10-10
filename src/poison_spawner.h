#ifndef POISON_SPAWNER_H
#define POISON_SPAWNER_H

#include <atomic>
#include <memory>
#include <future>
#include <thread>
#include "message_queue.h"
#include "poison.h"

// Runs a background thread that, at random moments, creates a poison with a
// random cell and lifetime and sends it to a MessageQueue. The spawner does not
// know the board: the game thread takes the poisons from the queue and decides
// whether the cell is free, see Game::Update.
class PoisonSpawner {
 public:
  using PoisonQueue = MessageQueue<std::unique_ptr<Poison>>;

  // The queue must outlive the spawner (the thread uses it until the
  // destructor has joined it).
  PoisonSpawner(PoisonQueue &queue, int grid_width, int grid_height);

  ~PoisonSpawner();

  PoisonSpawner(const PoisonSpawner &) = delete;
  PoisonSpawner &operator=(const PoisonSpawner &) = delete;

  // While the spawner is not active the thread keeps running but adds no
  // poisons. Called by the game thread, read by the spawner thread, hence the
  // atomic. Not active after construction.
  void SetActive(bool active);

 private:
  void Run();

  PoisonQueue &_queue;
  std::atomic<bool> _active{false};
  int _grid_width;
  int _grid_height;
  std::promise<void> _stop;
  std::future<void> _stop_future;
  std::thread _thread;
};

#endif
