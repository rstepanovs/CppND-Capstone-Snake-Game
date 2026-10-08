#ifndef GAME_H
#define GAME_H

#include <chrono>
#include <memory>
#include <mutex>
#include <random>
#include "SDL.h"
#include "controller.h"
#include "game_state.h"
#include "item.h"
#include "renderer.h"
#include "snake.h"
#include "poison_field.h"
#include "poison_spawner.h"

class Game {
 public:
  Game(std::size_t grid_width, std::size_t grid_height);
  void Run(Controller const &controller, Renderer &renderer,
           std::size_t target_frame_duration);
  int GetScore() const;
  int GetSize() const;

 private:
  std::size_t _grid_width;
  std::size_t _grid_height;
  GameState _state{GameState::kStart};

  // Protects what the spawner thread reads: _snake (head, body) and _food.
  // The game thread takes it in every function that WRITES them: Update, Reset
  // and the constructor. The spawner thread takes it in TrySpawnPoison.
  // Lock order: _world_mutex first, then the mutex inside _poison_field, never
  // the other way round (otherwise: deadlock). Must be declared before
  // _poison_spawner: its thread may call TrySpawnPoison as soon as it runs.
  std::mutex _world_mutex;

  Snake _snake;
  
  std::unique_ptr<Item> _food;

  PoisonField _poison_field;
  PoisonSpawner _poison_spawner;

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;

  int score{0};

  // The caller holds _world_mutex.
  void PlaceFood();
  // Called by the spawner thread. Places a poison on `cell` unless the cell is
  // the food, a snake cell or already has a poison. Returns true if placed.
  bool TrySpawnPoison(const SDL_Point &cell, std::chrono::milliseconds lifetime);
  // Starts a new game: a new snake, score 0, no poisons, a new fruit, the
  // poison spawner active.
  void Reset();
  void Update();
};

#endif