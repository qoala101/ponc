#ifndef VH_ESC_ENUMS_H_
#define VH_ESC_ENUMS_H_

#include <imgui.h>

enum class PinType {
  Flow,
  Bool,
  Int,
  Float,
  String,
  Object,
  Function,
  Delegate,
};

enum class PinKind { Output, Input };

enum class NodeType { Blueprint, Simple, Tree, Comment, Houdini };

enum class IconType : ImU32 {
  Flow,
  Circle,
  Square,
  Grid,
  RoundSquare,
  Diamond
};

auto GetIconColor(PinType pin_type) -> ImColor;
auto GetIconType(PinType pin_type) -> IconType;

#endif  // VH_ESC_ENUMS_H_
