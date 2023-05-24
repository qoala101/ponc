/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_COREUI_I_PIN_TRAITS_H_
#define VH_PONC_COREUI_I_PIN_TRAITS_H_

#include <imgui.h>
#include <imgui_node_editor.h>

#include <optional>
#include <string>
#include <variant>

#include "core_i_node.h"
#include "cpp_non_copyable.h"

namespace ne = ax::NodeEditor;

namespace vh::ponc::coreui {
///
struct PinLabel {
  ///
  std::string text{};
  ///
  ImColor color{};
};

///
struct PinFlow {};

///
using PinValueVariant = std::variant<std::monostate, PinFlow, float, float*>;

///
class IPinTraits : public cpp::NonCopyable {
 public:
  ///
  static auto GetPinKind(const coreui::IPinTraits& pin_traits,
                         const core::INode& pin_node) -> ne::PinKind;

  ///
  virtual auto GetPin() const -> std::variant<ne::PinId, ne::PinKind> = 0;
  ///
  virtual auto GetLabel() const -> std::optional<PinLabel>;
  ///
  virtual auto GetValue() const -> PinValueVariant;
};
}  // namespace vh::ponc::coreui

#endif  // VH_PONC_COREUI_I_PIN_TRAITS_H_
