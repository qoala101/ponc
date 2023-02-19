#include "app_impl.h"

#include "app_state.h"
#include "draw_widgets.h"

namespace esc {
AppImpl::AppImpl(const Textures &textures)
    : project_{[]() {
        auto families = std::vector<std::shared_ptr<core::IFamily>>{
            // InputNode::CreateFamily(), ClientNode::CreateFamily()
        };

        // for (auto percentage_index = 0; percentage_index < 10;
        //      ++percentage_index) {
        //   families.emplace_back(CouplerNode::CreateFamily(percentage_index));
        // }

        // for (auto num_outputs : {2, 4, 8, 16}) {
        //   families.emplace_back(SplitterNode::CreateFamily(num_outputs));
        // }

        // families.emplace_back(AttenuatorNode::CreateFamily());

        return families;
      }()},
      widgets_{textures.node_header} {}

void AppImpl::OnFrame() {
  const auto app_state = AppState{.project = &project_,
                                  .id_generator = &id_generator_,
                                  .flow_calculator = &flow_calculator_,
                                  .widgets = &widgets_,
                                  .event_queue = &event_queue_};

  event_queue_.ExecuteEvents(app_state);
  flow_calculator_.Recalculate(project_);

  widgets_.Draw(app_state);
}
}  // namespace esc