#include "poison_field.h"
#include <algorithm>
#include <utility>
#include <chrono>
#include "SDL-extras.h"

void PoisonField::Add(std::unique_ptr<Poison> poison) {
  _poisons.push_back(std::move(poison));
}

void PoisonField::RemoveExpired() {
  _poisons.erase(std::remove_if(_poisons.begin(), _poisons.end(),
                                [](const std::unique_ptr<Poison> &p) {
                                  return p->IsExpired();
                                }),
                 _poisons.end());
}

std::unique_ptr<Poison> PoisonField::TakeAt(const SDL_Point &cell) {
  auto it = std::find_if(_poisons.begin(), _poisons.end(),
                         [&cell](const std::unique_ptr<Poison> &p) {
                           return p->Position() == cell;
                         });
  if (it != _poisons.end()) {
    std::unique_ptr<Poison> poison = std::move(*it);
    _poisons.erase(it);
    return poison;
  }
  return nullptr;
}

std::vector<SDL_Point> PoisonField::Positions() const {
  std::vector<SDL_Point> positions;
  for (const auto &poison : _poisons) {
    positions.push_back(poison->Position());
  }
  return positions;
}

std::size_t PoisonField::Count() const {
  return _poisons.size();
}

void PoisonField::Clear() {
      _poisons.clear();
}

bool PoisonField::Contains(const SDL_Point &cell) const {
    for(auto &p : _poisons) {
        if(p->Position() == cell)
            return true;
    }

  return false;
}
