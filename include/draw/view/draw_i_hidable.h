#ifndef VH_DRAW_I_HIDABLE_H_
#define VH_DRAW_I_HIDABLE_H_

#include "cpp_interface.h"

namespace esc::draw {
class IHidable : public cpp::Interface {
 public:
  auto IsVisible() const -> bool;
  void SetVisible(bool visible);

 protected:
  auto GetVisible() -> bool &;

 private:
  bool visible_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_I_HIDABLE_H_
