#include "controller.h"
#include <iostream>
#include "SDL.h"
#include "snake.h"

void Controller::ChangeDirection(Snake &snake, Snake::Direction input,
                                 Snake::Direction opposite) const {
  if (snake.direction != opposite || snake.size == 1) snake.direction = input;
  return;
}

void Controller::HandleInput(GameState &state, bool &running,
                             Snake &snake) const {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      running = false;
    } else if (e.type == SDL_KEYDOWN) {
      if(state == GameState::kPlaying) {
        switch (e.key.keysym.sym) {
          case SDLK_UP:
            ChangeDirection(snake, Snake::Direction::kUp,
                            Snake::Direction::kDown);
            break;

          case SDLK_DOWN:
            ChangeDirection(snake, Snake::Direction::kDown,
                            Snake::Direction::kUp);
            break;

          case SDLK_LEFT:
            ChangeDirection(snake, Snake::Direction::kLeft,
                            Snake::Direction::kRight);
            break;

          case SDLK_RIGHT:
            ChangeDirection(snake, Snake::Direction::kRight,
                            Snake::Direction::kLeft);
            break;
          case SDLK_ESCAPE:
            state = GameState::kGameOver;
            break;
        }
      } else if(state == GameState::kStart) {
        if(e.key.keysym.sym == SDLK_RETURN) {
          state = GameState::kPlaying;
        } else if(e.key.keysym.sym == SDLK_ESCAPE) {
          running = false;
        }
      } else if(state == GameState::kGameOver) {
        if(e.key.keysym.sym == SDLK_RETURN) {
          state = GameState::kStart;
        } else if(e.key.keysym.sym == SDLK_ESCAPE) {
          running = false;
        }
      }
    }
  }
}