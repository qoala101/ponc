#ifndef VH_ESC_TEXTURES_HANDLE_H_
#define VH_ESC_TEXTURES_HANDLE_H_

#include <application.h>
#include <imgui.h>

#include <functional>
#include <memory>

#include "esc_types.h"

namespace esc {
class TexturesHandle {
 public:
  struct TextureIds {
    ImTextureID node_header{};
  };

  explicit TexturesHandle(std::shared_ptr<Application> app);

  TexturesHandle(const TexturesHandle&) = delete;
  TexturesHandle(TexturesHandle&&) noexcept = default;

  auto operator=(const TexturesHandle&) noexcept -> TexturesHandle& = delete;
  auto operator=(TexturesHandle&&) noexcept -> TexturesHandle& = default;

  ~TexturesHandle();

  auto GetTextureIds  () -> const TextureIds&;
  auto GetTextureWithDims  (ImTextureID texture_id)
  -> TextureWithDims;

 private:
  auto GetTextureIdsAsArray  ();
  auto AreTexturesLoaded  ();

  std::shared_ptr<Application> app_{};
  TextureIds texture_ids_{};
};
}  // namespace esc

#endif  // VH_ESC_TEXTURES_HANDLE_H_
