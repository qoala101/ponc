/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_DRAW_TEXTURE_H_
#define VH_DRAW_TEXTURE_H_

#include <imgui.h>

namespace esc::draw {
// ---
struct Texture {
  // ---
  ImTextureID id{};
  // ---
  int width{};
  // ---
  int height{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_TEXTURE_H_
