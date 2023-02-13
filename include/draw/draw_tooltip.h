#ifndef VH_DRAW_TOOLTIP_H_
#define VH_DRAW_TOOLTIP_H_

#include "draw_i_drawable.h"
#include "imgui.h"

namespace esc::draw {
// ---
class Tooltip : public IDrawable {
 public:
  // ---
  explicit Tooltip(std::string text, const ImColor& color);

  // ---
  void Draw(State& state) override;

 private:
  // ---
  std::string text_{};
  // ---
  ImColor color_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_TOOLTIP_H_
