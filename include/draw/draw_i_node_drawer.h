#ifndef VH_DRAW_I_NODE_DRAWER_H_
#define VH_DRAW_I_NODE_DRAWER_H_

#include <memory>
#include <string>
#include <vector>

#include "cpp_interface.h"
#include "draw_i_pin_drawer.h"
#include "imgui.h"
#include "imgui_node_editor.h"

namespace esc::draw {
// ---
class INodeDrawer : public cpp::Interface {
 public:
  // ---
  virtual auto GetLabel [[nodiscard]] () const -> std::string = 0;
  // ---
  virtual auto GetColor [[nodiscard]] () const -> ImColor = 0;
  // ---
  virtual auto CreatePinDrawer [[nodiscard]] (ne::PinId pin_id) const
      -> std::unique_ptr<IPinDrawer> = 0;
};
}  // namespace esc::draw

#endif  // VH_DRAW_I_NODE_DRAWER_H_
