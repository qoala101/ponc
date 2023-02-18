/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_CORE_FREE_PIN_FAMILY_H_
#define VH_CORE_FREE_PIN_FAMILY_H_

#include "core_i_family.h"
#include "imgui_node_editor.h"
#include "json_i_family_parser.h"

namespace esc::core {
// ---
// NOLINTNEXTLINE(*-multiple-inheritance)
class FreePinFamily : public IFamily,
                      public std::enable_shared_from_this<FreePinFamily> {
 public:
  // ---
  explicit FreePinFamily(std::vector<std::shared_ptr<INode>> nodes = {});

  // ---
  static auto CreateParser [[nodiscard]] ()
  -> std::unique_ptr<json::IFamilyParser>;

  // ---
  auto CreateNodeParser [[nodiscard]] ()
  -> std::unique_ptr<json::INodeParser> override;
  // ---
  auto CreateWriter [[nodiscard]] ()
  -> std::unique_ptr<json::IFamilyWriter> override;
  // ---
  auto CreateDrawer [[nodiscard]] ()
  -> std::unique_ptr<coreui::IFamilyDrawer> override;

  auto EmplaceNodeFromFlow [[nodiscard]] (IdGenerator& id_generator,
                                          ne::PinId pin_id, bool has_input_pin)
  -> const std::shared_ptr<INode>&;

 private:
  // ---
  auto CreateNode [[noreturn]] (IdGenerator& id_generator)
  -> std::shared_ptr<INode> override;
};
}  // namespace esc::core

#endif  // VH_CORE_FREE_PIN_FAMILY_H_
