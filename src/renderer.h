#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "SDL.h"
#include "item.h"
#include "snake.h"
#include "snake_sprites.h"
#include "text_renderer.h"
#include "texture.h"

class Renderer {
 public:
  Renderer(const std::size_t screen_width, const std::size_t screen_height,
           const std::size_t grid_width, const std::size_t grid_height);
  ~Renderer();

  // Draws the board and the score in the top left corner.
  void Render(Snake const snake, const Item &food,
              const std::vector<SDL_Point> &poisons, int score);
  // Start screen: the title and the keys.
  void RenderStart();
  // The frozen board under a dark veil, "Game Over", the score and the keys.
  void RenderGameOver(Snake const snake, const Item &food,
                      const std::vector<SDL_Point> &poisons, int score);
  void UpdateWindowTitle(int score, int fps);

 private:
  // Clears the screen and draws fruit, poisons and snake, without presenting.
  void DrawScene(Snake const snake, const Item &food,
                 const std::vector<SDL_Point> &poisons);
  SDL_Window *sdl_window;
  SDL_Renderer *sdl_renderer;

  // Directory with the sprites ("<directory of the executable>/assets/").
  std::string assets_path;
  // Owns the snake sprites. Created once the SDL renderer exists and released
  // before it is destroyed.
  std::unique_ptr<SnakeSprites> snake_sprites;
  std::unordered_map<std::string, Texture> item_textures;
  // Fonts. Must be released before TTF_Quit(), see the destructor.
  std::unique_ptr<TextRenderer> title_font;
  std::unique_ptr<TextRenderer> text_font;

  const std::size_t screen_width;
  const std::size_t screen_height;
  const std::size_t grid_width;
  const std::size_t grid_height;
};

#endif