#ifndef VH_COREUI_PIN_H_
#define VH_COREUI_PIN_H_

#include <imgui_node_editor.h>

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "core_diagram.h"
#include "core_i_family.h"
#include "core_i_node.h"
#include "core_id_generator.h"
#include "core_link.h"
#include "core_project.h"
#include "coreui_handmade_link.h"
#include "coreui_i_pin_traits.h"
#include "coreui_link_creation.h"
#include "coreui_texture.h"
#include "imgui.h"
#include "imgui_internal.h"

namespace esc::coreui {
///
struct PinIconData {
  ///
  ne::PinId id{};
  ///
  ImColor color{};
  ///
  bool filled{};
};

class PinIcon {
 public:
  ///
  explicit PinIcon(const PinIconData &data);

  ///
  auto GetData() const -> const PinIconData &;

  ///
  auto GetRect(ne::PinId pin_id) const -> ImRect;
  ///
  void SetRect(ne::PinId pin_id, ImRect rect);

 private:
  ///
  PinIconData data_{};
  ///
  std::optional<ImRect> rect{};
};

///
struct Pin {
  ///
  std::optional<PinIcon> icon{};
  ///
  std::variant<std::monostate, float, float *> value{};
  ///
  std::string label{};
};
}  // namespace esc::coreui

#endif  // VH_COREUI_PIN_H_
