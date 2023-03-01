#ifndef VH_COREUI_I_PIN_TRAITS_H_
#define VH_COREUI_I_PIN_TRAITS_H_

#include <imgui_node_editor.h>

#include <optional>
#include <string>
#include <variant>

#include "core_i_node.h"
#include "cpp_interface.h"

namespace ne = ax::NodeEditor;

namespace esc::coreui {
///
class IPinTraits : public cpp::Interface {
 public:
  ///
  static auto GetPinKind(const coreui::IPinTraits& pin_traits,
                         const core::INode& pin_node) -> ne::PinKind;

  ///
  virtual auto GetPin() const -> std::variant<ne::PinId, ne::PinKind> = 0;
  ///
  virtual auto GetLabel() const -> std::string;
  ///
  virtual auto GetValue() -> std::variant<std::monostate, float, float*>;
};
}  // namespace esc::coreui

#endif  // VH_COREUI_I_PIN_TRAITS_H_
