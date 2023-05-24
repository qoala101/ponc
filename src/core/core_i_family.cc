/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "core_i_family.h"

#include <optional>
#include <variant>

#include "core_i_node.h"
#include "core_id_generator.h"

namespace vh::ponc::core {
///
auto IFamily::GetType() const -> std::optional<FamilyType> {
  return std::nullopt;
}

///
auto IFamily::GetId() const -> FamilyId { return id_; }

///
auto IFamily::GetIds() -> std::vector<IdPtr> { return {&id_}; }

///
auto IFamily::CreateSampleNode() const -> std::unique_ptr<INode> {
  auto id_generator = IdGenerator{};
  return CreateNode(id_generator);
}

///
IFamily::IFamily(FamilyId id) : id_{id} {}
}  // namespace vh::ponc::core