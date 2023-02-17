#include "coreui_empty_pin_drawer.h"

namespace esc::coreui {
EmptyPinDrawer::EmptyPinDrawer(ne::PinKind pin_kind) : pin_kind_{pin_kind} {}

auto EmptyPinDrawer::GetKind() const -> ne::PinKind { return pin_kind_; }
}  // namespace esc::draw