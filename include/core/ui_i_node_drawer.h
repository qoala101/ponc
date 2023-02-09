#ifndef VH_UI_I_NODE_DRAWER_H_
#define VH_UI_I_NODE_DRAWER_H_

#include <imgui.h>

#include <string>

namespace esc {
// NOLINTNEXTLINE(*-special-member-functions)
class INodeDrawer {
 public:
  virtual ~INodeDrawer() = default;

  virtual auto GetLabel [[nodiscard]] () const -> std::string = 0;
  virtual auto GetColor [[nodiscard]] () const -> ImColor = 0;

 protected:
  INodeDrawer() = default;
};
}  // namespace esc

#endif  // VH_UI_I_NODE_DRAWER_H_
