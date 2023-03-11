#ifndef VH_DRAW_LINKING_H_
#define VH_DRAW_LINKING_H_

#include <imgui_node_editor.h>

#include <optional>
#include <string>

#include "coreui_linker.h"
#include "cpp_callbacks.h"
#include "imgui.h"

namespace esc::draw {
///
class Linker {
 public:
  ///
  struct Callbacks {
    ///
    cpp::Query<ImVec2, ne::PinId> get_pin_tip_pos{};
    ///
    cpp::Signal<ImVec2> new_node_requested_at{};
  };

  ///
  void Draw(coreui::Linker &linker, const Callbacks &callbacks);

 private:
  ///
  void DrawNewNodeQuery(coreui::Linker &linker,
                        const Callbacks &callbacks) const;
  ///
  auto GetPosValue(const coreui::PosVariant &pos_variant,
                   const Callbacks &callbacks) const;
  ///
  void DrawManualLink(const coreui::ManualLink &link,
                      const Callbacks &callbacks) const;

  ///
  ImVec2 new_node_pos_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_LINKING_H_
