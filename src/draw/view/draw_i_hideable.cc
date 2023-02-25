#include "draw_i_hidable.h"

namespace esc::draw {
auto IHidable::IsVisible() const -> bool { return visible_; }

void IHidable::SetVisible(bool visible) { visible_ = visible; }

auto IHidable::GetVisible() -> bool& { return visible_; }
}  // namespace esc::draw