/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_COREUI_I_NODE_TRAITS_H_
#define VH_PONC_COREUI_I_NODE_TRAITS_H_

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "coreui_i_header_traits.h"
#include "coreui_i_pin_traits.h"
#include "cpp_non_copyable.h"

namespace vh::ponc::coreui {
///
class INodeTraits : public cpp::NonCopyable {
 public:
  ///
  virtual auto GetLabel() const -> std::string = 0;
  ///
  virtual auto CreateHeaderTraits() const
      -> std::optional<std::unique_ptr<IHeaderTraits>>;
  ///
  virtual auto CreatePinTraits() const
      -> std::vector<std::unique_ptr<IPinTraits>>;
  ///
  virtual auto GetActionNames() const -> std::vector<std::string>;
  ///
  virtual void ExecuteAction(std::string_view action_name);
};
}  // namespace vh::ponc::coreui

#endif  // VH_PONC_COREUI_I_NODE_TRAITS_H_
