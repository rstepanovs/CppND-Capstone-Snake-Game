#include "snake_sprites.h"
#include <iostream>
#include <string>
#include <filesystem>
#include <map>
#include <tuple>
#include "logging.h"


namespace {

  // Map sprite names to the corresponding part and directions.
  const std::map<std::tuple<SnakeSprites::Part, Snake::Direction, Snake::Direction>,
                 std::string> kSprites = {
      /* Body sprites*/
      {{SnakeSprites::Part::kBody, Snake::Direction::kLeft,
        Snake::Direction::kRight},
       "body_horizontal"},
      {{SnakeSprites::Part::kBody, Snake::Direction::kRight,
        Snake::Direction::kLeft},
       "body_horizontal"},
      {{SnakeSprites::Part::kBody, Snake::Direction::kUp,
        Snake::Direction::kDown},
       "body_vertical"},
      {{SnakeSprites::Part::kBody, Snake::Direction::kDown,
        Snake::Direction::kUp},
       "body_vertical"},
      {{SnakeSprites::Part::kBody, Snake::Direction::kUp,
        Snake::Direction::kRight},
       "corner_up_right"},
      {{SnakeSprites::Part::kBody, Snake::Direction::kRight,
        Snake::Direction::kUp},
       "corner_up_right"},
      {{SnakeSprites::Part::kBody, Snake::Direction::kRight,
        Snake::Direction::kDown},
       "corner_down_right"},
      {{SnakeSprites::Part::kBody, Snake::Direction::kDown,
        Snake::Direction::kRight},
       "corner_down_right"},
      {{SnakeSprites::Part::kBody, Snake::Direction::kDown,
        Snake::Direction::kLeft},
       "corner_down_left"},
      {{SnakeSprites::Part::kBody, Snake::Direction::kLeft,
        Snake::Direction::kDown},
       "corner_down_left"},
      {{SnakeSprites::Part::kBody, Snake::Direction::kLeft,
        Snake::Direction::kUp},
       "corner_up_left"},
      {{SnakeSprites::Part::kBody, Snake::Direction::kUp,
        Snake::Direction::kLeft},
       "corner_up_left"},

      /* Head sprites */
      {{SnakeSprites::Part::kHead, Snake::Direction::kDown,
        Snake::Direction::kUp},
       "head_up"},
      {{SnakeSprites::Part::kHead, Snake::Direction::kUp,
        Snake::Direction::kDown},
       "head_down"},
      {{SnakeSprites::Part::kHead, Snake::Direction::kRight,
        Snake::Direction::kLeft},
       "head_left"},
      {{SnakeSprites::Part::kHead, Snake::Direction::kLeft,
        Snake::Direction::kRight},
       "head_right"},
      {{SnakeSprites::Part::kHead, Snake::Direction::kUp,
        Snake::Direction::kRight},
       "head_vertical_right_up"},
      {{SnakeSprites::Part::kHead, Snake::Direction::kRight,
        Snake::Direction::kUp},
       "head_horizontal_right_up"},
      {{SnakeSprites::Part::kHead, Snake::Direction::kRight,
        Snake::Direction::kDown},
       "head_horizontal_right_down"},
      {{SnakeSprites::Part::kHead, Snake::Direction::kDown,
        Snake::Direction::kRight},
       "head_vertical_right_down"},
      {{SnakeSprites::Part::kHead, Snake::Direction::kDown,
        Snake::Direction::kLeft},
       "head_vertical_left_down"},
      {{SnakeSprites::Part::kHead, Snake::Direction::kLeft,
        Snake::Direction::kDown},
       "head_horizontal_left_down"},
      {{SnakeSprites::Part::kHead, Snake::Direction::kLeft,
        Snake::Direction::kUp},
       "head_horizontal_left_up"},
      {{SnakeSprites::Part::kHead, Snake::Direction::kUp,
        Snake::Direction::kLeft},
       "head_vertical_left_up"},

      /* Tail sprites */
      {{SnakeSprites::Part::kTail, Snake::Direction::kUp,
        Snake::Direction::kDown},
       "tail_up"},
      {{SnakeSprites::Part::kTail, Snake::Direction::kDown,
        Snake::Direction::kUp},
       "tail_down"},
      {{SnakeSprites::Part::kTail, Snake::Direction::kLeft,
        Snake::Direction::kRight},
       "tail_left"},
      {{SnakeSprites::Part::kTail, Snake::Direction::kRight,
        Snake::Direction::kLeft},
       "tail_right"},
  };
}  // namespace

SnakeSprites::SnakeSprites(SDL_Renderer *renderer, const std::string &assets_path) {
  // Load texture for each sprite name in the table. Missing files are reported by Texture and simply not drawn.
  try {
    for (const auto &entry : std::filesystem::directory_iterator(assets_path + "snake/")) {
      if (entry.path().extension() == ".bmp") {
        std::string key = entry.path().stem().string();
        textures.emplace(key, Texture(renderer, entry.path().string()));
      }
    }
  } catch (const std::filesystem::filesystem_error &e) {
    Logging::Error() << "Failed to load snake textures: " << e.what();
  }
  for (const auto &[key, _] : kSprites) {
    const std::string &sprite_name = kSprites.at(key);
    if (textures.find(sprite_name) == textures.end()) {
      Logging::Warning() << "Missing snake sprite: " << sprite_name;
    }
  }
}

Snake::Direction SnakeSprites::SideOf(const SDL_Point &from, const SDL_Point &to,
                                      int grid_w, int grid_h) {
  int dx = to.x - from.x;
  int dy = to.y - from.y;

  // A difference of grid_w - 1 means "one step to the left", the same for y.
  if (dx == grid_w - 1) {
    dx = -1;
  } else if (dx == -(grid_w - 1)) {
    dx = 1;
  }

  if (dy == grid_h - 1) {
    dy = -1;
  } else if (dy == -(grid_h - 1)) {
    dy = 1;
  }

  if (dx == -1) return Snake::Direction::kLeft;
  if (dx == 1) return Snake::Direction::kRight;
  if (dy == -1) return Snake::Direction::kUp;
  return Snake::Direction::kDown;
}

SDL_Point SnakeSprites::Ahead(const SDL_Point &head, Snake::Direction direction) {
  switch (direction) {
    case Snake::Direction::kUp:
      return {head.x, head.y - 1};
    case Snake::Direction::kDown:
      return {head.x, head.y + 1};
    case Snake::Direction::kLeft:
      return {head.x - 1, head.y};
    case Snake::Direction::kRight:
      return {head.x + 1, head.y};
  }
  return head; // Default case, should not happen
}

const std::string &SnakeSprites::SpriteName(Part part, const SDL_Point &prev,
                                             const SDL_Point &cell,
                                             const SDL_Point &next, int grid_w,
                                             int grid_h) {
  Snake::Direction prev_dir = SideOf(cell, prev, grid_w, grid_h);
  Snake::Direction next_dir = SideOf(cell, next, grid_w, grid_h);
  auto it = kSprites.find({part, prev_dir, next_dir});
  if (it != kSprites.end()) {
    return it->second;
  }
  static const std::string invalid_sprite = "invalid";
  return invalid_sprite;
}

void SnakeSprites::DrawSprite(SDL_Renderer *renderer, const std::string &name,
                              const SDL_Point &cell, int cell_w, int cell_h) const {
  auto it = textures.find(name);
  if (it != textures.end()) {
    SDL_Rect destination;
    destination.x = cell.x * cell_w;
    destination.y = cell.y * cell_h;
    destination.w = cell_w;
    destination.h = cell_h;
    it->second.Draw(renderer, destination);
  }
}

void SnakeSprites::Draw(SDL_Renderer *renderer, const Snake &snake, int cell_w,
                        int cell_h, int grid_w, int grid_h) const {
  // Cells from the tail to the head: the body followed by the head cell.
  std::vector<SDL_Point> chain(snake.body.begin(), snake.body.end());
  chain.push_back({static_cast<int>(snake.head_x), static_cast<int>(snake.head_y)});
  const std::size_t chain_size = chain.size();
  
  for (std::size_t i = 1; i < chain_size - 1; ++i) {
    DrawSprite(renderer, SpriteName(Part::kBody, chain[i - 1], chain[i], chain[i + 1], grid_w, grid_h), chain[i], cell_w, cell_h);
  }
  if (chain_size >= 2) {
    DrawSprite(renderer, SpriteName(Part::kHead, chain[chain_size - 2], chain.back(), Ahead(chain.back(), snake.direction), grid_w, grid_h), chain.back(), cell_w, cell_h);
    DrawSprite(renderer, SpriteName(Part::kTail, {2 * chain[0].x - chain[1].x, 2 * chain[0].y - chain[1].y}, chain[0], chain[1], grid_w, grid_h), chain[0], cell_w, cell_h);
  } else if (chain_size == 1) {
    DrawSprite(renderer, SpriteName(Part::kHead, {2 * chain[0].x - Ahead(chain[0], snake.direction).x, 2 * chain[0].y - Ahead(chain[0], snake.direction).y}, chain[0], Ahead(chain[0], snake.direction), grid_w, grid_h), chain[0], cell_w, cell_h);
  }


}
