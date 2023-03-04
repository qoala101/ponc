#include "core_pin.h"

namespace esc::core {
///
auto Pin::GetOppositeKind(ne::PinKind pin_kind) -> ne::PinKind {
  return (pin_kind == ne::PinKind::Input) ? ne::PinKind::Output
                                          : ne::PinKind::Input;
}
}  // namespace esc::core