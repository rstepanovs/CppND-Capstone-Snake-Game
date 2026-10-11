#include "poison_spawner.h"
#include <chrono>
#include <memory>
#include <random>
#include <utility>
#include "logging.h"


namespace {
constexpr int kMinDelayMs{3000};
constexpr int kMaxDelayMs{8000};
constexpr int kMinLifetimeMs{4000};
constexpr int kMaxLifetimeMs{8000};
}  // namespace

PoisonSpawner::PoisonSpawner(PoisonQueue &queue, int grid_width, int grid_height) : _queue(queue), _grid_width(grid_width), _grid_height(grid_height) {
  _spawned_future = _spawned.get_future();
  _thread = std::thread(&PoisonSpawner::Run, this);
}

PoisonSpawner::~PoisonSpawner() {

  {
    std::lock_guard lock(_stop_mutex);
    _stopping = true;
  }
  _stop_cv.notify_one();
  
  _thread.join();

  try
  {
    Logging::Info() << "Poisons generated: " << _spawned_future.get();
  }
  catch(const std::exception& e)
  {    
    Logging::Error() << "Poison spawner failed: " << e.what();
  }
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

  std::size_t spawned{0};

  try
  {
    
    while (true)
    {
      int delay = delay_dist(engine);
      std::unique_lock<std::mutex> lock(_stop_mutex);
      if (_stop_cv.wait_for(lock, std::chrono::milliseconds(delay), [this]{return _stopping;}))
        break;
      lock.unlock();

      if (_active) {
        SDL_Point cell{x_dist(engine), y_dist(engine)};
        int lifetime = lifetime_dist(engine);
        _queue.Send(std::make_unique<Poison>(cell, std::chrono::milliseconds(lifetime)));
        spawned++;
      }
    }
    _spawned.set_value(spawned);
  }
  catch(...)
  {
    _spawned.set_exception(std::current_exception());
    
  }
}
