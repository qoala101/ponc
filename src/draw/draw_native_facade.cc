#include "draw_native_facade.h"

#include <concepts>

#include "core_concepts.h"
#include "imgui_node_editor.h"

namespace vh::ponc::draw {
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
}  // namespace vh::ponc::draw