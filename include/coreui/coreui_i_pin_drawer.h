#ifndef VH_COREUI_I_PIN_DRAWER_H_
#define VH_COREUI_I_PIN_DRAWER_H_

#include <imgui_node_editor.h>

#include <optional>
#include <string>

#include "core_i_node.h"
#include "cpp_interface.h"

namespace ne = ax::NodeEditor;

namespace esc::coreui {
// ---
class IPinDrawer : public cpp::Interface {
 public:
  // ---
  virtual auto GetLabel [[nodiscard]] () const -> std::string;
  // ---
  virtual auto GetKind [[nodiscard]] () const -> std::optional<ne::PinKind>;
  // ---
  virtual auto GetFloat [[nodiscard]] () -> std::optional<float *>;
  // ---
  virtual auto IsEditable [[nodiscard]] () const -> bool;
  // ---
  virtual auto GetPinId [[nodiscard]] () const -> std::optional<ne::PinId>;
};
}  // namespace esc::coreui

#endif  // VH_COREUI_I_PIN_DRAWER_H_
