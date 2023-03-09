#ifndef VH_COREUI_I_NODE_TRAITS_H_
#define VH_COREUI_I_NODE_TRAITS_H_

#include <memory>
#include <string>
#include <vector>

#include "coreui_i_header_traits.h"
#include "coreui_i_pin_traits.h"
#include "cpp_interface.h"

namespace esc::coreui {
///
class INodeTraits : public cpp::Interface {
 public:
  ///
  virtual auto GetLabel() const -> std::string = 0;
  ///
  virtual auto CreateHeaderTraits() const
      -> std::optional<std::unique_ptr<IHeaderTraits>>;
  ///
  virtual auto CreatePinTraits() const
      -> std::vector<std::unique_ptr<IPinTraits>>;
};
}  // namespace esc::coreui

#endif  // VH_COREUI_I_NODE_TRAITS_H_
