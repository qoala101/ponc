#ifndef VH_COREUI_TEXTURES_HANDLE_H_
#define VH_COREUI_TEXTURES_HANDLE_H_

#include <string>
#include <unordered_map>

#include "coreui_texture.h"
#include "cpp_hook.h"
#include "cpp_safe_pointer.h"
#include "imgui.h"

namespace esc::coreui {
///
class TexturesHandle : public cpp::EnableSafePointer<TexturesHandle> {
 public:
  ///
  struct Hooks {
    ///
    cpp::Hook<auto(std::string_view file_path)->Texture> load_texture{};
    ///
    cpp::Hook<void(ImTextureID)> destroy_texture{};
  };

  ///
  explicit TexturesHandle(Hooks hooks);

  ///
  TexturesHandle(const TexturesHandle &) = delete;
  ///
  TexturesHandle(TexturesHandle &&) noexcept = default;

  ///
  auto operator=(const TexturesHandle &) -> TexturesHandle & = delete;
  ///
  auto operator=(TexturesHandle &&) noexcept -> TexturesHandle & = default;

  ///
  ~TexturesHandle();

  ///
  auto GetTexture(std::string_view file_path) -> const Texture &;

 private:
  ///
  Hooks hooks_{};
  ///
  std::unordered_map<std::string, Texture> loaded_textures_{};
};
}  // namespace esc::coreui

#endif  // VH_COREUI_TEXTURES_HANDLE_H_
