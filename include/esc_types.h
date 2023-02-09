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

namespace ne = ax::NodeEditor;

struct Node;

struct Pin {
  Pin(ne::PinId id, std::string name, PinType type, PinKind kind, Node* node);

  ne::PinId ID;
  Node* node;
  std::string Name;
  PinType Type;
  PinKind Kind;

  float value{};
  bool editable{};
};

auto CanCreateLink [[nodiscard]] (const Pin* left, const Pin* right) -> bool;

struct Node {
  Node(ne::NodeId id, std::string name, ImColor color);

  ne::NodeId ID{};
  std::string Name{};
  std::vector<Pin> Inputs{};
  std::vector<Pin> Outputs{};
  ImColor Color{255, 255, 255};
  NodeType Type{NodeType::Blueprint};
  ImVec2 Size{0, 0};

  int coupler_percentage_index_{};
};

struct InputNode : public Node {
  explicit InputNode(esc::IdGenerator& id_generator)
      : Node{id_generator.GetNext<ne::NodeId>(), "Input", {255, 0, 0}} {
    auto& pin = Outputs.emplace_back(id_generator.GetNext<ne::PinId>(), "",
                                     PinType::Flow, PinKind::Output, this);
    pin.editable = true;
  }
};

struct CouplerNode : public Node {
  explicit CouplerNode(esc::IdGenerator& id_generator)
      : Node{id_generator.GetNext<ne::NodeId>(), "Coupler 1x2", {255, 0, 255}} {
    Inputs.emplace_back(id_generator.GetNext<ne::PinId>(), "", PinType::Flow,
                        PinKind::Input, this);
    Inputs.emplace_back(id_generator.GetNext<ne::PinId>(), "", PinType::Float,
                        PinKind::Input, this);
    Inputs.emplace_back(id_generator.GetNext<ne::PinId>(), "", PinType::Float,
                        PinKind::Input, this);

    Outputs.emplace_back(id_generator.GetNext<ne::PinId>(), "", PinType::Empty,
                         PinKind::Output, this);
    Outputs.emplace_back(id_generator.GetNext<ne::PinId>(), "", PinType::Flow,
                         PinKind::Output, this);
    Outputs.emplace_back(id_generator.GetNext<ne::PinId>(), "", PinType::Flow,
                         PinKind::Output, this);
  }
};

struct SplitterNode : public Node {
  explicit SplitterNode(esc::IdGenerator& id_generator, int n)
      : Node{id_generator.GetNext<ne::NodeId>(),
             "Splitter 1x" + std::to_string(n),
             {0, 0, 127 + 128 / n}} {
    Inputs.emplace_back(id_generator.GetNext<ne::PinId>(), "", PinType::Flow,
                        PinKind::Input, this);
    Inputs.emplace_back(id_generator.GetNext<ne::PinId>(), "", PinType::Float,
                        PinKind::Input, this);

    for (auto i = 0; i < n; ++i) {
      Outputs.emplace_back(id_generator.GetNext<ne::PinId>(), "", PinType::Flow,
                           PinKind::Output, this);
    }
  }
};

struct ClientNode : public Node {
  explicit ClientNode(esc::IdGenerator& id_generator)
      : Node{id_generator.GetNext<ne::NodeId>(), "Client", {0, 255, 0}} {
    Inputs.emplace_back(id_generator.GetNext<ne::PinId>(), "In", PinType::Flow,
                        PinKind::Input, this);

    auto* pin = &Inputs.emplace_back(id_generator.GetNext<ne::PinId>(), "min",
                                     PinType::Float, PinKind::Input, this);
    pin->editable = true;
    pin = &Inputs.emplace_back(id_generator.GetNext<ne::PinId>(), "max",
                               PinType::Float, PinKind::Input, this);
    pin->editable = true;
  }
};

auto GetCouplerPercentageNames [[nodiscard]] ()
-> const std::vector<std::string>&;
auto GetCouplerPercentageValues [[nodiscard]] ()
-> const std::vector<std::pair<float, float>>&;

struct Link {
  ne::LinkId ID{};
  ne::PinId StartPinID{};
  ne::PinId EndPinID{};
  ImColor Color{255, 255, 255};
};

struct TextureWithDims {
  ImTextureID id{};
  int width{};
  int height{};
};

#endif  // VH_ESC_TYPES_H_
