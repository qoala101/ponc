#include "esc_types.h"

#include <cstdint>

namespace ne = ax::NodeEditor;

Pin::Pin(int id, const char* name, PinType type, PinKind kind, Node* node)
    : ID{static_cast<uint64_t>(id)},
      node{node},
      Name{name},
      Type{type},
      Kind{kind} {}
// vh: ok
auto CanCreateLink(const Pin* left, const Pin* right) -> bool {
  return (left != nullptr) && (right != nullptr) && (left != right) &&
         (left->Kind != right->Kind) && (left->Type == right->Type) &&
         (left->node != right->node);
}

Node::Node(int id, const char* name, ImColor color)
    : ID{static_cast<uint64_t>(id)},
      Name{name},
      Color{color},
      Type{NodeType::Blueprint},
      Size{0, 0} {}

auto GetCouplerPercentageNames() -> const std::vector<std::string>& {
  static auto kNames = std::vector<std::string>{
      "05%%-95%%", "10%%-90%%", "15%%-85%%", "20%%-80%%", "25%%-75%%",
      "30%%-70%%", "35%%-65%%", "40%%-60%%", "45%%-55%%", "50%%-50%%"};
  return kNames;
}

auto GetCouplerPercentageValues()
    -> const std::vector<std::pair<float, float>>& {
  static auto kValues = std::vector<std::pair<float, float>>{
      {13.80, 00.40}, {10.60, 00.70}, {08.80, 00.95}, {07.50, 01.20},
      {06.50, 01.55}, {05.70, 01.85}, {05.00, 02.20}, {04.40, 02.60},
      {03.90, 03.00}, {03.40, 03.40},
  };
  return kValues;
}