#ifndef VH_DRAW_LINK_CONNECTION_PROCESS_H_
#define VH_DRAW_LINK_CONNECTION_PROCESS_H_

#include <memory>

#include "draw_new_link.h"

namespace esc {
class State;
}  // namespace esc

namespace esc::draw {
class LinkConnectionProcess {
 public:
  explicit LinkConnectionProcess(
      std::shared_ptr<std::optional<NewLink>> new_link);

  void Draw(const AppState &app_state);

 private:
  std::shared_ptr<std::optional<NewLink>> new_link_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_LINK_CONNECTION_PROCESS_H_
