/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_COREUI_LINK_H_
#define VH_PONC_COREUI_LINK_H_

#include <imgui.h>

#include "core_link.h"

namespace vh::ponc::coreui {
///
struct Link {
  ///
  core::Link core_link{};
  ///
  ImColor color{};
  ///
  float thickness{};
  ///
  float drop{};
};
}  // namespace vh::ponc::coreui

#endif  // VH_PONC_COREUI_LINK_H_
