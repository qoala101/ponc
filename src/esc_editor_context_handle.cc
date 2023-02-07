#include "esc_editor_context_handle.h"

#include "esc_cpp.h"

namespace esc {
EditorContextHandle::EditorContextHandle(const ne::Config& config) {
  editor_context_.reset(ne::CreateEditor(&config));
  ne::SetCurrentEditor(editor_context_.get());
}

void EditorContextHandle::ContextDestroyer::operator()(
    ne::EditorContext* context) noexcept {
  cpp::Expects(context != nullptr);
  ne::DestroyEditor(context);
}
}  // namespace esc