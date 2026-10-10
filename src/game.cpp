#include "game.h"
#include <array>
#include <string>
#include "fruit.h"
#include "logging.h"
#include "poison.h"
#include "SDL.h"
#include "SDL-extras.h"


Game::Game(std::size_t grid_width, std::size_t grid_height)
    : _grid_width(grid_width),
      _grid_height(grid_height),
      _snake(grid_width, grid_height),
      _poison_field(),
      _poison_spawner(_poison_queue, grid_width, grid_height),
      engine(dev()),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1)) {
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
        renderer.Render(_snake, *_food, _poison_field.Positions(), score);
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

  while (true) {
    SDL_Point p{random_w(engine), random_h(engine)};
    // Check that the location is not occupied by a snake item before placing
    // food.
    if (!_snake.SnakeCell(p) && !_poison_field.Contains(p)) {
      _food = std::make_unique<Fruit>(p, name);
      return;
    }
  }
}

namespace {
  constexpr std::size_t kMaxPoisons{3};
  }

void Game::AddSpawnedPoisons() {
  std::vector<std::unique_ptr<Poison>> poisons = _poison_queue.TakeAll();
  for(auto &p : poisons) {
    if(_poison_field.Count() >= kMaxPoisons)
      continue;

    if(p->Position() == _food->Position())
      continue;

    if(_snake.SnakeCell(p->Position()))
      continue;

    if(_poison_field.Contains(p->Position()))
      continue;

    _poison_field.Add(std::move(p));
  }
}

void Game::Reset() {
  _poison_queue.TakeAll();
  _snake = Snake(_grid_width, _grid_height);
  score = 0;
  _poison_field.Clear();
  PlaceFood();
  _poison_spawner.SetActive(true);
}

void Game::Update() {

  AddSpawnedPoisons();

  _snake.Update();

  if (!_snake.alive) {
    _state = GameState::kGameOver;
    _poison_spawner.SetActive(false);
    Logging::Info() << "Snake has died. Game over.";
    return;
  }

  SDL_Point new_point = _snake.head;


  // Check if there's food over here
  if (_food->Position() == new_point) {
    _food->Apply(_snake, score);
    PlaceFood();
  }

  _poison_field.RemoveExpired();
  auto poison = _poison_field.TakeAt(new_point);
  if (poison) {
    poison->Apply(_snake, score);
    if (score < 0) {
      score = 0;
    }
  }
}

int Game::GetScore() const { return score; }
int Game::GetSize() const { return _snake.size; }