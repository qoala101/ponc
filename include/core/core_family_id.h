#ifndef VH_CORE_FAMILY_ID_H_
#define VH_CORE_FAMILY_ID_H_

#include <imgui_node_editor.h>

namespace ne = ax::NodeEditor;

namespace esc::core {
///
struct FamilyId : public ne::Details::SafePointerType<FamilyId> {
  ///
  using SafePointerType::SafePointerType;
};
}  // namespace esc::core

#endif  // VH_CORE_FAMILY_ID_H_
