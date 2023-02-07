#include "esc_textures_handle.h"

#include <bits/ranges_algo.h>

#include "esc_cpp.h"

namespace esc {
auto TexturesHandle::GetTextureIdsAsArray() {
  return std::array{&texture_ids_.restore_icon, &texture_ids_.save_icon,
                    &texture_ids_.header_background};
}

auto TexturesHandle::AreTexturesLoaded() {
  return std::ranges::all_of(GetTextureIdsAsArray(), [](auto* texture_id) {
    return *texture_id != nullptr;
  });
}

TexturesHandle::TexturesHandle(std::shared_ptr<Application> app)
    : app_{(cpp::Expects(app != nullptr), std::move(app))},
      texture_ids_{[&app = app_]() {
        return TextureIds{
            .header_background =
                app->LoadTexture("data/BlueprintBackground.png"),
            .save_icon = app->LoadTexture("data/ic_save_white_24dp.png"),
            .restore_icon = app->LoadTexture("data/ic_restore_white_24dp.png")};
      }()} {
  cpp::Ensures(app_ != nullptr);
  cpp::Ensures(AreTexturesLoaded());
}

auto TexturesHandle::GetTextureIds() -> const TextureIds& {
  return texture_ids_;
}

TexturesHandle::~TexturesHandle() {
  for (auto* texture_id : GetTextureIdsAsArray()) {
    app_->DestroyTexture(texture_id);
    *texture_id = nullptr;
  }
}
}  // namespace esc