#include "coreui_i_node_traits.h"

#include <memory>

#include "coreui_i_header_traits.h"

namespace esc::coreui {
///
auto INodeTraits::CreateHeaderTraits() const
    -> std::optional<std::unique_ptr<IHeaderTraits>> {
  return std::nullopt;
}

///
auto INodeTraits::CreatePinTraits() const
    -> std::vector<std::unique_ptr<IPinTraits>> {
  return {};
}
}  // namespace esc::coreui