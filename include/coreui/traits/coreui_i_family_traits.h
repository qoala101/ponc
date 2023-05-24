/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_COREUI_I_FAMILY_TRAITS_H_
#define VH_PONC_COREUI_I_FAMILY_TRAITS_H_

#include <string>

#include "cpp_non_copyable.h"

namespace vh::ponc::coreui {
///
class IFamilyTraits : public cpp::NonCopyable {
 public:
  ///
  virtual auto GetLabel() const -> std::string = 0;
  ///
  virtual auto GetGroupLabel() const -> std::string;
  ///
  virtual auto IsUserAccessible() -> bool;
};
}  // namespace vh::ponc::coreui

#endif  // VH_PONC_COREUI_I_FAMILY_TRAITS_H_
