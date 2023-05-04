#ifndef VH_PONC_CORE_ID_PTR_H_
#define VH_PONC_CORE_ID_PTR_H_

#include <imgui_node_editor.h>

#include <variant>

#include "core_id_value.h"

namespace ne = ax::NodeEditor;

namespace vh::ponc::core {
///
struct FamilyId;

///
using IdPtr = std::variant<ne::NodeId *, ne::LinkId *, ne::PinId *, FamilyId *>;

///
auto GetValue(const IdPtr &id) -> UnspecifiedIdValue;
///
void SetValue(const IdPtr &id, UnspecifiedIdValue value);
}  // namespace vh::ponc::core

#endif  // VH_PONC_CORE_ID_PTR_H_
