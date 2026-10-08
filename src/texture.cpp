#include "texture.h"

#include <iostream>

Texture::Texture(SDL_Renderer *renderer, const std::string &path) {
  SDL_Surface *surface = SDL_LoadBMP(path.c_str());
  if (surface == nullptr) {
    std::cerr << "Failed to load BMP file: " << path << " - " << SDL_GetError() << "\n";
    return;
  }
  SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 255, 0, 255));
  _texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (_texture == nullptr) {
    std::cerr << "Failed to create texture from surface: " << SDL_GetError() << "\n";
  }
  SDL_FreeSurface(surface);
}

Texture::~Texture() {
  if (_texture != nullptr) {
    SDL_DestroyTexture(_texture);
    _texture = nullptr;
  }
}

Texture::Texture(Texture &&other) noexcept {
  _texture = other._texture;
  other._texture = nullptr;
}

Texture &Texture::operator=(Texture &&other) noexcept {
  if (this != &other) {
    if (_texture != nullptr) {
      SDL_DestroyTexture(_texture);
    }
    _texture = other._texture;
    other._texture = nullptr;
  }
  return *this;
}

void Texture::Draw(SDL_Renderer *renderer, const SDL_Rect &destination) const {
  if (_texture != nullptr) {
    SDL_RenderCopy(renderer, _texture, nullptr, &destination);
  }   
}
