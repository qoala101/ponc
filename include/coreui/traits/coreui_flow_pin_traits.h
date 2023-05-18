/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_COREUI_FLOW_PIN_TRAITS_H_
#define VH_PONC_COREUI_FLOW_PIN_TRAITS_H_

#include <imgui_node_editor.h>

#include "coreui_i_pin_traits.h"

namespace vh::ponc::coreui {
///
class FlowPinTraits : public IPinTraits {
 public:
  ///
  explicit FlowPinTraits(ne::PinId pin_id);

  ///
  auto GetPin() const -> std::variant<ne::PinId, ne::PinKind> override;
  ///
  auto GetValue() const -> PinValueVariant override;

 private:
  ///
  ne::PinId pin_id_{};
};
}  // namespace vh::ponc::coreui

#endif  // VH_PONC_COREUI_FLOW_PIN_TRAITS_H_
