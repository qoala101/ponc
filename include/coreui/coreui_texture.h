#ifndef VH_PONC_COREUI_TEXTURE_H_
#define VH_PONC_COREUI_TEXTURE_H_

#include <imgui.h>

namespace vh::ponc::coreui {
///
struct Texture {
  ///
  ImTextureID id{};
  ///
  int width{};
  ///
  int height{};
};
}  // namespace vh::ponc::coreui

#endif  // VH_PONC_COREUI_TEXTURE_H_
