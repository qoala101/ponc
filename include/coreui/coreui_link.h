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
};
}  // namespace vh::ponc::coreui

#endif  // VH_PONC_COREUI_LINK_H_
