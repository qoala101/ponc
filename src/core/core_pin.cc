/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "core_pin.h"

namespace vh::ponc::core {
///
auto Pin::GetOppositeKind(ne::PinKind pin_kind) -> ne::PinKind {
  return (pin_kind == ne::PinKind::Input) ? ne::PinKind::Output
                                          : ne::PinKind::Input;
}
}  // namespace vh::ponc::core