#ifndef SDL_EXTRAS_H
#define SDL_EXTRAS_H

#include <memory>
#include "SDL.h"

using SurfacePtr = std::unique_ptr<SDL_Surface, void (*)(SDL_Surface *)>;
using TexturePtr = std::unique_ptr<SDL_Texture, void (*)(SDL_Texture *)>;
//using 

// Overload operators to compare SDL_Point structures
inline bool operator==(const SDL_Point &a, const SDL_Point &b) {
  return a.x == b.x && a.y == b.y;
}
inline bool operator!=(const SDL_Point &a, const SDL_Point &b) {
  return !(a == b);
}

#endif
