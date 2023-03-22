#ifndef VH_PONC_DRAW_ITEM_DELETER_H_
#define VH_PONC_DRAW_ITEM_DELETER_H_

#include <set>

#include "core_diagram.h"
#include "coreui_diagram.h"

namespace vh::ponc::draw {
///
class ItemDeleter {
 public:
  ///
  struct ItemIds {
    ///
    std::set<uintptr_t> registered_ids_{};
    ///
    std::set<uintptr_t> unregistered_ids_{};
  };

  ///
  void UnregisterDeletedItems(const core::Diagram &diagram);
  ///
  void DeleteUnregisteredItems(coreui::Diagram &diagram);

 private:
  ///
  ItemIds node_ids_{};
  ///
  ItemIds link_ids_{};
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_ITEM_DELETER_H_
