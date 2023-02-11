#ifndef VH_ESC_TYPES_H_
#define VH_ESC_TYPES_H_

#include <application.h>
#include <imgui_node_editor.h>

#include "esc_id_generator.h"
#include "imgui.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include <algorithm>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "esc_enums.h"
#include "core_link.h"
#include "core_i_node.h"
#include "core_pin.h"

namespace ne = ax::NodeEditor;

auto CanCreateLink  (const IPin* left, const IPin* right) -> bool;

auto GetCouplerPercentageNames  ()
-> const std::vector<std::string>&;
auto GetCouplerPercentageValues  ()
-> const std::vector<std::pair<float, float>>&;

struct TextureWithDims {
  ImTextureID id{};
  int width{};
  int height{};
};

#endif  // VH_ESC_TYPES_H_
