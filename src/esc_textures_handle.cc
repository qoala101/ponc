#include "esc_textures_handle.h"

#include <algorithm>
#include <array>
#include <ranges>

#include "cpp_assert.h"

namespace esc {
auto TexturesHandle::GetTextureIdsAsArray() {
  return std::array{&texture_ids_.node_header};
}

auto TexturesHandle::AreTexturesLoaded() {
  return std::ranges::all_of(GetTextureIdsAsArray(), [](auto* texture_id) {
    return *texture_id != ImTextureID{};
  });
}

TexturesHandle::TexturesHandle(std::shared_ptr<Application> app)
    : app_{(Expects(app != nullptr), std::move(app))},
      texture_ids_{[&app = app_]() {
        return TextureIds{.node_header =
                              app->LoadTexture("data/node_header_texture.png")};
      }()} {
  Ensures(app_ != nullptr);
  Ensures(AreTexturesLoaded());
}

auto TexturesHandle::GetTextureIds() -> const TextureIds& {
  return texture_ids_;
}

auto TexturesHandle::GetTextureWithDims(ImTextureID texture_id)
    -> TextureWithDims {
  return {.id = texture_id,
          .width = app_->GetTextureWidth(texture_id),
          .height = app_->GetTextureHeight(texture_id)};
}

TexturesHandle::~TexturesHandle() {
  if (const auto object_was_moved = app_ == nullptr) {
    return;
  }

  for (auto* texture_id : GetTextureIdsAsArray()) {
    app_->DestroyTexture(*texture_id);
    *texture_id = ImTextureID{};
  }
}
}  // namespace esc