/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_DRAW_I_HIDABLE_H_
#define VH_DRAW_I_HIDABLE_H_

#include "cpp_interface.h"

namespace esc::draw {
// ---
class IHidable : public cpp::Interface {
 public:
  // ---
  auto IsVisible [[nodiscard]] () const -> bool;
  // ---
  void SetVisible(bool visible);

 protected:
  // ---
  auto GetVisible [[nodiscard]] () -> bool &;

 private:
  // ---
  bool visible_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_I_HIDABLE_H_
