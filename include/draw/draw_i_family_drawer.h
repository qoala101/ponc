#ifndef VH_DRAW_I_FAMILY_DRAWER_H_
#define VH_DRAW_I_FAMILY_DRAWER_H_

#include <string>

#include "cpp_interface.h"
#include "imgui.h"

namespace esc::draw {
// ---
class IFamilyDrawer : public cpp::Interface {
 public:
  // ---
  virtual auto GetLabel [[nodiscard]] () const -> std::string = 0;
  // ---
  virtual auto GetColor [[nodiscard]] () const -> ImColor = 0;
};
}  // namespace esc::draw

#endif  // VH_DRAW_I_FAMILY_DRAWER_H_
