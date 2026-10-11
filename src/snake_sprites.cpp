#include "snake_sprites.h"
#include <string>
#include <filesystem>
#include <map>
#include <tuple>
#include "logging.h"
#include "SDL-extras.h"


namespace {

  using Part = SnakeSprites::Part;
  using Dir  = Snake::Direction;

  // Map sprite names to the corresponding part and directions.
  const std::map<std::tuple<Part, Dir, Dir>,
                 std::string> kSprites = {
      /* Body sprites*/
      {{Part::kBody, Dir::kLeft,        Dir::kRight}, "body_horizontal"},
      {{Part::kBody, Dir::kRight,       Dir::kLeft},  "body_horizontal"},
      {{Part::kBody, Dir::kUp,          Dir::kDown},  "body_vertical"},
      {{Part::kBody, Dir::kDown,        Dir::kUp},    "body_vertical"},
      {{Part::kBody, Dir::kUp,          Dir::kRight}, "corner_up_right"},
      {{Part::kBody, Dir::kRight,       Dir::kUp},    "corner_up_right"},
      {{Part::kBody, Dir::kRight,       Dir::kDown},  "corner_down_right"},
      {{Part::kBody, Dir::kDown,        Dir::kRight}, "corner_down_right"},
      {{Part::kBody, Dir::kDown,        Dir::kLeft},  "corner_down_left"},
      {{Part::kBody, Dir::kLeft,        Dir::kDown},  "corner_down_left"},
      {{Part::kBody, Dir::kLeft,        Dir::kUp},    "corner_up_left"},
      {{Part::kBody, Dir::kUp,          Dir::kLeft},  "corner_up_left"},

      /* Head sprites */
      {{Part::kHead, Dir::kDown,        Dir::kUp},    "head_up"},
      {{Part::kHead, Dir::kUp,          Dir::kDown},  "head_down"},
      {{Part::kHead, Dir::kRight,       Dir::kLeft},  "head_left"},
      {{Part::kHead, Dir::kLeft,        Dir::kRight}, "head_right"},
      {{Part::kHead, Dir::kDown,        Dir::kRight}, "head_vertical_right_up"},
      {{Part::kHead, Dir::kDown,        Dir::kLeft},  "head_vertical_left_up"},
      {{Part::kHead, Dir::kUp,          Dir::kRight}, "head_vertical_right_down"},
      {{Part::kHead, Dir::kUp,          Dir::kLeft},  "head_vertical_left_down"},
      {{Part::kHead, Dir::kLeft,        Dir::kUp},    "head_horizontal_right_up"},
      {{Part::kHead, Dir::kLeft,        Dir::kDown},  "head_horizontal_right_down"},
      {{Part::kHead, Dir::kRight,       Dir::kUp},    "head_horizontal_left_up"},
      {{Part::kHead, Dir::kRight,       Dir::kDown},  "head_horizontal_left_down"},
      /* Tail sprites */
      {{Part::kTail, Dir::kUp,          Dir::kDown},  "tail_up"},
      {{Part::kTail, Dir::kDown,        Dir::kUp},    "tail_down"},
      {{Part::kTail, Dir::kLeft,        Dir::kRight}, "tail_left"},
      {{Part::kTail, Dir::kRight,       Dir::kLeft},  "tail_right"},
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
  std::vector<SDL_Point> chain(snake.Body().begin(), snake.Body().end());
  chain.push_back(snake.Head());
  const std::size_t chain_size = chain.size();

  //Draw head
  DrawSprite(renderer, SpriteName(Part::kHead, chain[chain_size - 2], chain.back(), Ahead(chain.back(), snake.direction), grid_w, grid_h), chain.back(), cell_w, cell_h);

  //Draw body
  for (std::size_t i = 1; i < chain_size - 1; ++i) {
    DrawSprite(renderer, SpriteName(Part::kBody, chain[i - 1], chain[i], chain[i + 1], grid_w, grid_h), chain[i], cell_w, cell_h);
  }

  //Draw tail
  DrawSprite(renderer, SpriteName(Part::kTail, {2 * chain[0].x - chain[1].x, 2 * chain[0].y - chain[1].y}, chain[0], chain[1], grid_w, grid_h), chain[0], cell_w, cell_h);

}
