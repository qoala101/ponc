#ifndef VH_APP_INPUT_GENERATION_H_
#define VH_APP_INPUT_GENERATION_H_

#include <memory>

#include "core_i_family.h"
#include "core_i_generation.h"
#include "json_i_family_parser.h"

namespace esc {
///
class InputGeneration : public core::IGeneration {
 public:
  ///
  auto CreateFamilies(core::IdGenerator &id_generator) const
      -> std::vector<std::unique_ptr<core::IFamily>> override;
  ///
  auto CreateFamilyParser() const
      -> std::unique_ptr<json::IFamilyParser> override;
};
}  // namespace esc

#endif  // VH_APP_INPUT_GENERATION_H_
