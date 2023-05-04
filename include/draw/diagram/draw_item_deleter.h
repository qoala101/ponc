#ifndef VH_PONC_DRAW_ITEM_DELETER_H_
#define VH_PONC_DRAW_ITEM_DELETER_H_

#include <set>

#include "core_diagram.h"
#include "core_id_value.h"
#include "coreui_diagram.h"

namespace vh::ponc::draw {
///
class ItemDeleter {
 public:
  ///
  struct ItemIds {
    ///
    std::set<core::UnspecifiedIdValue> registered_ids_{};
    ///
    std::set<core::UnspecifiedIdValue> unregistered_ids_{};
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
