#ifndef VH_DRAW_ITEM_DELETER_H_
#define VH_DRAW_ITEM_DELETER_H_

#include <set>

#include "core_diagram.h"
#include "coreui_diagram.h"

namespace esc::draw {
///
class ItemDeleter {
 public:
  ///
  void UnregisterDeletedItems(const core::Diagram &diagram);
  ///
  void DeleteUnregisteredItems(coreui::Diagram &diagram);

 private:
  ///
  std::set<uintptr_t> registered_node_ids_{};
  ///
  std::set<uintptr_t> registered_link_ids_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_ITEM_DELETER_H_
