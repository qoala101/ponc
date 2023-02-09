#include "ui_client_node_drawer.h"

#include "core_client_node.h"
#include "esc_cpp.h"

namespace vh::esc::ui {
ClientNodeDrawer::ClientNodeDrawer(std::shared_ptr<ClientNode> node)
    : node_{(cpp::Expects(node != nullptr), std::move(node))} {}

auto ClientNodeDrawer::GetName() const -> std::string { return "Client"; }

auto ClientNodeDrawer::GetColor() const -> ImColor { return {0, 255, 0}; }
}  // namespace vh::esc::ui