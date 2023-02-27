#include "app_impl.h"

#include "app_attenuator_node.h"
#include "app_input_node.h"
#include "core_family_id.h"
#include "core_id_generator.h"
#include "draw_main_window.h"
#include "frame_node.h"

namespace esc {
AppImpl::AppImpl(const Textures &textures)
    : project_{[]() {
        auto id_generator = core::IdGenerator{};
        auto families = std::vector<std::shared_ptr<core::IFamily>>{
            InputNode::CreateFamily(id_generator.Generate<core::FamilyId>())
            // , ClientNode::CreateFamily()
        };

        // for (auto percentage_index = 0; percentage_index < 10;
        //      ++percentage_index) {
        //   families.emplace_back(CouplerNode::CreateFamily(percentage_index));
        // }

        // for (auto num_outputs : {2, 4, 8, 16}) {
        //   families.emplace_back(SplitterNode::CreateFamily(num_outputs));
        // }

        families.emplace_back(AttenuatorNode::CreateFamily(
            id_generator.Generate<core::FamilyId>()));

        return families;
      }()}, controller_{&project_} {}

void AppImpl::OnFrame() {
  controller_.OnFrame();
  main_window_.Draw(controller_);
}
}  // namespace esc