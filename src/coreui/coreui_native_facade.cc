/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "coreui_native_facade.h"

#include <concepts>

#include "core_concepts.h"
#include "imgui_node_editor.h"

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
}  // namespace

///
auto NativeFacade::GetSelectedNodes() -> std::vector<ne::NodeId> {
  return GetSelectedItems<ne::NodeId>(&ne::GetSelectedNodes);
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