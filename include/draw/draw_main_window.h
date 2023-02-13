#ifndef VH_DRAW_MAIN_WINDOW_H_
#define VH_DRAW_MAIN_WINDOW_H_

#include <memory>

#include "draw_i_drawable.h"
#include "draw_menu_bar.h"
#include "esc_state.h"

namespace esc::draw {
// ---
class MainWindow : public IDrawable {
 public:
  // ---
  void Draw(State &state) override;

 private:
  MenuBar menu_bar_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_MAIN_WINDOW_H_
