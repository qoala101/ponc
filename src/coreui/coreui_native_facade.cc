/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "coreui_native_facade.h"

#include <imgui_node_editor.h>
#include <imgui_node_editor_internal.h>

#include <concepts>

#include "core_concepts.h"
#include "cpp_assert.h"

namespace vh::ponc::coreui {
namespace {
///
template <core::Id T>
auto GetSelectedItems(const std::invocable<T*, int> auto& selector) {
  auto selected_items = std::vector<T>{};
  selected_items.resize(ne::GetSelectedObjectCount());

  const auto num_selected_items =
      selector(selected_items.data(), static_cast<int>(selected_items.size()));
  selected_items.resize(num_selected_items);

  return selected_items;
}

///
auto IsNodeOfType(ne::NodeId node_id, ne::Detail::NodeType node_type) {
  auto* editor =
      // NOLINTNEXTLINE(*-reinterpret-cast)
      reinterpret_cast<ne::Detail::EditorContext*>(ne::GetCurrentEditor());
  Expects(editor != nullptr);

  const auto* node = editor->FindNode(node_id);
  Expects(node != nullptr);

  return node->m_Type == node_type;
}

///
void LeaveNodesOfType(ne::Detail::NodeType node_type,
                      std::vector<ne::NodeId>& node_ids) {
  std::erase_if(node_ids, [node_type](const auto node_id) {
    return !IsNodeOfType(node_id, node_type);
  });
}
}  // namespace

///
auto NativeFacade::IsArea(ne::NodeId node_id) -> bool {
  return IsNodeOfType(node_id, ne::Detail::NodeType::Group);
}

///
auto NativeFacade::GetSelectedNodes() -> std::vector<ne::NodeId> {
  auto nodes = GetSelectedItems<ne::NodeId>(&ne::GetSelectedNodes);
  LeaveNodesOfType(ne::Detail::NodeType::Node, nodes);
  return nodes;
}

///
auto NativeFacade::GetSelectedNodesAndAreas()
    -> std::pair<std::vector<ne::NodeId>, std::vector<ne::NodeId>> {
  auto nodes = GetSelectedItems<ne::NodeId>(&ne::GetSelectedNodes);
  auto areas = nodes;

  LeaveNodesOfType(ne::Detail::NodeType::Node, nodes);
  LeaveNodesOfType(ne::Detail::NodeType::Group, areas);

  return {std::move(nodes), std::move(areas)};
}

///
auto NativeFacade::GetSelectedLinks() -> std::vector<ne::LinkId> {
  return GetSelectedItems<ne::LinkId>(&ne::GetSelectedLinks);
}

///
void NativeFacade::SelectNode(ne::NodeId node_id, bool append) {
  if (!ne::IsNodeSelected(node_id)) {
    ne::SelectNode(node_id, append);
  }
}

///
void NativeFacade::SelectLink(ne::LinkId link_id, bool append) {
  if (!ne::IsLinkSelected(link_id)) {
    ne::SelectLink(link_id, append);
  }
}
}  // namespace vh::ponc::coreui