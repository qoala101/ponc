#include "coreui_i_node_traits.h"

#include <memory>

#include "coreui_i_header_traits.h"
#include "cpp_assert.h"

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

///
auto INodeTraits::GetActionNames() const -> std::vector<std::string> {
  return {};
}

///
void INodeTraits::ExecuteAction(std::string_view /*unused*/) { Expects(false); }
}  // namespace esc::coreui