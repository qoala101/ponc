/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_item_deleter.h"

#include <imgui_node_editor.h>

#include <algorithm>
#include <concepts>
#include <functional>
#include <iterator>
#include <utility>
#include <vector>

#include "core_concepts.h"
#include "core_diagram.h"
#include "core_id_value.h"
#include "coreui_diagram.h"
#include "coreui_native_facade.h"

namespace vh::ponc::draw {
namespace {
///
auto GetLinkIds(const core::Diagram &diagram) {
  const auto &links = diagram.GetLinks();

  auto link_ids = std::set<core::UnspecifiedIdValue>{};
  std::transform(links.cbegin(), links.cend(),
                 std::inserter(link_ids, link_ids.cbegin()),
                 [](const auto &link) { return link.id.Get(); });

  return link_ids;
}

///
auto GetNodeIds(const core::Diagram &diagram) {
  auto node_ids = std::set<core::UnspecifiedIdValue>{};

  const auto &nodes = diagram.GetNodes();
  std::transform(nodes.cbegin(), nodes.cend(),
                 std::inserter(node_ids, node_ids.cbegin()),
                 [](const auto &node) { return node->GetId().Get(); });

  const auto &areas = diagram.GetAreas();
  std::transform(areas.cbegin(), areas.cend(),
                 std::inserter(node_ids, node_ids.cbegin()),
                 [](const auto &area) { return area.id.Get(); });

  return node_ids;
}

///
void UnregisterDeletedItemsImpl(
    ItemDeleter::ItemIds &item_ids,
    std::set<core::UnspecifiedIdValue> new_item_ids,
    const std::invocable<core::UnspecifiedIdValue> auto &unregister_item) {
  auto deleted_item_ids = std::vector<core::UnspecifiedIdValue>{};
  std::set_difference(item_ids.registered_ids_.cbegin(),
                      item_ids.registered_ids_.cend(), new_item_ids.cbegin(),
                      new_item_ids.cend(),
                      std::back_inserter(deleted_item_ids));

  for (const auto deleted_id : deleted_item_ids) {
    unregister_item(deleted_id);
    item_ids.unregistered_ids_.insert(deleted_id);
  }

  item_ids.registered_ids_ = std::move(new_item_ids);
}

///
template <core::Id T>
void DeleteUnregisteredItemsImpl(
    ItemDeleter::ItemIds &item_ids,
    const std::invocable<T *> auto &query_deleted_item,
    const std::invocable<T> auto &delete_item) {
  auto item_id = T{};

  while (query_deleted_item(&item_id)) {
    if (!ne::AcceptDeletedItem()) {
      continue;
    }

    const auto item_id_value = item_id.Get();

    if (item_ids.unregistered_ids_.contains(item_id_value)) {
      item_ids.unregistered_ids_.erase(item_id_value);
      continue;
    }

    delete_item(item_id);
    item_ids.registered_ids_.erase(item_id_value);
  }
}

///
void DeleteNode(coreui::Diagram &diagram, ne::NodeId node_id) {
  if (coreui::NativeFacade::IsArea(node_id)) {
    diagram.DeleteArea(node_id);
    return;
  }

  diagram.DeleteNode(node_id);
}
}  // namespace

///
void ItemDeleter::UnregisterDeletedItems(const core::Diagram &diagram) {
  UnregisterDeletedItemsImpl(link_ids_, GetLinkIds(diagram),
                             [](const auto link_id) {
                               ne::DeselectLink(link_id);
                               ne::DeleteLink(link_id);
                             });

  UnregisterDeletedItemsImpl(node_ids_, GetNodeIds(diagram),
                             [](const auto node_id) {
                               ne::DeselectNode(node_id);
                               ne::DeleteNode(node_id);
                             });
}

///
void ItemDeleter::DeleteUnregisteredItems(coreui::Diagram &diagram) {
  if (ne::BeginDelete()) {
    DeleteUnregisteredItemsImpl<ne::LinkId>(
        link_ids_,
        [](const auto link_id) { return ne::QueryDeletedLink(link_id); },
        std::bind_front(&coreui::Diagram::DeleteLink, &diagram));

    DeleteUnregisteredItemsImpl<ne::NodeId>(
        node_ids_, &ne::QueryDeletedNode,
        std::bind_front(&DeleteNode, std::ref(diagram)));
  }

  ne::EndDelete();
}
}  // namespace vh::ponc::draw