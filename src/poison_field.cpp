#include "poison_field.h"
#include <algorithm>
#include <utility>
#include <chrono>

void PoisonField::Add(std::unique_ptr<Poison> poison) {
  std::lock_guard<std::mutex> lock(_mutex);
  _poisons.push_back(std::move(poison));
}

void PoisonField::RemoveExpired() {
  std::lock_guard<std::mutex> lock(_mutex);
  _poisons.erase(std::remove_if(_poisons.begin(), _poisons.end(),
                                [](const std::unique_ptr<Poison> &p) {
                                  return p->IsExpired();
                                }),
                 _poisons.end());
}

std::unique_ptr<Poison> PoisonField::TakeAt(const SDL_Point &cell) {
  std::lock_guard<std::mutex> lock(_mutex);
  auto it = std::find_if(_poisons.begin(), _poisons.end(),
                         [&cell](const std::unique_ptr<Poison> &p) {
                           return p->Position().x == cell.x &&
                                  p->Position().y == cell.y;
                         });
  if (it != _poisons.end()) {
    std::unique_ptr<Poison> poison = std::move(*it);
    _poisons.erase(it);
    return poison;
  }
  return nullptr;
}

std::vector<SDL_Point> PoisonField::Positions() const {
  std::lock_guard<std::mutex> lock(_mutex);
  std::vector<SDL_Point> positions;
  for (const auto &poison : _poisons) {
    positions.push_back(poison->Position());
  }
  return positions;
}

std::size_t PoisonField::Count() const {
  std::lock_guard<std::mutex> lock(_mutex);
  return _poisons.size();
}

void PoisonField::Clear() {
    std::lock_guard<std::mutex> lock(_mutex);
    _poisons.clear();
}

// TODO 15: lock the mutex and return true if any poison has Position() equal to
//          `cell` (std::any_of, or a loop). The function is const, that is why
//          the mutex is `mutable`.
bool PoisonField::Contains(const SDL_Point &cell) const {
  (void)cell;
  return false;
}
