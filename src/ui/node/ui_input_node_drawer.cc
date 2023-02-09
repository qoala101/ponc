#include "ui_input_node_drawer.h"

#include "core_input_node.h"
#include "esc_cpp.h"

namespace vh::esc::ui {
InputNodeDrawer::InputNodeDrawer(std::shared_ptr<InputNode> node)
    : node_{(cpp::Expects(node != nullptr), std::move(node))} {}

auto InputNodeDrawer::GetName() const -> std::string { return "Input"; }

auto InputNodeDrawer::GetColor() const -> ImColor { return {255, 0, 255}; }
}  // namespace vh::esc::ui