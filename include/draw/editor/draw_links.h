#ifndef VH_DRAW_LINKS_H_
#define VH_DRAW_LINKS_H_

#include <memory>

#include "draw_new_link.h"

namespace esc {
class State;
}  // namespace esc

namespace esc::draw {
class Links {
 public:
  explicit Links(std::shared_ptr<std::optional<NewLink>> new_link);

  void Draw(const AppState &app_state);

 private:
  std::shared_ptr<std::optional<NewLink>> new_link_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_LINKS_H_
