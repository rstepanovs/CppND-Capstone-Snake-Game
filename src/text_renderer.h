#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <string>
#include <memory>
#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL-extras.h"


// Draws text with one TrueType font of one size. Owns the TTF_Font.
// TTF_Init() must be called before the first TextRenderer is created and
// TTF_Quit() only after the last one is destroyed.
class TextRenderer {
 public:
  // Opens the font file. If it cannot be opened, an error is logged and Draw
  // does nothing.
  TextRenderer(const std::string &font_path, int size);
  ~TextRenderer();

  // The TTF_Font is owned exclusively, so the object can be neither copied nor
  // moved.
  TextRenderer(const TextRenderer &) = delete;
  TextRenderer &operator=(const TextRenderer &) = delete;
  TextRenderer(TextRenderer &&) = delete;
  TextRenderer &operator=(TextRenderer &&) = delete;

  // Draws `text` starting at `left_x`, its top edge is at `top_y`.
  void DrawLeft(SDL_Renderer *renderer, const std::string &text, int left_x,
                int top_y, SDL_Color color) const;

  // Like DrawCentered, but the color runs through the rainbow along the text
  // and is shifted by `phase` (degrees of hue, any value; call it every frame
  // with a growing value to make the colors flow).
  void DrawGradientCentered(SDL_Renderer *renderer, const std::string &text,
                            int center_x, int top_y, float phase) const;

  // Draws `text` so that it is centered horizontally around `center_x`, its
  // top edge is at `top_y`. Does nothing if the font is not loaded or the
  // text is empty.
  void DrawCentered(SDL_Renderer *renderer, const std::string &text,
                    int center_x, int top_y, SDL_Color color) const;

 private:
  // Renders `text` into a new 32 bit surface (nullptr on failure, the error is
  // logged). The caller owns the surface.
  SurfacePtr RenderSurface(const std::string &text, SDL_Color color) const;
  // Copies the surface to the screen with its top left corner at (x, y), then
  // frees the surface (takes ownership, also when something fails).
  static void DrawSurface(SDL_Renderer *renderer, SurfacePtr surface, int x,
                          int y);

  TTF_Font *_font{nullptr};  // owned, closed in the destructor
};

#endif
