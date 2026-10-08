#ifndef SNAKE_SPRITES_H
#define SNAKE_SPRITES_H

#include <string>
#include <unordered_map>
#include "SDL.h"
#include "snake.h"
#include "texture.h"

// Draws a Snake with sprites: a head, straight and corner body pieces and a tail.
// Which sprite is used depends on the part of the snake and on the sides of the
// cell where the snake enters and leaves it. All of this is one table in
// snake_sprites.cpp.
class SnakeSprites {
 public:
  // Loads all BMP files from "<assets_path>snake/". Missing files are reported
  // by Texture and simply not drawn.
  SnakeSprites(SDL_Renderer *renderer, const std::string &assets_path);

  // Draws tail, body cells and head. cell_w / cell_h is the size of one grid
  // cell in pixels, grid_w / grid_h the grid size in cells.
  void Draw(SDL_Renderer *renderer, const Snake &snake, int cell_w, int cell_h,
            int grid_w, int grid_h) const;

  // Snake parts: tail, body and head. 
  enum class Part { kTail, kBody, kHead };


 private:

  // On which side of `from` lies the adjacent cell `to`. The snake wraps around
  // the edges, so cells on opposite borders are neighbours too.
  static Snake::Direction SideOf(const SDL_Point &from, const SDL_Point &to,
                                 int grid_w, int grid_h);

  static SDL_Point Ahead(const SDL_Point &head, Snake::Direction direction);

  //Returns the file name (without ".bmp") of the sprite for `cell`.
  static const std::string &SpriteName(Part part, const SDL_Point &prev,
                                       const SDL_Point &cell,
                                       const SDL_Point &next, int grid_w,
                                       int grid_h);

  void DrawSprite(SDL_Renderer *renderer, const std::string &name,
                  const SDL_Point &cell, int cell_w, int cell_h) const;

  std::unordered_map<std::string, Texture> textures;
};

#endif
