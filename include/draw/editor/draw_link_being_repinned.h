#ifndef VH_DRAW_LINK_BEING_REPINNED_H_
#define VH_DRAW_LINK_BEING_REPINNED_H_

#include <memory>

#include "draw_new_link.h"

namespace esc {
class State;
}  // namespace esc

namespace esc::draw {
class LinkBeingRepinned {
 public:
  explicit LinkBeingRepinned(
      std::shared_ptr<std::optional<NewLink>> new_link);

  void Draw(State &state);

 private:
  std::shared_ptr<std::optional<NewLink>> new_link_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_LINK_BEING_REPINNED_H_
