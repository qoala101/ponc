#include "coreui_i_pin_drawer.h"

#include <optional>

#include "imgui_node_editor.h"

namespace esc::coreui {
auto IPinDrawer::GetLabel() const -> std::string { return {}; }

auto IPinDrawer::GetKind() const -> std::optional<ne::PinKind> {
  return std::nullopt;
}

auto IPinDrawer::GetFloat() -> std::optional<float *> { return std::nullopt; }

auto IPinDrawer::IsEditable() const -> bool { return false; }

auto IPinDrawer::GetPinId() const -> std::optional<ne::PinId> {
  return std::nullopt;
}
}  // namespace esc::coreui