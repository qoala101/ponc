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

TexturesHandle::TexturesHandle(std::function<Application&()> app_provider)
    : app_provider_{(cpp::Expects(bool{app_provider}),
                     std::move(app_provider))},
      texture_ids_{[&app_provider = app_provider_]() {
        auto& app = app_provider();

        return TextureIds{
            .header_background =
                app.LoadTexture("data/BlueprintBackground.png"),
            .save_icon = app.LoadTexture("data/ic_save_white_24dp.png"),
            .restore_icon = app.LoadTexture("data/ic_restore_white_24dp.png")};
      }()} {
  cpp::Ensures(bool{app_provider_});
  cpp::Ensures(AreTexturesLoaded());
}

auto TexturesHandle::GetTextureIds() -> const TextureIds& {
  return texture_ids_;
}

TexturesHandle::~TexturesHandle() {
  auto& app = app_provider_();

  for (auto* texture_id : GetTextureIdsAsArray()) {
    app.DestroyTexture(texture_id);
    *texture_id = nullptr;
  }
}
}  // namespace esc