#include "coreui_textures_handle.h"

namespace esc::coreui {
///
TexturesHandle::TexturesHandle(Hooks hooks)
    : EnableSafePointer{this}, hooks_{std::move(hooks)} {}

///
TexturesHandle::~TexturesHandle() {
  for (const auto &[file_path, texture] : loaded_textures_) {
    hooks_.destroy_texture(texture.id);
  }
}

///
auto TexturesHandle::GetTexture(std::string_view file_path) -> const Texture & {
  auto texture = loaded_textures_.find(file_path.data());

  if (texture == loaded_textures_.end()) {
    texture =
        loaded_textures_.emplace(file_path, hooks_.load_texture(file_path))
            .first;
  }

  return texture->second;
}
}  // namespace esc::coreui