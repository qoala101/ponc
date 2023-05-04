#ifndef VH_PONC_COREUI_CLONER_H_
#define VH_PONC_COREUI_CLONER_H_

#include "core_diagram.h"
#include "core_id_generator.h"
#include "cpp_no_instances.h"

namespace vh::ponc::coreui {
///
struct Cloner : public cpp::NoInstances {
  ///
  static auto Clone(const core::Diagram& diagram,
                    const std::vector<std::unique_ptr<core::IFamily>>& families)
      -> core::Diagram;

  ///
  static void RewireIds(const std::vector<core::IdPtr>& ids,
                        core::IdGenerator& id_generator);
};
}  // namespace vh::ponc::coreui

#endif  // VH_PONC_COREUI_CLONER_H_
