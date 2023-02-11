#ifndef VH_DRAW_I_PIN_DRAWER_H_
#define VH_DRAW_I_PIN_DRAWER_H_

#include <imgui_node_editor.h>

#include <string>

#include "cpp_interface.h"

namespace esc::draw {
namespace ne = ax::NodeEditor;

// ---
// NOLINTNEXTLINE(*-special-member-functions)
class IPinDrawer : public cpp::Interface {
 public:
  // ---
  virtual auto GetLabel [[nodiscard]] () const -> std::string = 0;
  // ---
  virtual auto GetKind [[nodiscard]] () const -> ne::PinKind = 0;
  // ---
  virtual auto GetFloat [[nodiscard]] () -> float* = 0;
  // ---
  virtual auto IsEditable [[nodiscard]] () const -> bool = 0;
};
}  // namespace esc::draw

#endif  // VH_DRAW_I_PIN_DRAWER_H_
