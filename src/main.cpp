#include <iostream>
#include "controller.h"
#include "game.h"
#include "renderer.h"
#include "logging.h"


int main() {
  constexpr std::size_t kFramesPerSecond{30};
  constexpr std::size_t kMsPerFrame{1000 / kFramesPerSecond};
  constexpr std::size_t kScreenWidth{640};
  constexpr std::size_t kScreenHeight{640};
  constexpr std::size_t kGridWidth{20};
  constexpr std::size_t kGridHeight{20};

  Logging::SetLevel(LogLevel::kDebug);

  Renderer renderer(kScreenWidth, kScreenHeight, kGridWidth, kGridHeight);
  Controller controller;
  Game game(kGridWidth, kGridHeight);
  game.Run(controller, renderer, kMsPerFrame);
  Logging::Info() << "Game has terminated successfully!";
  Logging::Info() << "Score: " << game.GetScore();
  Logging::Info() << "Size: " << game.GetSize();

  return 0;
}