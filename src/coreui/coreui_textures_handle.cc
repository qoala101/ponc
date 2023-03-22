#include "coreui_textures_handle.h"

namespace vh::ponc::coreui {
///
TexturesHandle::TexturesHandle(Callbacks callbacks)
    : callbacks_{std::move(callbacks)} {}

///
TexturesHandle::~TexturesHandle() {
  for (const auto &[file_path, texture] : loaded_textures_) {
    callbacks_.destroy_texture(texture.id);
  }
}

///
auto TexturesHandle::GetTexture(std::string_view file_path) -> const Texture & {
  auto texture = loaded_textures_.find(file_path.data());

  if (texture == loaded_textures_.end()) {
    texture =
        loaded_textures_.emplace(file_path, callbacks_.load_texture(file_path))
            .first;
  }

  return texture->second;
}
}  // namespace vh::ponc::coreui