#include "draw_i_drawable.h"

namespace esc::draw {
auto IDrawable::IsVisible() const -> bool { return visible_; }

void IDrawable::SetVisible(bool visible) {
  visible_ = visible;
  OnVisibleChanged();
}

auto IDrawable::GetVisible() -> bool& { return visible_; }

void IDrawable::OnVisibleChanged() {}
}  // namespace esc::draw