#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <string>
#include "SDL.h"
#include "SDL_ttf.h"

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

  // Draws `text` so that it is centered horizontally around `center_x`, its
  // top edge is at `top_y`. Does nothing if the font is not loaded or the
  // text is empty.
  void DrawCentered(SDL_Renderer *renderer, const std::string &text,
                    int center_x, int top_y, SDL_Color color) const;

 private:
  TTF_Font *_font{nullptr};  // owned, closed in the destructor
};

#endif
