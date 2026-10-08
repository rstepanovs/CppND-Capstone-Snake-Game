#include "renderer.h"
#include <iostream>
#include <string>
#include <filesystem>
#include "logging.h"

Renderer::Renderer(const std::size_t screen_width,
                   const std::size_t screen_height,
                   const std::size_t grid_width, const std::size_t grid_height)
    : screen_width(screen_width),
      screen_height(screen_height),
      grid_width(grid_width),
      grid_height(grid_height) {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    Logging::Error() << "SDL could not initialize.";
    Logging::Error() << "SDL_Error: " << SDL_GetError();
  }

  // Create Window
  sdl_window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, screen_width,
                                screen_height, SDL_WINDOW_SHOWN);

  if (nullptr == sdl_window) {
    Logging::Error() << "Window could not be created.";
    Logging::Error() << " SDL_Error: " << SDL_GetError();
  }

  // Create renderer
  sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED);
  if (nullptr == sdl_renderer) {
    Logging::Error() << "Renderer could not be created.";
    Logging::Error() << "SDL_Error: " << SDL_GetError();
  }

  auto base_path = SDL_GetBasePath();
  if (base_path != nullptr) {
    assets_path = std::string(base_path) + "assets/";
    SDL_free(base_path);
  } else {
    Logging::Error() << "SDL_GetBasePath() failed: " << SDL_GetError();
    assets_path = "assets/";  // fallback to relative path
  }
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

  grass_bg = Texture(sdl_renderer, assets_path + "grass/grass_bg.bmp");

  snake_sprites = std::make_unique<SnakeSprites>(sdl_renderer, assets_path);

  if (TTF_Init() != 0) {
    Logging::Error() << "Failed to initialize SDL_ttf: " << TTF_GetError();
  }
  title_font = std::make_unique<TextRenderer>(assets_path + "fonts/Dyuthi-Regular.ttf", 72);
  text_font = std::make_unique<TextRenderer>(assets_path + "fonts/Dyuthi-Regular.ttf", 28);


  try {
    for (const auto &entry : std::filesystem::directory_iterator(assets_path + "fruits/")) {
      if (entry.path().extension() == ".bmp") {
        std::string key = entry.path().stem().string();
        item_textures.emplace(key, Texture(sdl_renderer, entry.path().string()));
      }
    }
  } catch (const std::filesystem::filesystem_error &e) {
     Logging::Error() << "Failed to load fruit textures: " << e.what();
  }
}

Renderer::~Renderer() {
  grass_bg = Texture();  // release the texture
  snake_sprites.reset();
  title_font.reset();
  text_font.reset();
  TTF_Quit();
  item_textures.clear();
  SDL_DestroyRenderer(sdl_renderer);
  sdl_renderer = nullptr;
  SDL_DestroyWindow(sdl_window);
  SDL_Quit();
}

void Renderer::RenderBackground() {
  SDL_Rect block;
  block.w = screen_width / grid_width;
  block.h = screen_height / grid_height;
  for (int y = 0; y < screen_height; y += 2 *block.h) {
    for (int x = 0; x < screen_width; x += 2 * block.w) {
      SDL_Rect destination{x, y, 2 * block.w, 2 * block.h};
      grass_bg.Draw(sdl_renderer, destination);
    }
  } 
}

void Renderer::Render(Snake const snake, const Item &food, const std::vector<SDL_Point> &poisons) {
  DrawScene(snake, food, poisons);
  SDL_RenderPresent(sdl_renderer);
}

void Renderer::DrawScene(Snake const snake, const Item &food, const std::vector<SDL_Point> &poisons) {
  SDL_Rect block;
  block.w = screen_width / grid_width;
  block.h = screen_height / grid_height;

  // Clear screen
  SDL_SetRenderDrawColor(sdl_renderer, 0x2E, 0x4A, 0x2E, 0xFF);
  SDL_RenderClear(sdl_renderer);

  // Removed for better sprites visibility
  //RenderBackground();

  // Render food
  auto it = item_textures.find(food.SpriteName());
  if (it != item_textures.end()) {
    SDL_Rect destination;
    destination.x = food.Position().x * block.w;
    destination.y = food.Position().y * block.h;
    destination.w = block.w;
    destination.h = block.h;
    it->second.Draw(sdl_renderer, destination);
  } else {    
    SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0xCC, 0x00, 0xFF);
    block.x = food.Position().x * block.w;
    block.y = food.Position().y * block.h;
    SDL_RenderFillRect(sdl_renderer, &block);
  }

  for (const auto &poison : poisons) {
    auto it = item_textures.find("poison");
    if (it != item_textures.end()) {
      SDL_Rect destination;
      destination.x = poison.x * block.w;
      destination.y = poison.y * block.h;
      destination.w = block.w;
      destination.h = block.h;
      it->second.Draw(sdl_renderer, destination);
    }
  }
  snake_sprites->Draw(sdl_renderer, snake, block.w, block.h, grid_width, grid_height);
}

void Renderer::RenderStart() {
  SDL_SetRenderDrawColor(sdl_renderer, 0x2E, 0x4A, 0x2E, 0xFF);
  SDL_RenderClear(sdl_renderer);

  SDL_Color title_color{0xE8, 0xE8, 0xE8, 0xFF};
  SDL_Color text_color{0xE8, 0xE8, 0xE8, 0xFF};

  title_font->DrawCentered(sdl_renderer, "Python", screen_width / 2,
                           screen_height / 3 - 36, title_color);
  text_font->DrawCentered(sdl_renderer,
                          "Enter to start game, ESC to exit",
                          screen_width / 2,
                          screen_height / 3 + 76, text_color);

  SDL_RenderPresent(sdl_renderer);
}

void Renderer::RenderGameOver(Snake const snake, const Item &food,
                              const std::vector<SDL_Point> &poisons, int score) {
  DrawScene(snake, food, poisons);
  SDL_SetRenderDrawBlendMode(sdl_renderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 160);
  SDL_RenderFillRect(sdl_renderer, nullptr);
  SDL_SetRenderDrawBlendMode(sdl_renderer, SDL_BLENDMODE_NONE);

  SDL_Color title_color{0xE8, 0xE8, 0xE8, 0xFF};
  SDL_Color text_color{0xE8, 0xE8, 0xE8, 0xFF};

  title_font->DrawCentered(sdl_renderer, "Game Over", screen_width / 2,
                           screen_height / 3 - 36, title_color);
  text_font->DrawCentered(sdl_renderer,
                          "Score: " + std::to_string(score),
                          screen_width / 2,
                          screen_height / 3 + 76, text_color);
  text_font->DrawCentered(sdl_renderer,
                          "Enter to continue, ESC to exit",
                          screen_width / 2,
                          screen_height / 3 + 112, text_color);

  SDL_RenderPresent(sdl_renderer);
}

void Renderer::UpdateWindowTitle(int score, int fps) {
  std::string title{"Snake Score: " + std::to_string(score) + " FPS: " + std::to_string(fps)};
  SDL_SetWindowTitle(sdl_window, title.c_str());
}
