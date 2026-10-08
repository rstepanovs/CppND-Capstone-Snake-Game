#include "game.h"
#include <iostream>
#include <array>
#include <string>
#include "fruit.h"
#include "logging.h"
#include "poison.h"
#include "SDL.h"


Game::Game(std::size_t grid_width, std::size_t grid_height)
    : _grid_width(grid_width),
      _grid_height(grid_height),
      _snake(grid_width, grid_height),
      _poison_field(),
      _poison_spawner(_poison_field,
                      [this](const SDL_Point &cell,
                             std::chrono::milliseconds lifetime) {
                        return TrySpawnPoison(cell, lifetime);
                      },
                      grid_width, grid_height),
      engine(dev()),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1)) {
  // TODO 17: take the _world_mutex here (std::lock_guard): PlaceFood requires
  //          it. The spawner is not active yet, but the rule is the same
  //          everywhere.
  PlaceFood();
}

void Game::Run(Controller const &controller, Renderer &renderer,
               std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;
  bool running = true;

  while (running) {
    frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.
    const GameState state_before = _state;
    controller.HandleInput(_state, running, _snake);

    if (state_before != GameState::kPlaying && _state == GameState::kPlaying) {
      Reset();
    }

    if (_state != GameState::kPlaying)
    {
      _poison_spawner.SetActive(false);
    }
    

    switch (_state) {
      case GameState::kStart:
        renderer.RenderStart();
        break;
      case GameState::kPlaying:
        Update();
        renderer.Render(_snake, *_food, _poison_field.Positions());
        break;
      case GameState::kGameOver:
        renderer.RenderGameOver(_snake, *_food, _poison_field.Positions(),
                                score);
        break;
    }

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000) {
      renderer.UpdateWindowTitle(score, frame_count);
      frame_count = 0;
      title_timestamp = frame_end;
    }

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }
}

void Game::PlaceFood() {

  const std::array<std::string,9> fruit_names = {
      "apple", "banana", "cherry", "grapes", "lemon", "orange", "pear",
      "strawberry", "watermelon"};

      
  std::uniform_int_distribution<int> random_fruit(0, static_cast<int>(fruit_names.size() - 1));
  const std::string &name = fruit_names[random_fruit(engine)];

  int x, y;
  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    // Check that the location is not occupied by a snake item before placing
    // food.
    // TODO 18: also skip cells that have a poison (_poison_field.Contains).
    if (!_snake.SnakeCell(x, y)) {
      _food = std::make_unique<Fruit>(SDL_Point{x, y}, name);
      return;
    }
  }
}

// TODO 19: implement TrySpawnPoison: take the _world_mutex, return false if
//          `cell` is the food position, a snake cell (_snake.SnakeCell) or
//          _poison_field.Contains(cell); otherwise add
//          std::make_unique<Poison>(cell, lifetime) to _poison_field and return
//          true. The check and the Add must happen under ONE lock, otherwise
//          the snake can move onto the cell in between.
bool Game::TrySpawnPoison(const SDL_Point &cell,
                          std::chrono::milliseconds lifetime) {
  (void)cell;
  (void)lifetime;
  return false;
}

void Game::Reset() {
  // TODO 20: take the _world_mutex for the whole function: _snake and _food
  //          are replaced here while the spawner thread may be reading them.
  _snake = Snake(_grid_width, _grid_height);
  score = 0;
  _poison_field.Clear();
  PlaceFood();
  _poison_spawner.SetActive(true);
}

void Game::Update() {
  // TODO 21: take the _world_mutex for the whole function (the snake moves, the
  //          food is replaced). PoisonField functions called below take their
  //          own mutex inside: that is the allowed lock order.

  _snake.Update();

  if (!_snake.alive) {
    _state = GameState::kGameOver;
    _poison_spawner.SetActive(false);
    Logging::Info() << "Snake has died. Game over.";
    return;
  }

  int new_x = static_cast<int>(_snake.head_x);
  int new_y = static_cast<int>(_snake.head_y);

  // Check if there's food over here
  if (_food->Position().x == new_x && _food->Position().y == new_y) {
    _food->Apply(_snake, score);
    PlaceFood();
  }

  _poison_field.RemoveExpired();
  auto poison = _poison_field.TakeAt(SDL_Point{new_x, new_y});
  if (poison) {
    poison->Apply(_snake, score);
    if (score < 0) {
      score = 0;
    }
  }
}

int Game::GetScore() const { return score; }
int Game::GetSize() const { return _snake.size; }