#include "poison_spawner.h"
#include <chrono>
#include <memory>
#include <random>
#include <utility>


namespace {
constexpr int kMinDelayMs{3000};
constexpr int kMaxDelayMs{8000};
constexpr int kMinLifetimeMs{4000};
constexpr int kMaxLifetimeMs{8000};
}  // namespace

PoisonSpawner::PoisonSpawner(PoisonQueue &queue, int grid_width, int grid_height) : _queue(queue), _grid_width(grid_width), _grid_height(grid_height) {
  _stop_future = _stop.get_future();
  _thread = std::thread(&PoisonSpawner::Run, this);  
}

PoisonSpawner::~PoisonSpawner() {
  _stop.set_value();
  _thread.join();
}

void PoisonSpawner::SetActive(bool active) { 
  _active = active;
 }

void PoisonSpawner::Run() {
std::random_device dev;
std::mt19937 engine(dev());
std::uniform_int_distribution<int> delay_dist(kMinDelayMs, kMaxDelayMs);
std::uniform_int_distribution<int> lifetime_dist(kMinLifetimeMs, kMaxLifetimeMs);
std::uniform_int_distribution<int> x_dist(0, _grid_width - 1);
std::uniform_int_distribution<int> y_dist(0, _grid_height - 1);

  while (true)
  {
    int delay = delay_dist(engine);
    if (_stop_future.wait_for(std::chrono::milliseconds(delay)) != std::future_status::timeout) {
      return;
    }

    if (_active) {
      SDL_Point cell{x_dist(engine), y_dist(engine)};
      int lifetime = lifetime_dist(engine);
      _queue.Send(std::make_unique<Poison>(cell, std::chrono::milliseconds(lifetime)));
    }
    
  }
  
}
