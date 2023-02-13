#ifndef VH_DRAW_POPUPS_H_
#define VH_DRAW_POPUPS_H_

#include <memory>

#include "draw_i_drawable.h"
#include "draw_i_popup.h"

namespace esc::draw {
// ---
class Popups : public IDrawable {
 public:
  // ---
  void Draw(State &state) override;

 private:
  // ---
  void UpdateCurrentPopup(State &state);
  // ---
  void DrawCurrentPopup(State &state);

  // ---
  std::optional<std::unique_ptr<IPopup>> current_popup_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_POPUPS_H_
