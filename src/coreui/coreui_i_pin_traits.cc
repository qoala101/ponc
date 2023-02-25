#include <optional>

#include "coreui_i_pin_traits.h"
#include "imgui_node_editor.h"

namespace esc::coreui {
auto IPinTraits::GetLabel() const -> std::string { return {}; }

auto IPinTraits::GetKind() const -> std::optional<ne::PinKind> {
  return std::nullopt;
}

auto IPinTraits::GetFloat() -> std::optional<float *> { return std::nullopt; }

auto IPinTraits::IsEditable() const -> bool { return false; }

auto IPinTraits::GetPinId() const -> std::optional<ne::PinId> {
  return std::nullopt;
}
}  // namespace esc::coreui