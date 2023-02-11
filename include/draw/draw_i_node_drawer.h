#ifndef VH_DRAW_I_NODE_DRAWER_H_
#define VH_DRAW_I_NODE_DRAWER_H_

#include <string>

#include "imgui.h"

namespace esc::draw {
// ---
// NOLINTNEXTLINE(*-special-member-functions)
class INodeDrawer {
 public:
  virtual ~INodeDrawer() = default;

  // ---
  virtual auto GetLabel [[nodiscard]] () const -> std::string = 0;
  // ---
  virtual auto GetColor [[nodiscard]] () const -> ImColor = 0;

 protected:
  INodeDrawer() = default;
};
}  // namespace esc::draw

#endif  // VH_DRAW_I_NODE_DRAWER_H_
