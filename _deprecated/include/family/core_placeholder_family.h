/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_CORE_PLACEHOLDER_FAMILY_H_
#define VH_CORE_PLACEHOLDER_FAMILY_H_

#include "core_i_family.h"
#include "json_i_family_parser.h"

namespace esc::core {
// NOLINTNEXTLINE(*-multiple-inheritance)
class PlaceholderFamily
    : public IFamily,
      public std::enable_shared_from_this<PlaceholderFamily> {
 public:
  explicit PlaceholderFamily(FamilyId id,
                             std::vector<std::shared_ptr<INode>> nodes = {});

  static auto CreateParser() -> std::unique_ptr<json::IFamilyParser>;

  auto CreateNode [[noreturn]] (IdGenerator& id_generator)
  -> std::unique_ptr<INode> override;
  auto CreateNodeParser() -> std::unique_ptr<json::INodeParser> override;
  auto CreateWriter() -> std::unique_ptr<json::IFamilyWriter> override;
  auto CreateDrawer() -> std::unique_ptr<coreui::IFamilyDrawer> override;

  auto EmplaceNodeFromFlow(IdGenerator& id_generator,
                           const flow::NodeFlow& connected_flow)
      -> const std::shared_ptr<INode>&;
};
}  // namespace esc::core

#endif  // VH_CORE_PLACEHOLDER_FAMILY_H_
