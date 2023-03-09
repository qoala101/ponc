#ifndef VH_COREUI_I_NODE_HEADER_TRAITS_H_
#define VH_COREUI_I_NODE_HEADER_TRAITS_H_

#include <string>

#include "cpp_interface.h"
#include "imgui.h"

namespace esc::coreui {
///
class IHeaderTraits : public cpp::Interface {
 public:
  ///
  virtual auto GetColor() const -> ImColor = 0;
  ///
  virtual auto GetTextureFilePath() const -> std::string;
};
}  // namespace esc::coreui

#endif  // VH_COREUI_I_NODE_HEADER_TRAITS_H_
