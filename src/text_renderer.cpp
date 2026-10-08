#include "text_renderer.h"
#include "logging.h"

TextRenderer::TextRenderer(const std::string &font_path, int size) {
  _font = TTF_OpenFont(font_path.c_str(), size);
  if (!_font) {
    Logging::Error() << "Failed to open font: " << TTF_GetError();
  }
}

TextRenderer::~TextRenderer() {
  if (_font) {
    TTF_CloseFont(_font);
  }
}

void TextRenderer::DrawCentered(SDL_Renderer *renderer, const std::string &text,
                                int center_x, int top_y, SDL_Color color) const {
  if (!_font || text.empty()) {
    return;
  }

  SDL_Surface *surface = TTF_RenderUTF8_Blended(_font, text.c_str(), color);
  if (!surface) {
    Logging::Error() << "Failed to render text: " << TTF_GetError();
    return;
  }

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

  if (!texture) {
    Logging::Error() << "Failed to create texture: " << SDL_GetError();
    return;
  }

  SDL_Rect destination;
  const int w = surface->w;
  const int h = surface->h;
  SDL_FreeSurface(surface);

  destination.w = w;
  destination.h = h;
  destination.x = center_x - destination.w / 2;
  destination.y = top_y;
  
  SDL_RenderCopy(renderer, texture, nullptr, &destination);
  SDL_DestroyTexture(texture);
}
