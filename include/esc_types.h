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

#include "core_client_node.h"
#include "core_coupler_node.h"
#include "esc_enums.h"
#include "core_input_node.h"
#include "core_link.h"
#include "core_node.h"
#include "core_pin.h"
#include "core_splitter_node.h"

namespace ne = ax::NodeEditor;

auto CanCreateLink [[nodiscard]] (const Pin* left, const Pin* right) -> bool;

auto GetCouplerPercentageNames [[nodiscard]] ()
-> const std::vector<std::string>&;
auto GetCouplerPercentageValues [[nodiscard]] ()
-> const std::vector<std::pair<float, float>>&;

struct TextureWithDims {
  ImTextureID id{};
  int width{};
  int height{};
};

#endif  // VH_ESC_TYPES_H_
