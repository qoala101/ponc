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

namespace ax::Drawing {
enum class IconType : ImU32 {
  Flow,
  Circle,
  Square,
  Grid,
  RoundSquare,
  Diamond
};
}  // namespace ax::Drawing

#endif  // VH_ESC_ENUMS_H_
