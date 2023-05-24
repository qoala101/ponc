/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_COREUI_I_HEADER_TRAITS_H_
#define VH_PONC_COREUI_I_HEADER_TRAITS_H_

#include <imgui.h>

#include <string>

#include "cpp_non_copyable.h"

namespace vh::ponc::coreui {
///
class IHeaderTraits : public cpp::NonCopyable {
 public:
  ///
  virtual auto GetColor() const -> ImColor = 0;
};
}  // namespace vh::ponc::coreui

#endif  // VH_PONC_COREUI_I_HEADER_TRAITS_H_
