#ifndef VH_ESC_ENUMS_H_
#define VH_ESC_ENUMS_H_

#include <imgui.h>

enum class PinType { Flow };

enum class PinKind { Output, Input };

enum class NodeType { Blueprint };

enum class IconType : ImU32 { Flow };

auto GetIconColor(PinType pin_type) -> ImColor;
auto GetIconType(PinType pin_type) -> IconType;

#endif  // VH_ESC_ENUMS_H_
