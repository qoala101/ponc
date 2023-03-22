#include "core_pin.h"

namespace vh::ponc::core {
///
auto Pin::GetOppositeKind(ne::PinKind pin_kind) -> ne::PinKind {
  return (pin_kind == ne::PinKind::Input) ? ne::PinKind::Output
                                          : ne::PinKind::Input;
}
}  // namespace vh::ponc::core