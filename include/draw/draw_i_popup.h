#ifndef VH_DRAW_I_POPUP_H_
#define VH_DRAW_I_POPUP_H_

#include "draw_i_drawable.h"
#include "esc_state.h"

namespace esc::draw {
// ---
class IPopup : public IDrawable {
 public:
  // ---
  void Draw(State &state) override;

  // ---
  void Open(State &state);

 private:
  // ---
  virtual auto GetLabel [[nodiscard]] (State &state) const -> std::string = 0;
  // ---
  virtual void DrawContent(State &state) = 0;
};
}  // namespace esc::draw

#endif  // VH_DRAW_I_POPUP_H_
