/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_CORE_AREA_H_
#define VH_PONC_CORE_AREA_H_

#include <imgui.h>
#include <imgui_node_editor.h>

#include <string>

namespace ne = ax::NodeEditor;

namespace vh::ponc::core {
///
using AreaId = ne::NodeId;

///
struct Area {
  ///
  AreaId id{};
  ///
  std::string name{};
  ///
  ImVec2 pos{};
  ///
  ImVec2 size{};
};
}  // namespace vh::ponc::core

#endif  // VH_PONC_CORE_AREA_H_
