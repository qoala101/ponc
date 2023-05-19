/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_CORE_LINK_H_
#define VH_PONC_CORE_LINK_H_

#include <imgui_node_editor.h>

#include <vector>

#include "core_id_ptr.h"

namespace ne = ax::NodeEditor;

namespace vh::ponc::core {
///
struct Link {
  ///
  static auto GetIds(Link &link) -> std::vector<IdPtr>;
  ///
  static auto HasPin(const Link &link, ne::PinId pin_id) -> bool;
  ///
  static auto GetPinKind(const Link &link, ne::PinId pin_id) -> ne::PinKind;
  ///
  static auto GetOtherPin(const Link &link, ne::PinId pin_id) -> ne::PinId;
  ///
  static auto GetPinOfKind(const Link &link, ne::PinKind pin_kind) -> ne::PinId;
  ///
  static auto GetPinOfKind(Link &link, ne::PinKind pin_kind) -> ne::PinId &;

  ///
  ne::LinkId id{};
  ///
  ne::PinId start_pin_id{};
  ///
  ne::PinId end_pin_id{};

 private:
  ///
  friend auto operator==(const Link &, const Link &) -> bool = default;
};
}  // namespace vh::ponc::core

#endif  // VH_PONC_CORE_LINK_H_
