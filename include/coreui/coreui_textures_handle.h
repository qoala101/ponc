#ifndef VH_COREUI_TEXTURES_HANDLE_H_
#define VH_COREUI_TEXTURES_HANDLE_H_

#include <string>
#include <unordered_map>

#include "coreui_texture.h"
#include "cpp_callbacks.h"
#include "cpp_safe_ptr.h"
#include "imgui.h"

namespace esc::coreui {
///
class TexturesHandle {
 public:
  ///
  struct Callbacks {
    ///
    cpp::Action<auto(std::string_view)->Texture> load_texture{};
    ///
    cpp::Action<void(ImTextureID)> destroy_texture{};
  };

  ///
  explicit TexturesHandle(Callbacks callbacks);

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
  Callbacks callbacks_{};
  ///
  std::unordered_map<std::string, Texture> loaded_textures_{};
};
}  // namespace esc::coreui

#endif  // VH_COREUI_TEXTURES_HANDLE_H_
