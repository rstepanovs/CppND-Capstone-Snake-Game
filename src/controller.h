#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "game_state.h"
#include "snake.h"

class Controller {
 public:
  // Reads all pending events. What a key does depends on `state`:
  //   kStart:    Enter -> kPlaying, ESC -> running = false;
  //   kPlaying:  arrows change the direction of the snake;
  //   kGameOver: Enter -> kStart, ESC -> running = false.
  // Closing the window always sets running = false.
  void HandleInput(GameState &state, bool &running, Snake &snake) const;

 private:
  void ChangeDirection(Snake &snake, Snake::Direction input,
                       Snake::Direction opposite) const;
};

#endif