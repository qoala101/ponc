#include "app_impl.h"

#include "app_attenuator_node.h"
#include "app_input_node.h"
#include "app_state.h"
#include "core_family_id.h"
#include "draw_widgets.h"
#include "frame_node.h"

namespace esc {
AppImpl::AppImpl(const Textures &textures)
    : project_{[&id_generator = id_generator_]() {
        auto families = std::vector<std::shared_ptr<core::IFamily>>{
            InputNode::CreateFamily(id_generator.GetNext<core::FamilyId>())
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
            id_generator.GetNext<core::FamilyId>()));

        return families;
      }()},
      widgets_{textures.node_header} {}

void AppImpl::OnFrame() {
  auto frame = frame::Frame{project_, widgets_};
  const auto app_state = AppState{.id_generator = id_generator_,
                                  .project = project_,
                                  .widgets = widgets_,
                                  .event_queue = event_queue_,
                                  .frame = frame};

  event_queue_.ExecuteEvents(app_state);
  widgets_.Draw(app_state);
}
}  // namespace esc