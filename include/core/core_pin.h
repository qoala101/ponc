#ifndef VH_PONC_CORE_PIN_H_
#define VH_PONC_CORE_PIN_H_

#include <imgui_node_editor.h>

#include "cpp_static_api.h"

namespace ne = ax::NodeEditor;

namespace vh::ponc::core {
//
struct Pin : public cpp::StaticApi {
  ///
  static auto GetOppositeKind(ne::PinKind pin_kind) -> ne::PinKind;
};
}  // namespace vh::ponc::core

#endif  // VH_PONC_CORE_PIN_H_
