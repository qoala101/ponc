#include "ui_splitter_node_drawer.h"

#include "core_splitter_node.h"
#include "esc_cpp.h"

namespace vh::esc::ui {
SplitterNodeDrawer::SplitterNodeDrawer(std::shared_ptr<SplitterNode> node)
    : node_{(cpp::Expects(node != nullptr), std::move(node))} {}

auto SplitterNodeDrawer::GetNumOutputs() const {
  return static_cast<int>(node_->GetOutputPins().size());
}

auto SplitterNodeDrawer::GetName() const -> std::string {
  return "Splitter 1x" + std::to_string(GetNumOutputs());
}

auto SplitterNodeDrawer::GetColor() const -> ImColor {
  return {0, 0, 127 + 128 / GetNumOutputs()};
}
}  // namespace vh::esc::ui