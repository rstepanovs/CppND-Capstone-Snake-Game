#ifndef GAME_STATE_H
#define GAME_STATE_H

// The screens of the game. Game owns the current state, Controller changes it
// in response to the keyboard, Game::Run draws the matching screen.
enum class GameState { kStart, kPlaying, kGameOver };

#endif
