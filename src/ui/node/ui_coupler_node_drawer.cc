#include "ui_coupler_node_drawer.h"

#include "core_coupler_node.h"
#include "esc_cpp.h"

namespace vh::esc::ui {
CouplerNodeDrawer::CouplerNodeDrawer(std::shared_ptr<CouplerNode> node)
    : node_{(cpp::Expects(node != nullptr), std::move(node))} {}

auto CouplerNodeDrawer::GetName() const -> std::string { return "Coupler 1x2"; }

auto CouplerNodeDrawer::GetColor() const -> ImColor { return {255, 0, 255}; }
}  // namespace vh::esc::ui