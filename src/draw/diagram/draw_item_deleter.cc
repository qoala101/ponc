#include "draw_item_deleter.h"

#include <concepts>
#include <cstdint>
#include <iterator>

#include "core_link.h"
#include "imgui_node_editor.h"

namespace esc::draw {
namespace {
///
template <typename T>
void UnregisterDeletedItemsImpl(
    const std::vector<T> &items,
    const std::invocable<const T &> auto &get_item_id,
    const std::invocable<uintptr_t> auto &unregister,
    std::set<uintptr_t> &registered_item_ids) {
  auto item_ids = std::set<uintptr_t>{};

  std::transform(items.begin(), items.end(),
                 std::inserter(item_ids, item_ids.begin()), get_item_id);

  auto deleted_item_ids = std::vector<uintptr_t>{};

  std::set_difference(registered_item_ids.begin(), registered_item_ids.end(),
                      item_ids.begin(), item_ids.end(),
                      std::back_inserter(deleted_item_ids));

  for (const auto deleted_id : deleted_item_ids) {
    unregister(deleted_id);
  }

  registered_item_ids = std::move(item_ids);
}
}  // namespace

///
void ItemDeleter::UnregisterDeletedItems(const core::Diagram &diagram) {
  UnregisterDeletedItemsImpl(
      diagram.GetLinks(), [](const auto &link) { return link.id.Get(); },
      [](const auto link_id) { ne::DeleteLink(link_id); },
      registered_link_ids_);

  UnregisterDeletedItemsImpl(
      diagram.GetNodes(), [](const auto &node) { return node->GetId().Get(); },
      [](const auto node_id) { ne::DeleteNode(node_id); },
      registered_node_ids_);
}

///
void ItemDeleter::DeleteUnregisteredItems(coreui::Diagram &diagram) {
  if (ne::BeginDelete()) {
    auto link_id = ne::LinkId{};

    while (ne::QueryDeletedLink(&link_id)) {
      if (!ne::AcceptDeletedItem()) {
        continue;
      }

      registered_link_ids_.erase(link_id.Get());
      diagram.DeleteLink(link_id);
    }

    auto node_id = ne::NodeId{};

    while (ne::QueryDeletedNode(&node_id)) {
      if (!ne::AcceptDeletedItem()) {
        continue;
      }

      registered_node_ids_.erase(node_id.Get());
      diagram.DeleteNode(node_id);
    }
  }

  ne::EndDelete();
}
}  // namespace esc::draw