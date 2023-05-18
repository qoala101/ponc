/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "coreui_i_node_traits.h"

#include <memory>

#include "coreui_i_header_traits.h"
#include "cpp_assert.h"

namespace vh::ponc::coreui {
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
}  // namespace vh::ponc::coreui