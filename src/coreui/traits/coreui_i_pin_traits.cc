#include "coreui_i_pin_traits.h"

#include <optional>
#include <variant>

#include "core_i_node.h"
#include "cpp_assert.h"
#include "imgui_node_editor.h"

namespace esc::coreui {
///
auto GetPinKind(const coreui::IPinTraits& pin_traits,
                const core::INode& pin_node) -> ne::PinKind {
  const auto pin = pin_traits.GetPin();

  if (std::holds_alternative<ne::PinKind>(pin)) {
    return std::get<ne::PinKind>(pin);
  }

  Expects(std::holds_alternative<ne::PinId>(pin));
  return core::INode::GetPinKind(pin_node, std::get<ne::PinId>(pin));
}

///
auto IPinTraits::GetLabel() const -> std::string { return {}; }

///
auto IPinTraits::GetValue() const
    -> std::variant<std::monostate, float, float*> {
  return {};
}
}  // namespace esc::coreui