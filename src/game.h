#ifndef GAME_H
#define GAME_H

#include <memory>
#include <random>
#include "SDL.h"
#include "controller.h"
#include "game_state.h"
#include "item.h"
#include "renderer.h"
#include "snake.h"
#include "message_queue.h"
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

  Snake _snake;
  
  std::unique_ptr<Item> _food;

  PoisonField _poison_field;
  // New poisons from the spawner thread. Declared before the spawner: the
  // thread uses the queue until the spawner's destructor has joined it.
  MessageQueue<std::unique_ptr<Poison>> _poison_queue;
  PoisonSpawner _poison_spawner;

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;

  int score{0};

  void PlaceFood();
  // Takes the poisons that the spawner has sent and puts those whose cell is
  // free (not the food, not the snake, no poison there) on the field, while
  // there are fewer than kMaxPoisons. The others are dropped.
  void AddSpawnedPoisons();
  // Starts a new game: a new snake, score 0, no poisons, a new fruit, the
  // poison spawner active.
  void Reset();
  void Update();
};

#endif