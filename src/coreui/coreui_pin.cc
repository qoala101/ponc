#include "coreui_pin.h"

namespace esc::coreui {
///
PinIcon::PinIcon(const PinIconData &data) : data_{data} {}

///
auto PinIcon::GetData() const -> const PinIconData & { return data_; }

///
auto PinIcon::GetRect() const -> const ImRect & { return rect_; }

///
void PinIcon::SetRect(const ImRect &rect) { rect_ = rect; }
}  // namespace esc::coreui