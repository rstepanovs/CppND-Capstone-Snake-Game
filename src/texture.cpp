#include "texture.h"
#include "logging.h"
#include <iostream>

Texture::Texture(SDL_Renderer *renderer, const std::string &path) {

  SurfacePtr surface(SDL_LoadBMP(path.c_str()), SDL_FreeSurface);
  
  if (!surface) {
    Logging::Error() << "Failed to load BMP file: " << path << " - " << SDL_GetError();
    return;
  }
  SDL_SetColorKey(surface.get(), SDL_TRUE, SDL_MapRGB(surface->format, 255, 0, 255));
  _texture.reset(SDL_CreateTextureFromSurface(renderer, surface.get()));
  if (!_texture) {
    Logging::Error() << "Failed to create texture from surface: " << SDL_GetError();
  }
}


void Texture::Draw(SDL_Renderer *renderer, const SDL_Rect &destination) const {
  if (_texture) {
    SDL_RenderCopy(renderer, _texture.get(), nullptr, &destination);
  }   
}
