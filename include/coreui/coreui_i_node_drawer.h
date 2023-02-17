#ifndef VH_DRAW_I_NODE_DRAWER_H_
#define VH_DRAW_I_NODE_DRAWER_H_

#include <memory>
#include <string>
#include <vector>

#include "coreui_i_pin_drawer.h"
#include "cpp_interface.h"
#include "imgui.h"
#include "imgui_node_editor.h"

namespace ne = ax::NodeEditor;

namespace esc::coreui {
// ---
class INodeDrawer : public cpp::Interface {
 public:
  // ---
  virtual auto GetLabel [[nodiscard]] () const -> std::string = 0;
  // ---
  virtual auto GetColor [[nodiscard]] () const -> ImColor = 0;
  // ---
  virtual auto HasHeader [[nodiscard]] () const -> bool;
  // ---
  virtual auto CreatePinDrawer [[nodiscard]] (ne::PinId pin_id) const
      -> std::unique_ptr<IPinDrawer> = 0;
};
}  // namespace esc::draw

#endif  // VH_DRAW_I_NODE_DRAWER_H_
