#ifndef VH_DRAW_I_NODE_FACTORY_DRAWER_H_
#define VH_DRAW_I_NODE_FACTORY_DRAWER_H_

#include <string>

#include "imgui.h"

namespace esc::draw {
// ---
// NOLINTNEXTLINE(*-special-member-functions)
class INodeFactoryDrawer {
 public:
  virtual ~INodeFactoryDrawer() = default;

  // ---
  virtual auto GetLabel [[nodiscard]] () const -> std::string = 0;
  // ---
  virtual auto GetColor [[nodiscard]] () const -> ImColor = 0;

 protected:
  INodeFactoryDrawer() = default;
};
}  // namespace esc::draw

#endif  // VH_DRAW_I_NODE_FACTORY_DRAWER_H_
