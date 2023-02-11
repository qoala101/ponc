#ifndef VH_DRAW_I_DRAWABLE_H_
#define VH_DRAW_I_DRAWABLE_H_

#include "cpp_interface.h"
#include "esc_app_state.h"

namespace esc::draw {
// ---
class IDrawable : public cpp::Interface {
 public:
  // ---
  virtual void Draw(AppState &app_state) = 0;

  // ---
  auto IsVisible [[nodiscard]] () const -> bool;
  // ---
  void SetVisible(bool visible);

 protected:
  // ---
  virtual void OnVisibleChanged();

  // ---
  auto GetVisible [[nodiscard]] () -> bool &;

 private:
  bool visible_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_I_DRAWABLE_H_
