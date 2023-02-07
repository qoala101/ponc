#ifndef VH_ESC_TEXTURES_HANDLE_H_
#define VH_ESC_TEXTURES_HANDLE_H_

#include <application.h>
#include <imgui.h>

#include <functional>

namespace esc {
class TexturesHandle {
 public:
  struct TextureIds {
    ImTextureID header_background{};
    ImTextureID save_icon{};
    ImTextureID restore_icon{};
  };

  explicit TexturesHandle(std::function<Application&()> app_provider);

  TexturesHandle(const TexturesHandle&) = delete;
  TexturesHandle(TexturesHandle&&) noexcept = delete;

  auto operator=(const TexturesHandle&) noexcept -> TexturesHandle& = delete;
  auto operator=(TexturesHandle&&) noexcept -> TexturesHandle& = delete;

  ~TexturesHandle();

  auto GetTextureIds [[nodiscard]] () -> const TextureIds&;

 private:
  auto GetTextureIdsAsArray [[nodiscard]] ();
  auto AreTexturesLoaded [[nodiscard]] ();

  std::function<Application&()> app_provider_{};
  TextureIds texture_ids_{};
};
}  // namespace esc

#endif  // VH_ESC_TEXTURES_HANDLE_H_
