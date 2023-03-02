#include "coreui_i_header_traits.h"

namespace esc::coreui {
///
auto IHeaderTraits::GetTextureFilePath() const -> std::string {
  return "data/node_header_texture.png";
}
}  // namespace esc::coreui