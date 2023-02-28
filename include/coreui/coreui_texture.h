#ifndef VH_COREUI_TEXTURE_H_
#define VH_COREUI_TEXTURE_H_

#include <imgui.h>

namespace esc::coreui {
///
struct Texture {
  ///
  ImTextureID id{};
  ///
  int width{};
  ///
  int height{};
};
}  // namespace esc::coreui

#endif  // VH_COREUI_TEXTURE_H_
