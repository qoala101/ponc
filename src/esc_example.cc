#include "esc_example.h"

#include <application.h>
#include <imgui_node_editor.h>

#include "esc_builders.h"
#include "esc_widgets.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include <algorithm>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace {
auto* kEditor = static_cast<ne::EditorContext*>(nullptr);
const auto kPinIconSize = 24;
const auto kTouchTime = 1.0F;

auto ImGui_GetItemRect() -> ImRect {
  return ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
}

auto ImRect_Expanded(const ImRect& rect, float x, float y) -> ImRect {
  auto result = rect;
  result.Min.x -= x;
  result.Min.y -= y;
  result.Max.x += x;
  result.Max.y += y;
  return result;
}

auto CanCreateLink(Pin* a, Pin* b) -> bool {
  if (!a || !b || a == b || a->Kind == b->Kind || a->Type != b->Type ||
      a->Node == b->Node)
    return false;

  return true;
}

void BuildNode(Node* node) {
  for (auto& input : node->Inputs) {
    input.Node = node;
    input.Kind = PinKind::Input;
  }

  for (auto& output : node->Outputs) {
    output.Node = node;
    output.Kind = PinKind::Output;
  }
}

auto GetIconColor(PinType type) -> ImColor {
  switch (type) {
    default:
    case PinType::Flow:
      return ImColor(255, 255, 255);
    case PinType::Bool:
      return ImColor(220, 48, 48);
    case PinType::Int:
      return ImColor(68, 201, 156);
    case PinType::Float:
      return ImColor(147, 226, 74);
    case PinType::String:
      return ImColor(124, 21, 153);
    case PinType::Object:
      return ImColor(51, 150, 215);
    case PinType::Function:
      return ImColor(218, 0, 183);
    case PinType::Delegate:
      return ImColor(255, 48, 48);
  }
}

void ShowStyleEditor(bool* show) {
  if (!ImGui::Begin("Style", show)) {
    ImGui::End();
    return;
  }

  auto paneWidth = ImGui::GetContentRegionAvail().x;

  auto& editorStyle = ne::GetStyle();
  ImGui::BeginHorizontal("Style buttons", ImVec2(paneWidth, 0), 1.0f);
  ImGui::TextUnformatted("Values");
  ImGui::Spring();
  if (ImGui::Button("Reset to defaults")) editorStyle = ne::Style();
  ImGui::EndHorizontal();
  ImGui::Spacing();
  ImGui::DragFloat4("Node Padding", &editorStyle.NodePadding.x, 0.1f, 0.0f,
                    40.0f);
  ImGui::DragFloat("Node Rounding", &editorStyle.NodeRounding, 0.1f, 0.0f,
                   40.0f);
  ImGui::DragFloat("Node Border Width", &editorStyle.NodeBorderWidth, 0.1f,
                   0.0f, 15.0f);
  ImGui::DragFloat("Hovered Node Border Width",
                   &editorStyle.HoveredNodeBorderWidth, 0.1f, 0.0f, 15.0f);
  ImGui::DragFloat("Selected Node Border Width",
                   &editorStyle.SelectedNodeBorderWidth, 0.1f, 0.0f, 15.0f);
  ImGui::DragFloat("Pin Rounding", &editorStyle.PinRounding, 0.1f, 0.0f, 40.0f);
  ImGui::DragFloat("Pin Border Width", &editorStyle.PinBorderWidth, 0.1f, 0.0f,
                   15.0f);
  ImGui::DragFloat("Link Strength", &editorStyle.LinkStrength, 1.0f, 0.0f,
                   500.0f);
  // ImVec2  SourceDirection;
  // ImVec2  TargetDirection;
  ImGui::DragFloat("Scroll Duration", &editorStyle.ScrollDuration, 0.001f, 0.0f,
                   2.0f);
  ImGui::DragFloat("Flow Marker Distance", &editorStyle.FlowMarkerDistance,
                   1.0f, 1.0f, 200.0f);
  ImGui::DragFloat("Flow Speed", &editorStyle.FlowSpeed, 1.0f, 1.0f, 2000.0f);
  ImGui::DragFloat("Flow Duration", &editorStyle.FlowDuration, 0.001f, 0.0f,
                   5.0f);
  // ImVec2  PivotAlignment;
  // ImVec2  PivotSize;
  // ImVec2  PivotScale;
  // float   PinCorners;
  // float   PinRadius;
  // float   PinArrowSize;
  // float   PinArrowWidth;
  ImGui::DragFloat("Group Rounding", &editorStyle.GroupRounding, 0.1f, 0.0f,
                   40.0f);
  ImGui::DragFloat("Group Border Width", &editorStyle.GroupBorderWidth, 0.1f,
                   0.0f, 15.0f);

  ImGui::Separator();

  static ImGuiColorEditFlags edit_mode = ImGuiColorEditFlags_DisplayRGB;
  ImGui::BeginHorizontal("Color Mode", ImVec2(paneWidth, 0), 1.0f);
  ImGui::TextUnformatted("Filter Colors");
  ImGui::Spring();
  ImGui::RadioButton("RGB", &edit_mode, ImGuiColorEditFlags_DisplayRGB);
  ImGui::Spring(0);
  ImGui::RadioButton("HSV", &edit_mode, ImGuiColorEditFlags_DisplayHSV);
  ImGui::Spring(0);
  ImGui::RadioButton("HEX", &edit_mode, ImGuiColorEditFlags_DisplayHex);
  ImGui::EndHorizontal();

  static ImGuiTextFilter filter;
  filter.Draw("", paneWidth);

  ImGui::Spacing();

  ImGui::PushItemWidth(-160);
  for (int i = 0; i < ne::StyleColor_Count; ++i) {
    auto name = ne::GetStyleColorName((ne::StyleColor)i);
    if (!filter.PassFilter(name)) continue;

    ImGui::ColorEdit4(name, &editorStyle.Colors[i].x, edit_mode);
  }
  ImGui::PopItemWidth();

  ImGui::End();
}

auto Splitter(bool split_vertically, float thickness, float* size1,
              float* size2, float min_size1, float min_size2,
              float splitter_long_axis_size = -1.0f) -> bool {
  using namespace ImGui;
  ImGuiContext& g = *GImGui;
  ImGuiWindow* window = g.CurrentWindow;
  ImGuiID id = window->GetID("##Splitter");
  ImRect bb;
  bb.Min = window->DC.CursorPos +
           (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
  bb.Max =
      bb.Min + CalcItemSize(split_vertically
                                ? ImVec2(thickness, splitter_long_axis_size)
                                : ImVec2(splitter_long_axis_size, thickness),
                            0.0f, 0.0f);
  return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y,
                          size1, size2, min_size1, min_size2, 0.0f);
}
}  // namespace

namespace ne = ax::NodeEditor;
namespace util = ax::NodeEditor::Utilities;

using namespace ax;

using ax::Widgets::IconType;

Example::Example(const char* name, int argc, char** argv)
    : Application{name, argc, argv}, next_id_{1} {}

auto Example::GetNextId() -> int { return next_id_++; }

auto Example::GetNextLinkId() -> ne::LinkId { return ne::LinkId(GetNextId()); }

void Example::TouchNode(ne::NodeId id) { node_touch_time_[id] = kTouchTime; }

auto Example::GetTouchProgress(ne::NodeId id) -> float {
  auto it = node_touch_time_.find(id);
  if (it != node_touch_time_.end() && it->second > 0.0f)
    return (kTouchTime - it->second) / kTouchTime;
  else
    return 0.0f;
}

void Example::UpdateTouch() {
  const auto deltaTime = ImGui::GetIO().DeltaTime;
  for (auto& entry : node_touch_time_) {
    if (entry.second > 0.0f) entry.second -= deltaTime;
  }
}

auto Example::FindNode(ne::NodeId id) -> Node* {
  for (auto& node : nodes_)
    if (node.ID == id) return &node;

  return nullptr;
}

auto Example::FindLink(ne::LinkId id) -> Link* {
  for (auto& link : links_)
    if (link.ID == id) return &link;

  return nullptr;
}

auto Example::FindPin(ne::PinId id) -> Pin* {
  if (!id) return nullptr;

  for (auto& node : nodes_) {
    for (auto& pin : node.Inputs)
      if (pin.ID == id) return &pin;

    for (auto& pin : node.Outputs)
      if (pin.ID == id) return &pin;
  }

  return nullptr;
}

auto Example::IsPinLinked(ne::PinId id) -> bool {
  if (!id) return false;

  for (auto& link : links_)
    if (link.StartPinID == id || link.EndPinID == id) return true;

  return false;
}

auto Example::SpawnInputActionNode() -> Node* {
  nodes_.emplace_back(GetNextId(), "InputAction Fire", ImColor(255, 128, 128));
  nodes_.back().Outputs.emplace_back(GetNextId(), "", PinType::Delegate);
  nodes_.back().Outputs.emplace_back(GetNextId(), "Pressed", PinType::Flow);
  nodes_.back().Outputs.emplace_back(GetNextId(), "Released", PinType::Flow);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto Example::SpawnBranchNode() -> Node* {
  nodes_.emplace_back(GetNextId(), "Branch");
  nodes_.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
  nodes_.back().Inputs.emplace_back(GetNextId(), "Condition", PinType::Bool);
  nodes_.back().Outputs.emplace_back(GetNextId(), "True", PinType::Flow);
  nodes_.back().Outputs.emplace_back(GetNextId(), "False", PinType::Flow);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto Example::SpawnDoNNode() -> Node* {
  nodes_.emplace_back(GetNextId(), "Do N");
  nodes_.back().Inputs.emplace_back(GetNextId(), "Enter", PinType::Flow);
  nodes_.back().Inputs.emplace_back(GetNextId(), "N", PinType::Int);
  nodes_.back().Inputs.emplace_back(GetNextId(), "Reset", PinType::Flow);
  nodes_.back().Outputs.emplace_back(GetNextId(), "Exit", PinType::Flow);
  nodes_.back().Outputs.emplace_back(GetNextId(), "Counter", PinType::Int);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto Example::SpawnOutputActionNode() -> Node* {
  nodes_.emplace_back(GetNextId(), "OutputAction");
  nodes_.back().Inputs.emplace_back(GetNextId(), "Sample", PinType::Float);
  nodes_.back().Outputs.emplace_back(GetNextId(), "Condition", PinType::Bool);
  nodes_.back().Inputs.emplace_back(GetNextId(), "Event", PinType::Delegate);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto Example::SpawnPrintStringNode() -> Node* {
  nodes_.emplace_back(GetNextId(), "Print String");
  nodes_.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
  nodes_.back().Inputs.emplace_back(GetNextId(), "In String", PinType::String);
  nodes_.back().Outputs.emplace_back(GetNextId(), "", PinType::Flow);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto Example::SpawnMessageNode() -> Node* {
  nodes_.emplace_back(GetNextId(), "", ImColor(128, 195, 248));
  nodes_.back().Type = NodeType::Simple;
  nodes_.back().Outputs.emplace_back(GetNextId(), "Message", PinType::String);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto Example::SpawnSetTimerNode() -> Node* {
  nodes_.emplace_back(GetNextId(), "Set Timer", ImColor(128, 195, 248));
  nodes_.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
  nodes_.back().Inputs.emplace_back(GetNextId(), "Object", PinType::Object);
  nodes_.back().Inputs.emplace_back(GetNextId(), "Function Name",
                                    PinType::Function);
  nodes_.back().Inputs.emplace_back(GetNextId(), "Time", PinType::Float);
  nodes_.back().Inputs.emplace_back(GetNextId(), "Looping", PinType::Bool);
  nodes_.back().Outputs.emplace_back(GetNextId(), "", PinType::Flow);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto Example::SpawnLessNode() -> Node* {
  nodes_.emplace_back(GetNextId(), "<", ImColor(128, 195, 248));
  nodes_.back().Type = NodeType::Simple;
  nodes_.back().Inputs.emplace_back(GetNextId(), "", PinType::Float);
  nodes_.back().Inputs.emplace_back(GetNextId(), "", PinType::Float);
  nodes_.back().Outputs.emplace_back(GetNextId(), "", PinType::Float);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto Example::SpawnWeirdNode() -> Node* {
  nodes_.emplace_back(GetNextId(), "o.O", ImColor(128, 195, 248));
  nodes_.back().Type = NodeType::Simple;
  nodes_.back().Inputs.emplace_back(GetNextId(), "", PinType::Float);
  nodes_.back().Outputs.emplace_back(GetNextId(), "", PinType::Float);
  nodes_.back().Outputs.emplace_back(GetNextId(), "", PinType::Float);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto Example::SpawnTraceByChannelNode() -> Node* {
  nodes_.emplace_back(GetNextId(), "Single Line Trace by Channel",
                      ImColor(255, 128, 64));
  nodes_.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
  nodes_.back().Inputs.emplace_back(GetNextId(), "Start", PinType::Flow);
  nodes_.back().Inputs.emplace_back(GetNextId(), "End", PinType::Int);
  nodes_.back().Inputs.emplace_back(GetNextId(), "Trace Channel",
                                    PinType::Float);
  nodes_.back().Inputs.emplace_back(GetNextId(), "Trace Complex",
                                    PinType::Bool);
  nodes_.back().Inputs.emplace_back(GetNextId(), "Actors to Ignore",
                                    PinType::Int);
  nodes_.back().Inputs.emplace_back(GetNextId(), "Draw Debug Type",
                                    PinType::Bool);
  nodes_.back().Inputs.emplace_back(GetNextId(), "Ignore Self", PinType::Bool);
  nodes_.back().Outputs.emplace_back(GetNextId(), "", PinType::Flow);
  nodes_.back().Outputs.emplace_back(GetNextId(), "Out Hit", PinType::Float);
  nodes_.back().Outputs.emplace_back(GetNextId(), "Return Value",
                                     PinType::Bool);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto Example::SpawnTreeSequenceNode() -> Node* {
  nodes_.emplace_back(GetNextId(), "Sequence");
  nodes_.back().Type = NodeType::Tree;
  nodes_.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
  nodes_.back().Outputs.emplace_back(GetNextId(), "", PinType::Flow);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto Example::SpawnTreeTaskNode() -> Node* {
  nodes_.emplace_back(GetNextId(), "Move To");
  nodes_.back().Type = NodeType::Tree;
  nodes_.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto Example::SpawnTreeTask2Node() -> Node* {
  nodes_.emplace_back(GetNextId(), "Random Wait");
  nodes_.back().Type = NodeType::Tree;
  nodes_.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto Example::SpawnComment() -> Node* {
  nodes_.emplace_back(GetNextId(), "Test Comment");
  nodes_.back().Type = NodeType::Comment;
  nodes_.back().Size = ImVec2(300, 200);

  return &nodes_.back();
}

auto Example::SpawnHoudiniTransformNode() -> Node* {
  nodes_.emplace_back(GetNextId(), "Transform");
  nodes_.back().Type = NodeType::Houdini;
  nodes_.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
  nodes_.back().Outputs.emplace_back(GetNextId(), "", PinType::Flow);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto Example::SpawnHoudiniGroupNode() -> Node* {
  nodes_.emplace_back(GetNextId(), "Group");
  nodes_.back().Type = NodeType::Houdini;
  nodes_.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
  nodes_.back().Inputs.emplace_back(GetNextId(), "", PinType::Flow);
  nodes_.back().Outputs.emplace_back(GetNextId(), "", PinType::Flow);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

void Example::BuildNodes() {
  for (auto& node : nodes_) {
    BuildNode(&node);
  }
}

void Example::OnStart() {
  ne::Config config;

  config.SettingsFile = "Blueprints.json";

  config.UserPointer = this;

  config.LoadNodeSettings = [](ne::NodeId nodeId, char* data,
                               void* userPointer) -> size_t {
    auto self = static_cast<Example*>(userPointer);

    auto node = self->FindNode(nodeId);
    if (!node) return 0;

    if (data != nullptr) memcpy(data, node->State.data(), node->State.size());
    return node->State.size();
  };

  config.SaveNodeSettings = [](ne::NodeId nodeId, const char* data, size_t size,
                               ne::SaveReasonFlags reason,
                               void* userPointer) -> bool {
    auto self = static_cast<Example*>(userPointer);

    auto node = self->FindNode(nodeId);
    if (!node) return false;

    node->State.assign(data, size);

    self->TouchNode(nodeId);

    return true;
  };

  kEditor = ne::CreateEditor(&config);
  ne::SetCurrentEditor(kEditor);

  Node* node;
  node = SpawnInputActionNode();
  ne::SetNodePosition(node->ID, ImVec2(-252, 220));
  node = SpawnBranchNode();
  ne::SetNodePosition(node->ID, ImVec2(-300, 351));
  node = SpawnDoNNode();
  ne::SetNodePosition(node->ID, ImVec2(-238, 504));
  node = SpawnOutputActionNode();
  ne::SetNodePosition(node->ID, ImVec2(71, 80));
  node = SpawnSetTimerNode();
  ne::SetNodePosition(node->ID, ImVec2(168, 316));

  node = SpawnTreeSequenceNode();
  ne::SetNodePosition(node->ID, ImVec2(1028, 329));
  node = SpawnTreeTaskNode();
  ne::SetNodePosition(node->ID, ImVec2(1204, 458));
  node = SpawnTreeTask2Node();
  ne::SetNodePosition(node->ID, ImVec2(868, 538));

  node = SpawnComment();
  ne::SetNodePosition(node->ID, ImVec2(112, 576));
  ne::SetGroupSize(node->ID, ImVec2(384, 154));
  node = SpawnComment();
  ne::SetNodePosition(node->ID, ImVec2(800, 224));
  ne::SetGroupSize(node->ID, ImVec2(640, 400));

  node = SpawnLessNode();
  ne::SetNodePosition(node->ID, ImVec2(366, 652));
  node = SpawnWeirdNode();
  ne::SetNodePosition(node->ID, ImVec2(144, 652));
  node = SpawnMessageNode();
  ne::SetNodePosition(node->ID, ImVec2(-348, 698));
  node = SpawnPrintStringNode();
  ne::SetNodePosition(node->ID, ImVec2(-69, 652));

  node = SpawnHoudiniTransformNode();
  ne::SetNodePosition(node->ID, ImVec2(500, -70));
  node = SpawnHoudiniGroupNode();
  ne::SetNodePosition(node->ID, ImVec2(500, 42));

  ne::NavigateToContent();

  BuildNodes();

  links_.push_back(
      Link(GetNextLinkId(), nodes_[5].Outputs[0].ID, nodes_[6].Inputs[0].ID));
  links_.push_back(
      Link(GetNextLinkId(), nodes_[5].Outputs[0].ID, nodes_[7].Inputs[0].ID));

  links_.push_back(
      Link(GetNextLinkId(), nodes_[14].Outputs[0].ID, nodes_[15].Inputs[0].ID));

  header_background_ = LoadTexture("data/BlueprintBackground.png");
  save_icon_ = LoadTexture("data/ic_save_white_24dp.png");
  restore_icon_ = LoadTexture("data/ic_restore_white_24dp.png");

  // auto& io = ImGui::GetIO();
}

void Example::OnStop() {
  auto releaseTexture = [this](ImTextureID& id) {
    if (id) {
      DestroyTexture(id);
      id = nullptr;
    }
  };

  releaseTexture(restore_icon_);
  releaseTexture(save_icon_);
  releaseTexture(header_background_);

  if (kEditor) {
    ne::DestroyEditor(kEditor);
    kEditor = nullptr;
  }
}

void Example::DrawPinIcon(const Pin& pin, bool connected, int alpha) const {
  IconType iconType;
  ImColor color = GetIconColor(pin.Type);
  color.Value.w = alpha / 255.0f;
  switch (pin.Type) {
    case PinType::Flow:
      iconType = IconType::Flow;
      break;
    case PinType::Bool:
      iconType = IconType::Circle;
      break;
    case PinType::Int:
      iconType = IconType::Circle;
      break;
    case PinType::Float:
      iconType = IconType::Circle;
      break;
    case PinType::String:
      iconType = IconType::Circle;
      break;
    case PinType::Object:
      iconType = IconType::Circle;
      break;
    case PinType::Function:
      iconType = IconType::Circle;
      break;
    case PinType::Delegate:
      iconType = IconType::Square;
      break;
    default:
      return;
  }

  ax::Widgets::Icon(ImVec2(static_cast<float>(kPinIconSize),
                           static_cast<float>(kPinIconSize)),
                    iconType, connected, color, ImColor(32, 32, 32, alpha));
}

void Example::ShowLeftPane(float paneWidth) {
  auto& io = ImGui::GetIO();

  ImGui::BeginChild("Selection", ImVec2(paneWidth, 0));

  paneWidth = ImGui::GetContentRegionAvail().x;

  static bool showStyleEditor = false;
  ImGui::BeginHorizontal("Style Editor", ImVec2(paneWidth, 0));
  ImGui::Spring(0.0f, 0.0f);
  if (ImGui::Button("Zoom to Content")) ne::NavigateToContent();
  ImGui::Spring(0.0f);
  if (ImGui::Button("Show Flow")) {
    for (auto& link : links_) ne::Flow(link.ID);
  }
  ImGui::Spring();
  if (ImGui::Button("Edit Style")) showStyleEditor = true;
  ImGui::EndHorizontal();
  ImGui::Checkbox("Show Ordinals", &show_ordinals_);

  if (showStyleEditor) ShowStyleEditor(&showStyleEditor);

  std::vector<ne::NodeId> selectedNodes;
  std::vector<ne::LinkId> selectedLinks;
  selectedNodes.resize(ne::GetSelectedObjectCount());
  selectedLinks.resize(ne::GetSelectedObjectCount());

  int nodeCount = ne::GetSelectedNodes(selectedNodes.data(),
                                       static_cast<int>(selectedNodes.size()));
  int linkCount = ne::GetSelectedLinks(selectedLinks.data(),
                                       static_cast<int>(selectedLinks.size()));

  selectedNodes.resize(nodeCount);
  selectedLinks.resize(linkCount);

  int saveIconWidth = GetTextureWidth(save_icon_);
  int saveIconHeight = GetTextureWidth(save_icon_);
  int restoreIconWidth = GetTextureWidth(restore_icon_);
  int restoreIconHeight = GetTextureWidth(restore_icon_);

  ImGui::GetWindowDrawList()->AddRectFilled(
      ImGui::GetCursorScreenPos(),
      ImGui::GetCursorScreenPos() +
          ImVec2(paneWidth, ImGui::GetTextLineHeight()),
      ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]),
      ImGui::GetTextLineHeight() * 0.25f);
  ImGui::Spacing();
  ImGui::SameLine();
  ImGui::TextUnformatted("Nodes");
  ImGui::Indent();
  for (auto& node : nodes_) {
    ImGui::PushID(node.ID.AsPointer());
    auto start = ImGui::GetCursorScreenPos();

    if (const auto progress = GetTouchProgress(node.ID)) {
      ImGui::GetWindowDrawList()->AddLine(
          start + ImVec2(-8, 0), start + ImVec2(-8, ImGui::GetTextLineHeight()),
          IM_COL32(255, 0, 0, 255 - (int)(255 * progress)), 4.0f);
    }

    bool isSelected = std::find(selectedNodes.begin(), selectedNodes.end(),
                                node.ID) != selectedNodes.end();
    if (ImGui::Selectable(
            (node.Name + "##" +
             std::to_string(reinterpret_cast<uintptr_t>(node.ID.AsPointer())))
                .c_str(),
            &isSelected)) {
      if (io.KeyCtrl) {
        if (isSelected)
          ne::SelectNode(node.ID, true);
        else
          ne::DeselectNode(node.ID);
      } else
        ne::SelectNode(node.ID, false);

      ne::NavigateToSelection();
    }
    if (ImGui::IsItemHovered() && !node.State.empty())
      ImGui::SetTooltip("State: %s", node.State.c_str());

    auto id = std::string("(") +
              std::to_string(reinterpret_cast<uintptr_t>(node.ID.AsPointer())) +
              ")";
    auto textSize = ImGui::CalcTextSize(id.c_str(), nullptr);
    auto iconPanelPos =
        start +
        ImVec2(paneWidth - ImGui::GetStyle().FramePadding.x -
                   ImGui::GetStyle().IndentSpacing - saveIconWidth -
                   restoreIconWidth - ImGui::GetStyle().ItemInnerSpacing.x * 1,
               (ImGui::GetTextLineHeight() - saveIconHeight) / 2);
    ImGui::GetWindowDrawList()->AddText(
        ImVec2(
            iconPanelPos.x - textSize.x - ImGui::GetStyle().ItemInnerSpacing.x,
            start.y),
        IM_COL32(255, 255, 255, 255), id.c_str(), nullptr);

    auto drawList = ImGui::GetWindowDrawList();
    ImGui::SetCursorScreenPos(iconPanelPos);
    ImGui::SetItemAllowOverlap();
    if (node.SavedState.empty()) {
      if (ImGui::InvisibleButton(
              "save", ImVec2((float)saveIconWidth, (float)saveIconHeight)))
        node.SavedState = node.State;

      if (ImGui::IsItemActive())
        drawList->AddImage(save_icon_, ImGui::GetItemRectMin(),
                           ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1),
                           IM_COL32(255, 255, 255, 96));
      else if (ImGui::IsItemHovered())
        drawList->AddImage(save_icon_, ImGui::GetItemRectMin(),
                           ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1),
                           IM_COL32(255, 255, 255, 255));
      else
        drawList->AddImage(save_icon_, ImGui::GetItemRectMin(),
                           ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1),
                           IM_COL32(255, 255, 255, 160));
    } else {
      ImGui::Dummy(ImVec2((float)saveIconWidth, (float)saveIconHeight));
      drawList->AddImage(save_icon_, ImGui::GetItemRectMin(),
                         ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1),
                         IM_COL32(255, 255, 255, 32));
    }

    ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
    ImGui::SetItemAllowOverlap();
    if (!node.SavedState.empty()) {
      if (ImGui::InvisibleButton("restore", ImVec2((float)restoreIconWidth,
                                                   (float)restoreIconHeight))) {
        node.State = node.SavedState;
        ne::RestoreNodeState(node.ID);
        node.SavedState.clear();
      }

      if (ImGui::IsItemActive())
        drawList->AddImage(restore_icon_, ImGui::GetItemRectMin(),
                           ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1),
                           IM_COL32(255, 255, 255, 96));
      else if (ImGui::IsItemHovered())
        drawList->AddImage(restore_icon_, ImGui::GetItemRectMin(),
                           ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1),
                           IM_COL32(255, 255, 255, 255));
      else
        drawList->AddImage(restore_icon_, ImGui::GetItemRectMin(),
                           ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1),
                           IM_COL32(255, 255, 255, 160));
    } else {
      ImGui::Dummy(ImVec2((float)restoreIconWidth, (float)restoreIconHeight));
      drawList->AddImage(restore_icon_, ImGui::GetItemRectMin(),
                         ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1),
                         IM_COL32(255, 255, 255, 32));
    }

    ImGui::SameLine(0, 0);
    ImGui::SetItemAllowOverlap();
    ImGui::Dummy(ImVec2(0, (float)restoreIconHeight));

    ImGui::PopID();
  }
  ImGui::Unindent();

  static int changeCount = 0;

  ImGui::GetWindowDrawList()->AddRectFilled(
      ImGui::GetCursorScreenPos(),
      ImGui::GetCursorScreenPos() +
          ImVec2(paneWidth, ImGui::GetTextLineHeight()),
      ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]),
      ImGui::GetTextLineHeight() * 0.25f);
  ImGui::Spacing();
  ImGui::SameLine();
  ImGui::TextUnformatted("Selection");

  ImGui::BeginHorizontal("Selection Stats", ImVec2(paneWidth, 0));
  ImGui::Text("Changed %d time%s", changeCount, changeCount > 1 ? "s" : "");
  ImGui::Spring();
  if (ImGui::Button("Deselect All")) ne::ClearSelection();
  ImGui::EndHorizontal();
  ImGui::Indent();
  for (int i = 0; i < nodeCount; ++i)
    ImGui::Text("Node (%p)", selectedNodes[i].AsPointer());
  for (int i = 0; i < linkCount; ++i)
    ImGui::Text("Link (%p)", selectedLinks[i].AsPointer());
  ImGui::Unindent();

  if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Z)))
    for (auto& link : links_) ne::Flow(link.ID);

  if (ne::HasSelectionChanged()) ++changeCount;

  ImGui::EndChild();
}

void Example::OnFrame(float deltaTime) {
  UpdateTouch();

  auto& io = ImGui::GetIO();

  ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate,
              io.Framerate ? 1000.0f / io.Framerate : 0.0f);

  ne::SetCurrentEditor(kEditor);

  // auto& style = ImGui::GetStyle();

  // {
  //     for (auto x = -io.DisplaySize.y; x < io.DisplaySize.x; x += 10.0f)
  //     {
  //         ImGui::GetWindowDrawList()->AddLine(ImVec2(x, 0), ImVec2(x +
  //         io.DisplaySize.y, io.DisplaySize.y),
  //             IM_COL32(255, 255, 0, 255));
  //     }
  // }

  static ne::NodeId contextNodeId = 0;
  static ne::LinkId contextLinkId = 0;
  static ne::PinId contextPinId = 0;
  static bool createNewNode = false;
  static Pin* newNodeLinkPin = nullptr;
  static Pin* newLinkPin = nullptr;

  static float leftPaneWidth = 400.0f;
  static float rightPaneWidth = 800.0f;
  Splitter(true, 4.0f, &leftPaneWidth, &rightPaneWidth, 50.0f, 50.0f);

  ShowLeftPane(leftPaneWidth - 4.0f);

  ImGui::SameLine(0.0f, 12.0f);

  ne::Begin("Node editor");
  {
    auto cursorTopLeft = ImGui::GetCursorScreenPos();

    util::BlueprintNodeBuilder builder(header_background_,
                                       GetTextureWidth(header_background_),
                                       GetTextureHeight(header_background_));

    for (auto& node : nodes_) {
      if (node.Type != NodeType::Blueprint && node.Type != NodeType::Simple)
        continue;

      const auto isSimple = node.Type == NodeType::Simple;

      bool hasOutputDelegates = false;
      for (auto& output : node.Outputs)
        if (output.Type == PinType::Delegate) hasOutputDelegates = true;

      builder.Begin(node.ID);
      if (!isSimple) {
        builder.Header(node.Color);
        ImGui::Spring(0);
        ImGui::TextUnformatted(node.Name.c_str());
        ImGui::Spring(1);
        ImGui::Dummy(ImVec2(0, 28));
        if (hasOutputDelegates) {
          ImGui::BeginVertical("delegates", ImVec2(0, 28));
          ImGui::Spring(1, 0);
          for (auto& output : node.Outputs) {
            if (output.Type != PinType::Delegate) continue;

            auto alpha = ImGui::GetStyle().Alpha;
            if (newLinkPin && !CanCreateLink(newLinkPin, &output) &&
                &output != newLinkPin)
              alpha = alpha * (48.0f / 255.0f);

            ne::BeginPin(output.ID, ne::PinKind::Output);
            ne::PinPivotAlignment(ImVec2(1.0f, 0.5f));
            ne::PinPivotSize(ImVec2(0, 0));
            ImGui::BeginHorizontal(output.ID.AsPointer());
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
            if (!output.Name.empty()) {
              ImGui::TextUnformatted(output.Name.c_str());
              ImGui::Spring(0);
            }
            DrawPinIcon(output, IsPinLinked(output.ID), (int)(alpha * 255));
            ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.x / 2);
            ImGui::EndHorizontal();
            ImGui::PopStyleVar();
            ne::EndPin();

            // DrawItemRect(ImColor(255, 0, 0));
          }
          ImGui::Spring(1, 0);
          ImGui::EndVertical();
          ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.x / 2);
        } else
          ImGui::Spring(0);
        builder.EndHeader();
      }

      for (auto& input : node.Inputs) {
        auto alpha = ImGui::GetStyle().Alpha;
        if (newLinkPin && !CanCreateLink(newLinkPin, &input) &&
            &input != newLinkPin)
          alpha = alpha * (48.0f / 255.0f);

        builder.Input(input.ID);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
        DrawPinIcon(input, IsPinLinked(input.ID), (int)(alpha * 255));
        ImGui::Spring(0);
        if (!input.Name.empty()) {
          ImGui::TextUnformatted(input.Name.c_str());
          ImGui::Spring(0);
        }
        if (input.Type == PinType::Bool) {
          ImGui::Button("Hello");
          ImGui::Spring(0);
        }
        ImGui::PopStyleVar();
        builder.EndInput();
      }

      if (isSimple) {
        builder.Middle();

        ImGui::Spring(1, 0);
        ImGui::TextUnformatted(node.Name.c_str());
        ImGui::Spring(1, 0);
      }

      for (auto& output : node.Outputs) {
        if (!isSimple && output.Type == PinType::Delegate) continue;

        auto alpha = ImGui::GetStyle().Alpha;
        if (newLinkPin && !CanCreateLink(newLinkPin, &output) &&
            &output != newLinkPin)
          alpha = alpha * (48.0f / 255.0f);

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
        builder.Output(output.ID);
        if (output.Type == PinType::String) {
          static char buffer[128] = "Edit Me\nMultiline!";
          static bool wasActive = false;

          ImGui::PushItemWidth(100.0f);
          ImGui::InputText("##edit", buffer, 127);
          ImGui::PopItemWidth();
          if (ImGui::IsItemActive() && !wasActive) {
            ne::EnableShortcuts(false);
            wasActive = true;
          } else if (!ImGui::IsItemActive() && wasActive) {
            ne::EnableShortcuts(true);
            wasActive = false;
          }
          ImGui::Spring(0);
        }
        if (!output.Name.empty()) {
          ImGui::Spring(0);
          ImGui::TextUnformatted(output.Name.c_str());
        }
        ImGui::Spring(0);
        DrawPinIcon(output, IsPinLinked(output.ID), (int)(alpha * 255));
        ImGui::PopStyleVar();
        builder.EndOutput();
      }

      builder.End();
    }

    for (auto& node : nodes_) {
      if (node.Type != NodeType::Tree) continue;

      const float rounding = 5.0f;
      const float padding = 12.0f;

      const auto pinBackground = ne::GetStyle().Colors[ne::StyleColor_NodeBg];

      ne::PushStyleColor(ne::StyleColor_NodeBg, ImColor(128, 128, 128, 200));
      ne::PushStyleColor(ne::StyleColor_NodeBorder, ImColor(32, 32, 32, 200));
      ne::PushStyleColor(ne::StyleColor_PinRect, ImColor(60, 180, 255, 150));
      ne::PushStyleColor(ne::StyleColor_PinRectBorder,
                         ImColor(60, 180, 255, 150));

      ne::PushStyleVar(ne::StyleVar_NodePadding, ImVec4(0, 0, 0, 0));
      ne::PushStyleVar(ne::StyleVar_NodeRounding, rounding);
      ne::PushStyleVar(ne::StyleVar_SourceDirection, ImVec2(0.0f, 1.0f));
      ne::PushStyleVar(ne::StyleVar_TargetDirection, ImVec2(0.0f, -1.0f));
      ne::PushStyleVar(ne::StyleVar_LinkStrength, 0.0f);
      ne::PushStyleVar(ne::StyleVar_PinBorderWidth, 1.0f);
      ne::PushStyleVar(ne::StyleVar_PinRadius, 5.0f);
      ne::BeginNode(node.ID);

      ImGui::BeginVertical(node.ID.AsPointer());
      ImGui::BeginHorizontal("inputs");
      ImGui::Spring(0, padding * 2);

      ImRect inputsRect;
      int inputAlpha = 200;
      if (!node.Inputs.empty()) {
        auto& pin = node.Inputs[0];
        ImGui::Dummy(ImVec2(0, padding));
        ImGui::Spring(1, 0);
        inputsRect = ImGui_GetItemRect();

        ne::PushStyleVar(ne::StyleVar_PinArrowSize, 10.0f);
        ne::PushStyleVar(ne::StyleVar_PinArrowWidth, 10.0f);

        ne::PushStyleVar(ne::StyleVar_PinCorners,
                         ImDrawFlags_RoundCornersBottom);

        ne::BeginPin(pin.ID, ne::PinKind::Input);
        ne::PinPivotRect(inputsRect.GetTL(), inputsRect.GetBR());
        ne::PinRect(inputsRect.GetTL(), inputsRect.GetBR());
        ne::EndPin();
        ne::PopStyleVar(3);

        if (newLinkPin && !CanCreateLink(newLinkPin, &pin) &&
            &pin != newLinkPin)
          inputAlpha = (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
      } else
        ImGui::Dummy(ImVec2(0, padding));

      ImGui::Spring(0, padding * 2);
      ImGui::EndHorizontal();

      ImGui::BeginHorizontal("content_frame");
      ImGui::Spring(1, padding);

      ImGui::BeginVertical("content", ImVec2(0.0f, 0.0f));
      ImGui::Dummy(ImVec2(160, 0));
      ImGui::Spring(1);
      ImGui::TextUnformatted(node.Name.c_str());
      ImGui::Spring(1);
      ImGui::EndVertical();
      auto contentRect = ImGui_GetItemRect();

      ImGui::Spring(1, padding);
      ImGui::EndHorizontal();

      ImGui::BeginHorizontal("outputs");
      ImGui::Spring(0, padding * 2);

      ImRect outputsRect;
      int outputAlpha = 200;
      if (!node.Outputs.empty()) {
        auto& pin = node.Outputs[0];
        ImGui::Dummy(ImVec2(0, padding));
        ImGui::Spring(1, 0);
        outputsRect = ImGui_GetItemRect();

        ne::PushStyleVar(ne::StyleVar_PinCorners, ImDrawFlags_RoundCornersTop);

        ne::BeginPin(pin.ID, ne::PinKind::Output);
        ne::PinPivotRect(outputsRect.GetTL(), outputsRect.GetBR());
        ne::PinRect(outputsRect.GetTL(), outputsRect.GetBR());
        ne::EndPin();
        ne::PopStyleVar();

        if (newLinkPin && !CanCreateLink(newLinkPin, &pin) &&
            &pin != newLinkPin)
          outputAlpha = (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
      } else
        ImGui::Dummy(ImVec2(0, padding));

      ImGui::Spring(0, padding * 2);
      ImGui::EndHorizontal();

      ImGui::EndVertical();

      ne::EndNode();
      ne::PopStyleVar(7);
      ne::PopStyleColor(4);

      auto drawList = ne::GetNodeBackgroundDrawList(node.ID);

      // const auto fringeScale = ImGui::GetStyle().AntiAliasFringeScale;
      // const auto unitSize    = 1.0f / fringeScale;

      // const auto ImDrawList_AddRect = [](ImDrawList* drawList, const
      // ImVec2& a, const ImVec2& b, ImU32 col, float rounding, int
      // rounding_corners, float thickness)
      //{
      //     if ((col >> 24) == 0)
      //         return;
      //     drawList->PathRect(a, b, rounding, rounding_corners);
      //     drawList->PathStroke(col, true, thickness);
      // };

      const auto topRoundCornersFlags = ImDrawFlags_RoundCornersTop;
      const auto bottomRoundCornersFlags = ImDrawFlags_RoundCornersBottom;

      drawList->AddRectFilled(
          inputsRect.GetTL() + ImVec2(0, 1), inputsRect.GetBR(),
          IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y),
                   (int)(255 * pinBackground.z), inputAlpha),
          4.0f, bottomRoundCornersFlags);
      // ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
      drawList->AddRect(
          inputsRect.GetTL() + ImVec2(0, 1), inputsRect.GetBR(),
          IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y),
                   (int)(255 * pinBackground.z), inputAlpha),
          4.0f, bottomRoundCornersFlags);
      // ImGui::PopStyleVar();
      drawList->AddRectFilled(
          outputsRect.GetTL(), outputsRect.GetBR() - ImVec2(0, 1),
          IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y),
                   (int)(255 * pinBackground.z), outputAlpha),
          4.0f, topRoundCornersFlags);
      // ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
      drawList->AddRect(
          outputsRect.GetTL(), outputsRect.GetBR() - ImVec2(0, 1),
          IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y),
                   (int)(255 * pinBackground.z), outputAlpha),
          4.0f, topRoundCornersFlags);
      // ImGui::PopStyleVar();
      drawList->AddRectFilled(contentRect.GetTL(), contentRect.GetBR(),
                              IM_COL32(24, 64, 128, 200), 0.0f);
      // ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
      drawList->AddRect(contentRect.GetTL(), contentRect.GetBR(),
                        IM_COL32(48, 128, 255, 100), 0.0f);
      // ImGui::PopStyleVar();
    }

    for (auto& node : nodes_) {
      if (node.Type != NodeType::Houdini) continue;

      const float rounding = 10.0f;
      const float padding = 12.0f;

      ne::PushStyleColor(ne::StyleColor_NodeBg, ImColor(229, 229, 229, 200));
      ne::PushStyleColor(ne::StyleColor_NodeBorder,
                         ImColor(125, 125, 125, 200));
      ne::PushStyleColor(ne::StyleColor_PinRect, ImColor(229, 229, 229, 60));
      ne::PushStyleColor(ne::StyleColor_PinRectBorder,
                         ImColor(125, 125, 125, 60));

      const auto pinBackground = ne::GetStyle().Colors[ne::StyleColor_NodeBg];

      ne::PushStyleVar(ne::StyleVar_NodePadding, ImVec4(0, 0, 0, 0));
      ne::PushStyleVar(ne::StyleVar_NodeRounding, rounding);
      ne::PushStyleVar(ne::StyleVar_SourceDirection, ImVec2(0.0f, 1.0f));
      ne::PushStyleVar(ne::StyleVar_TargetDirection, ImVec2(0.0f, -1.0f));
      ne::PushStyleVar(ne::StyleVar_LinkStrength, 0.0f);
      ne::PushStyleVar(ne::StyleVar_PinBorderWidth, 1.0f);
      ne::PushStyleVar(ne::StyleVar_PinRadius, 6.0f);
      ne::BeginNode(node.ID);

      ImGui::BeginVertical(node.ID.AsPointer());
      if (!node.Inputs.empty()) {
        ImGui::BeginHorizontal("inputs");
        ImGui::Spring(1, 0);

        ImRect inputsRect;
        int inputAlpha = 200;
        for (auto& pin : node.Inputs) {
          ImGui::Dummy(ImVec2(padding, padding));
          inputsRect = ImGui_GetItemRect();
          ImGui::Spring(1, 0);
          inputsRect.Min.y -= padding;
          inputsRect.Max.y -= padding;

          const auto allRoundCornersFlags = ImDrawFlags_RoundCornersAll;

          // ed::PushStyleVar(ed::StyleVar_PinArrowSize, 10.0f);
          // ed::PushStyleVar(ed::StyleVar_PinArrowWidth, 10.0f);
          ne::PushStyleVar(ne::StyleVar_PinCorners, allRoundCornersFlags);

          ne::BeginPin(pin.ID, ne::PinKind::Input);
          ne::PinPivotRect(inputsRect.GetCenter(), inputsRect.GetCenter());
          ne::PinRect(inputsRect.GetTL(), inputsRect.GetBR());
          ne::EndPin();
          // ed::PopStyleVar(3);
          ne::PopStyleVar(1);

          auto drawList = ImGui::GetWindowDrawList();
          drawList->AddRectFilled(
              inputsRect.GetTL(), inputsRect.GetBR(),
              IM_COL32((int)(255 * pinBackground.x),
                       (int)(255 * pinBackground.y),
                       (int)(255 * pinBackground.z), inputAlpha),
              4.0f, allRoundCornersFlags);
          drawList->AddRect(inputsRect.GetTL(), inputsRect.GetBR(),
                            IM_COL32((int)(255 * pinBackground.x),
                                     (int)(255 * pinBackground.y),
                                     (int)(255 * pinBackground.z), inputAlpha),
                            4.0f, allRoundCornersFlags);

          if (newLinkPin && !CanCreateLink(newLinkPin, &pin) &&
              &pin != newLinkPin)
            inputAlpha =
                (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
        }

        // ImGui::Spring(1, 0);
        ImGui::EndHorizontal();
      }

      ImGui::BeginHorizontal("content_frame");
      ImGui::Spring(1, padding);

      ImGui::BeginVertical("content", ImVec2(0.0f, 0.0f));
      ImGui::Dummy(ImVec2(160, 0));
      ImGui::Spring(1);
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
      ImGui::TextUnformatted(node.Name.c_str());
      ImGui::PopStyleColor();
      ImGui::Spring(1);
      ImGui::EndVertical();
      auto contentRect = ImGui_GetItemRect();

      ImGui::Spring(1, padding);
      ImGui::EndHorizontal();

      if (!node.Outputs.empty()) {
        ImGui::BeginHorizontal("outputs");
        ImGui::Spring(1, 0);

        ImRect outputsRect;
        int outputAlpha = 200;
        for (auto& pin : node.Outputs) {
          ImGui::Dummy(ImVec2(padding, padding));
          outputsRect = ImGui_GetItemRect();
          ImGui::Spring(1, 0);
          outputsRect.Min.y += padding;
          outputsRect.Max.y += padding;

          const auto allRoundCornersFlags = ImDrawFlags_RoundCornersAll;
          const auto topRoundCornersFlags = ImDrawFlags_RoundCornersTop;

          ne::PushStyleVar(ne::StyleVar_PinCorners, topRoundCornersFlags);
          ne::BeginPin(pin.ID, ne::PinKind::Output);
          ne::PinPivotRect(outputsRect.GetCenter(), outputsRect.GetCenter());
          ne::PinRect(outputsRect.GetTL(), outputsRect.GetBR());
          ne::EndPin();
          ne::PopStyleVar();

          auto drawList = ImGui::GetWindowDrawList();
          drawList->AddRectFilled(
              outputsRect.GetTL(), outputsRect.GetBR(),
              IM_COL32((int)(255 * pinBackground.x),
                       (int)(255 * pinBackground.y),
                       (int)(255 * pinBackground.z), outputAlpha),
              4.0f, allRoundCornersFlags);
          drawList->AddRect(outputsRect.GetTL(), outputsRect.GetBR(),
                            IM_COL32((int)(255 * pinBackground.x),
                                     (int)(255 * pinBackground.y),
                                     (int)(255 * pinBackground.z), outputAlpha),
                            4.0f, allRoundCornersFlags);

          if (newLinkPin && !CanCreateLink(newLinkPin, &pin) &&
              &pin != newLinkPin)
            outputAlpha =
                (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
        }

        ImGui::EndHorizontal();
      }

      ImGui::EndVertical();

      ne::EndNode();
      ne::PopStyleVar(7);
      ne::PopStyleColor(4);

      // auto drawList = ed::GetNodeBackgroundDrawList(node.ID);

      // const auto fringeScale = ImGui::GetStyle().AntiAliasFringeScale;
      // const auto unitSize    = 1.0f / fringeScale;

      // const auto ImDrawList_AddRect = [](ImDrawList* drawList, const
      // ImVec2& a, const ImVec2& b, ImU32 col, float rounding, int
      // rounding_corners, float thickness)
      //{
      //     if ((col >> 24) == 0)
      //         return;
      //     drawList->PathRect(a, b, rounding, rounding_corners);
      //     drawList->PathStroke(col, true, thickness);
      // };

      // drawList->AddRectFilled(inputsRect.GetTL() + ImVec2(0, 1),
      // inputsRect.GetBR(),
      //     IM_COL32((int)(255 * pinBackground.x), (int)(255 *
      //     pinBackground.y), (int)(255 * pinBackground.z),
      //     inputAlpha), 4.0f, 12);
      // ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
      // drawList->AddRect(inputsRect.GetTL() + ImVec2(0, 1),
      // inputsRect.GetBR(),
      //     IM_COL32((int)(255 * pinBackground.x), (int)(255 *
      //     pinBackground.y), (int)(255 * pinBackground.z),
      //     inputAlpha), 4.0f, 12);
      // ImGui::PopStyleVar();
      // drawList->AddRectFilled(outputsRect.GetTL(), outputsRect.GetBR() -
      // ImVec2(0, 1),
      //     IM_COL32((int)(255 * pinBackground.x), (int)(255 *
      //     pinBackground.y), (int)(255 * pinBackground.z),
      //     outputAlpha), 4.0f, 3);
      ////ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
      // drawList->AddRect(outputsRect.GetTL(), outputsRect.GetBR() -
      // ImVec2(0, 1),
      //     IM_COL32((int)(255 * pinBackground.x), (int)(255 *
      //     pinBackground.y), (int)(255 * pinBackground.z),
      //     outputAlpha), 4.0f, 3);
      ////ImGui::PopStyleVar();
      // drawList->AddRectFilled(contentRect.GetTL(), contentRect.GetBR(),
      // IM_COL32(24, 64, 128, 200), 0.0f);
      // ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
      // drawList->AddRect(
      //     contentRect.GetTL(),
      //     contentRect.GetBR(),
      //     IM_COL32(48, 128, 255, 100), 0.0f);
      // ImGui::PopStyleVar();
    }

    for (auto& node : nodes_) {
      if (node.Type != NodeType::Comment) continue;

      const float commentAlpha = 0.75f;

      ImGui::PushStyleVar(ImGuiStyleVar_Alpha, commentAlpha);
      ne::PushStyleColor(ne::StyleColor_NodeBg, ImColor(255, 255, 255, 64));
      ne::PushStyleColor(ne::StyleColor_NodeBorder, ImColor(255, 255, 255, 64));
      ne::BeginNode(node.ID);
      ImGui::PushID(node.ID.AsPointer());
      ImGui::BeginVertical("content");
      ImGui::BeginHorizontal("horizontal");
      ImGui::Spring(1);
      ImGui::TextUnformatted(node.Name.c_str());
      ImGui::Spring(1);
      ImGui::EndHorizontal();
      ne::Group(node.Size);
      ImGui::EndVertical();
      ImGui::PopID();
      ne::EndNode();
      ne::PopStyleColor(2);
      ImGui::PopStyleVar();

      if (ne::BeginGroupHint(node.ID)) {
        // auto alpha   = static_cast<int>(commentAlpha *
        // ImGui::GetStyle().Alpha * 255);
        auto bgAlpha = static_cast<int>(ImGui::GetStyle().Alpha * 255);

        // ImGui::PushStyleVar(ImGuiStyleVar_Alpha, commentAlpha *
        // ImGui::GetStyle().Alpha);

        auto min = ne::GetGroupMin();
        // auto max = ed::GetGroupMax();

        ImGui::SetCursorScreenPos(
            min - ImVec2(-8, ImGui::GetTextLineHeightWithSpacing() + 4));
        ImGui::BeginGroup();
        ImGui::TextUnformatted(node.Name.c_str());
        ImGui::EndGroup();

        auto drawList = ne::GetHintBackgroundDrawList();

        auto hintBounds = ImGui_GetItemRect();
        auto hintFrameBounds = ImRect_Expanded(hintBounds, 8, 4);

        drawList->AddRectFilled(
            hintFrameBounds.GetTL(), hintFrameBounds.GetBR(),
            IM_COL32(255, 255, 255, 64 * bgAlpha / 255), 4.0f);

        drawList->AddRect(hintFrameBounds.GetTL(), hintFrameBounds.GetBR(),
                          IM_COL32(255, 255, 255, 128 * bgAlpha / 255), 4.0f);

        // ImGui::PopStyleVar();
      }
      ne::EndGroupHint();
    }

    for (auto& link : links_)
      ne::Link(link.ID, link.StartPinID, link.EndPinID, link.Color, 2.0f);

    if (!createNewNode) {
      if (ne::BeginCreate(ImColor(255, 255, 255), 2.0f)) {
        auto showLabel = [](const char* label, ImColor color) {
          ImGui::SetCursorPosY(ImGui::GetCursorPosY() -
                               ImGui::GetTextLineHeight());
          auto size = ImGui::CalcTextSize(label);

          auto padding = ImGui::GetStyle().FramePadding;
          auto spacing = ImGui::GetStyle().ItemSpacing;

          ImGui::SetCursorPos(ImGui::GetCursorPos() +
                              ImVec2(spacing.x, -spacing.y));

          auto rectMin = ImGui::GetCursorScreenPos() - padding;
          auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

          auto drawList = ImGui::GetWindowDrawList();
          drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
          ImGui::TextUnformatted(label);
        };

        ne::PinId startPinId = 0, endPinId = 0;
        if (ne::QueryNewLink(&startPinId, &endPinId)) {
          auto startPin = FindPin(startPinId);
          auto endPin = FindPin(endPinId);

          newLinkPin = startPin ? startPin : endPin;

          if (startPin->Kind == PinKind::Input) {
            std::swap(startPin, endPin);
            std::swap(startPinId, endPinId);
          }

          if (startPin && endPin) {
            if (endPin == startPin) {
              ne::RejectNewItem(ImColor(255, 0, 0), 2.0f);
            } else if (endPin->Kind == startPin->Kind) {
              showLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
              ne::RejectNewItem(ImColor(255, 0, 0), 2.0f);
            }
            // else if (endPin->Node == startPin->Node)
            //{
            //     showLabel("x Cannot connect to self", ImColor(45, 32, 32,
            //     180)); ed::RejectNewItem(ImColor(255, 0, 0), 1.0f);
            // }
            else if (endPin->Type != startPin->Type) {
              showLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
              ne::RejectNewItem(ImColor(255, 128, 128), 1.0f);
            } else {
              showLabel("+ Create Link", ImColor(32, 45, 32, 180));
              if (ne::AcceptNewItem(ImColor(128, 255, 128), 4.0f)) {
                links_.emplace_back(Link(GetNextId(), startPinId, endPinId));
                links_.back().Color = GetIconColor(startPin->Type);
              }
            }
          }
        }

        ne::PinId pinId = 0;
        if (ne::QueryNewNode(&pinId)) {
          newLinkPin = FindPin(pinId);
          if (newLinkPin) showLabel("+ Create Node", ImColor(32, 45, 32, 180));

          if (ne::AcceptNewItem()) {
            createNewNode = true;
            newNodeLinkPin = FindPin(pinId);
            newLinkPin = nullptr;
            ne::Suspend();
            ImGui::OpenPopup("Create New Node");
            ne::Resume();
          }
        }
      } else
        newLinkPin = nullptr;

      ne::EndCreate();

      if (ne::BeginDelete()) {
        ne::LinkId linkId = 0;
        while (ne::QueryDeletedLink(&linkId)) {
          if (ne::AcceptDeletedItem()) {
            auto id = std::find_if(
                links_.begin(), links_.end(),
                [linkId](auto& link) { return link.ID == linkId; });
            if (id != links_.end()) links_.erase(id);
          }
        }

        ne::NodeId nodeId = 0;
        while (ne::QueryDeletedNode(&nodeId)) {
          if (ne::AcceptDeletedItem()) {
            auto id = std::find_if(
                nodes_.begin(), nodes_.end(),
                [nodeId](auto& node) { return node.ID == nodeId; });
            if (id != nodes_.end()) nodes_.erase(id);
          }
        }
      }
      ne::EndDelete();
    }

    ImGui::SetCursorScreenPos(cursorTopLeft);
  }

  auto openPopupPosition = ImGui::GetMousePos();
  ne::Suspend();
  if (ne::ShowNodeContextMenu(&contextNodeId))
    ImGui::OpenPopup("Node Context Menu");
  else if (ne::ShowPinContextMenu(&contextPinId))
    ImGui::OpenPopup("Pin Context Menu");
  else if (ne::ShowLinkContextMenu(&contextLinkId))
    ImGui::OpenPopup("Link Context Menu");
  else if (ne::ShowBackgroundContextMenu()) {
    ImGui::OpenPopup("Create New Node");
    newNodeLinkPin = nullptr;
  }
  ne::Resume();

  ne::Suspend();
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
  if (ImGui::BeginPopup("Node Context Menu")) {
    auto node = FindNode(contextNodeId);

    ImGui::TextUnformatted("Node Context Menu");
    ImGui::Separator();
    if (node) {
      ImGui::Text("ID: %p", node->ID.AsPointer());
      ImGui::Text("Type: %s",
                  node->Type == NodeType::Blueprint
                      ? "Blueprint"
                      : (node->Type == NodeType::Tree ? "Tree" : "Comment"));
      ImGui::Text("Inputs: %d", (int)node->Inputs.size());
      ImGui::Text("Outputs: %d", (int)node->Outputs.size());
    } else
      ImGui::Text("Unknown node: %p", contextNodeId.AsPointer());
    ImGui::Separator();
    if (ImGui::MenuItem("Delete")) ne::DeleteNode(contextNodeId);
    ImGui::EndPopup();
  }

  if (ImGui::BeginPopup("Pin Context Menu")) {
    auto pin = FindPin(contextPinId);

    ImGui::TextUnformatted("Pin Context Menu");
    ImGui::Separator();
    if (pin) {
      ImGui::Text("ID: %p", pin->ID.AsPointer());
      if (pin->Node)
        ImGui::Text("Node: %p", pin->Node->ID.AsPointer());
      else
        ImGui::Text("Node: %s", "<none>");
    } else
      ImGui::Text("Unknown pin: %p", contextPinId.AsPointer());

    ImGui::EndPopup();
  }

  if (ImGui::BeginPopup("Link Context Menu")) {
    auto link = FindLink(contextLinkId);

    ImGui::TextUnformatted("Link Context Menu");
    ImGui::Separator();
    if (link) {
      ImGui::Text("ID: %p", link->ID.AsPointer());
      ImGui::Text("From: %p", link->StartPinID.AsPointer());
      ImGui::Text("To: %p", link->EndPinID.AsPointer());
    } else
      ImGui::Text("Unknown link: %p", contextLinkId.AsPointer());
    ImGui::Separator();
    if (ImGui::MenuItem("Delete")) ne::DeleteLink(contextLinkId);
    ImGui::EndPopup();
  }

  if (ImGui::BeginPopup("Create New Node")) {
    auto newNodePostion = openPopupPosition;
    // ImGui::SetCursorScreenPos(ImGui::GetMousePosOnOpeningCurrentPopup());

    // auto drawList = ImGui::GetWindowDrawList();
    // drawList->AddCircleFilled(ImGui::GetMousePosOnOpeningCurrentPopup(), 10.0f,
    // 0xFFFF00FF);

    Node* node = nullptr;
    if (ImGui::MenuItem("Input Action")) node = SpawnInputActionNode();
    if (ImGui::MenuItem("Output Action")) node = SpawnOutputActionNode();
    if (ImGui::MenuItem("Branch")) node = SpawnBranchNode();
    if (ImGui::MenuItem("Do N")) node = SpawnDoNNode();
    if (ImGui::MenuItem("Set Timer")) node = SpawnSetTimerNode();
    if (ImGui::MenuItem("Less")) node = SpawnLessNode();
    if (ImGui::MenuItem("Weird")) node = SpawnWeirdNode();
    if (ImGui::MenuItem("Trace by Channel")) node = SpawnTraceByChannelNode();
    if (ImGui::MenuItem("Print String")) node = SpawnPrintStringNode();
    ImGui::Separator();
    if (ImGui::MenuItem("Comment")) node = SpawnComment();
    ImGui::Separator();
    if (ImGui::MenuItem("Sequence")) node = SpawnTreeSequenceNode();
    if (ImGui::MenuItem("Move To")) node = SpawnTreeTaskNode();
    if (ImGui::MenuItem("Random Wait")) node = SpawnTreeTask2Node();
    ImGui::Separator();
    if (ImGui::MenuItem("Message")) node = SpawnMessageNode();
    ImGui::Separator();
    if (ImGui::MenuItem("Transform")) node = SpawnHoudiniTransformNode();
    if (ImGui::MenuItem("Group")) node = SpawnHoudiniGroupNode();

    if (node) {
      BuildNodes();

      createNewNode = false;

      ne::SetNodePosition(node->ID, newNodePostion);

      if (auto startPin = newNodeLinkPin) {
        auto& pins =
            startPin->Kind == PinKind::Input ? node->Outputs : node->Inputs;

        for (auto& pin : pins) {
          if (CanCreateLink(startPin, &pin)) {
            auto endPin = &pin;
            if (startPin->Kind == PinKind::Input) std::swap(startPin, endPin);

            links_.emplace_back(Link(GetNextId(), startPin->ID, endPin->ID));
            links_.back().Color = GetIconColor(startPin->Type);

            break;
          }
        }
      }
    }

    ImGui::EndPopup();
  } else
    createNewNode = false;
  ImGui::PopStyleVar();
  ne::Resume();

  /*
      cubic_bezier_t c;
      c.p0 = pointf(100, 600);
      c.p1 = pointf(300, 1200);
      c.p2 = pointf(500, 100);
      c.p3 = pointf(900, 600);

      auto drawList = ImGui::GetWindowDrawList();
      auto offset_radius = 15.0f;
      auto acceptPoint = [drawList, offset_radius](const
     bezier_subdivide_result_t& r)
      {
          drawList->AddCircle(to_imvec(r.point), 4.0f, IM_COL32(255, 0, 255,
     255));

          auto nt = r.tangent.normalized();
          nt = pointf(-nt.y, nt.x);

          drawList->AddLine(to_imvec(r.point), to_imvec(r.point + nt *
     offset_radius), IM_COL32(255, 0, 0, 255), 1.0f);
      };

      drawList->AddBezierCurve(to_imvec(c.p0), to_imvec(c.p1), to_imvec(c.p2),
     to_imvec(c.p3), IM_COL32(255, 255, 255, 255), 1.0f);
      cubic_bezier_subdivide(acceptPoint, c);
  */

  ne::End();

  auto editorMin = ImGui::GetItemRectMin();
  auto editorMax = ImGui::GetItemRectMax();

  if (show_ordinals_) {
    int nodeCount = ne::GetNodeCount();
    std::vector<ne::NodeId> orderedNodeIds;
    orderedNodeIds.resize(static_cast<size_t>(nodeCount));
    ne::GetOrderedNodeIds(orderedNodeIds.data(), nodeCount);

    auto drawList = ImGui::GetWindowDrawList();
    drawList->PushClipRect(editorMin, editorMax);

    int ordinal = 0;
    for (auto& nodeId : orderedNodeIds) {
      auto p0 = ne::GetNodePosition(nodeId);
      auto p1 = p0 + ne::GetNodeSize(nodeId);
      p0 = ne::CanvasToScreen(p0);
      p1 = ne::CanvasToScreen(p1);

      ImGuiTextBuffer builder;
      builder.appendf("#%d", ordinal++);

      auto textSize = ImGui::CalcTextSize(builder.c_str());
      auto padding = ImVec2(2.0f, 2.0f);
      auto widgetSize = textSize + padding * 2;

      auto widgetPosition = ImVec2(p1.x, p0.y) + ImVec2(0.0f, -widgetSize.y);

      drawList->AddRectFilled(widgetPosition, widgetPosition + widgetSize,
                              IM_COL32(100, 80, 80, 190), 3.0f,
                              ImDrawFlags_RoundCornersAll);
      drawList->AddRect(widgetPosition, widgetPosition + widgetSize,
                        IM_COL32(200, 160, 160, 190), 3.0f,
                        ImDrawFlags_RoundCornersAll);
      drawList->AddText(widgetPosition + padding, IM_COL32(255, 255, 255, 255),
                        builder.c_str());
    }

    drawList->PopClipRect();
  }

  // ImGui::ShowTestWindow();
  // ImGui::ShowMetricsWindow();
}