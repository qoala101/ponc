/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_CORE_DIAGRAM_H_
#define VH_PONC_CORE_DIAGRAM_H_

#include <imgui_node_editor.h>

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "core_area.h"
#include "core_i_node.h"
#include "core_id_ptr.h"
#include "core_link.h"

namespace vh::ponc::core {
///
class Diagram {
 public:
  ///
  explicit Diagram(std::string name = {},
                   std::vector<std::unique_ptr<INode>> nodes = {},
                   std::vector<Link> links = {}, std::vector<Area> areas = {});

  ///
  static auto GetIds(Diagram &diagram) -> std::vector<IdPtr>;
  ///
  static auto FindNode(const Diagram &diagram, ne::NodeId node_id) -> INode &;
  ///
  static auto FindPinNode(const Diagram &diagram, ne::PinId pin_id) -> INode &;
  ///
  static auto FindLink(Diagram &diagram, ne::LinkId link_id) -> Link &;
  ///
  static auto FindPinLink(const Diagram &diagram, ne::PinId pin_id)
      -> std::optional<const Link *>;
  ///
  static auto FindPinLink(Diagram &diagram, ne::PinId pin_id)
      -> std::optional<Link *>;
  ///
  static auto HasLink(const Diagram &diagram, ne::PinId pin_id) -> bool;
  ///
  static auto FindArea(const Diagram &diagram, AreaId area_id) -> const Area &;
  ///
  static auto FindArea(Diagram &diagram, AreaId area_id) -> Area &;
  ///
  static auto MakeUniqueDiagramName(const std::vector<Diagram> &diagrams = {},
                                    std::string source_name = "Diagram",
                                    std::string_view postfix = {})
      -> std::string;

  ///
  auto GetName() const -> const std::string &;
  ///
  void SetName(std::string name);
  ///
  auto GetNodes() const -> const std::vector<std::unique_ptr<INode>> &;
  ///
  auto EmplaceNode(std::unique_ptr<INode> node) -> INode &;
  ///
  void DeleteNode(ne::NodeId node_id);
  ///
  auto GetLinks() const -> const std::vector<Link> &;
  ///
  auto EmplaceLink(const Link &link) -> Link &;
  ///
  void DeleteLink(ne::LinkId link_id);
  ///
  auto GetAreas() const -> const std::vector<Area> &;
  ///
  auto GetAreas() -> std::vector<Area> &;
  ///
  auto EmplaceArea(const Area &area) -> Area &;
  ///
  void DeleteArea(AreaId area_id);

 private:
  ///
  auto GetLinksImpl() -> std::vector<Link> &;

  ///
  std::string name_{};
  ///
  std::vector<std::unique_ptr<INode>> nodes_{};
  ///
  std::vector<Link> links_{};
  ///
  std::vector<Area> areas_{};
};
}  // namespace vh::ponc::core

#endif  // VH_PONC_CORE_DIAGRAM_H_
