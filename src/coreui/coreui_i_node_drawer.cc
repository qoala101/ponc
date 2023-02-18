#include "coreui_i_node_drawer.h"

namespace esc::coreui {
// ---
auto INodeDrawer::HasHeader() const -> bool { return true; }

// ---
auto INodeDrawer::CreatePinDrawers() const
    -> std::vector<std::unique_ptr<IPinDrawer>> {
  return {};
}
}  // namespace esc::coreui