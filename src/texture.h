#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include "SDL.h"

// RAII wrapper around an SDL_Texture loaded from a BMP file.
// Pure magenta (255, 0, 255) pixels are treated as transparent (color key).
// The texture is owned exclusively: copying is forbidden, moving is allowed.
class Texture {
 public:
  Texture() = default;  // empty texture, draws nothing
  Texture(SDL_Renderer *renderer, const std::string &path);
  ~Texture();

  Texture(const Texture &) = delete;
  Texture &operator=(const Texture &) = delete;
  Texture(Texture &&other) noexcept;
  Texture &operator=(Texture &&other) noexcept;

  // Draws the texture stretched into the given rectangle (does nothing if empty).
  void Draw(SDL_Renderer *renderer, const SDL_Rect &destination) const;
  bool IsLoaded() const { return _texture != nullptr; }

 private:
  SDL_Texture *_texture{nullptr};  // owned, released in the destructor
};

#endif
