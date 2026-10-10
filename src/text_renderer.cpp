#include <cmath>
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

  SurfacePtr surface = RenderSurface(text, color);
  if (!surface) {
    Logging::Error() << "Failed to render text: " << TTF_GetError();
    return;
  }

  const int x = center_x - surface->w / 2;
  const int y = top_y;

  DrawSurface(renderer, std::move(surface), x, y);
  
}

SurfacePtr TextRenderer::RenderSurface(const std::string &text,
                                         SDL_Color color) const {
                                          
  if (!_font || text.empty()) {
    return SurfacePtr(nullptr, SDL_FreeSurface);
  }

  // Initialize unique_ptr with object initialize and destructor
  SurfacePtr surface(TTF_RenderUTF8_Blended(_font, text.c_str(), color), SDL_FreeSurface);
  if (!surface) {
    Logging::Error() << "Failed to render text: " << TTF_GetError();
    return SurfacePtr(nullptr, SDL_FreeSurface);
  }

  return surface;
}

void TextRenderer::DrawSurface(SDL_Renderer *renderer, SurfacePtr surface,
                               int x, int y) {

  if (!surface) {
    Logging::Error() << "Failed to render text: " << TTF_GetError();
    return;
  }                                

  TexturePtr texture(SDL_CreateTextureFromSurface(renderer, surface.get()), SDL_DestroyTexture);

  if (!texture) {
    Logging::Error() << "Failed to create texture: " << SDL_GetError();
    return;
  }
  
  SDL_Rect destination;

  destination.w = surface->w;
  destination.h = surface->h;
  destination.x = x;
  destination.y = y;

  SDL_RenderCopy(renderer, texture.get(), nullptr, &destination);
}


void TextRenderer::DrawLeft(SDL_Renderer *renderer, const std::string &text,
                            int left_x, int top_y, SDL_Color color) const {

  if (!_font || text.empty()) {
    return;
  }

  SurfacePtr surface = RenderSurface(text.c_str(), color);
  if (!surface) {
    Logging::Error() << "Failed to render text: " << TTF_GetError();
    return;
  }

  DrawSurface(renderer, std::move(surface), left_x, top_y);

}

namespace {

inline int ConvColor(float num) {
  return static_cast<Uint8>(num * 255.0f);
}

SDL_Color HsvToRgb(float hue, float saturation, float value) {

  float h6 = hue / 60.0f;
  int sector = static_cast<int>(h6) % 6;
  float f = h6 - sector;

  float v = value;
  float p = value * (1 - saturation);
  float t = value * (1 - saturation * (1-f));
  float q = value - (1 - saturation * f);

  Uint8 R = 0,G = 0,B = 0;

  switch (sector)
  {
  case 0:
    R = ConvColor(v);
    G = ConvColor(t);
    B = ConvColor(p);
    break;
  case 1:
    R = ConvColor(q);
    G = ConvColor(v);
    B = ConvColor(p);
    break;
  case 2:
    R = ConvColor(p);
    G = ConvColor(v);
    B = ConvColor(t);
    break;
  case 3:
    R = ConvColor(p);
    G = ConvColor(q);
    B = ConvColor(v);
    break;
  case 4:
    R = ConvColor(t);
    G = ConvColor(p);
    B = ConvColor(v);
    break;
  case 5:    
    R = ConvColor(v);
    G = ConvColor(p);
    B = ConvColor(q);
    break;

  default:
    break;
  }

  return SDL_Color{R, G, B, 255};
}
}  // namespace

void TextRenderer::DrawGradientCentered(SDL_Renderer *renderer,
                                        const std::string &text, int center_x,
                                        int top_y, float phase) const {
  SurfacePtr surface = RenderSurface(text, SDL_Color{255, 255, 255, 255});

  if (!surface) {
    Logging::Error() << "Failed to render text: " << TTF_GetError();
    return;
  }
  int w = surface->w;
  int h = surface->h;
  int p = surface->pitch;
  for(int y=0; y<h; y++) {
    Uint8* row = (Uint8 *)surface->pixels + y * p;
    for(int x=0; x<w; x++){
      Uint32 pixel = ((Uint32*)row)[x];
      Uint8 r,g,b,a;
      SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);
      if(a == 0)
        continue; 
      float hue = fmod(phase + 360.0f * x / surface->w, 360.0f);
      SDL_Color c = HsvToRgb(hue, 0.8f, 1.0f);
      Uint32 *pixels = reinterpret_cast<Uint32 *>(row);
      pixels[x] = SDL_MapRGBA(surface->format, c.r, c.g, c.b, a);
    }
  }

  int x = center_x - w/2;

  DrawSurface(renderer, std::move(surface), x, top_y);
}
