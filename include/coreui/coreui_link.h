#ifndef VH_COREUI_LINK_H_
#define VH_COREUI_LINK_H_

#include <imgui.h>

#include "core_link.h"

namespace esc::coreui {
///
struct Link {
  ///
  core::Link core_link{};
  ///
  ImColor color{};
  ///
  float thickness{};
};
}  // namespace esc::coreui

#endif  // VH_COREUI_LINK_H_
