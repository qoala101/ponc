#ifndef VH_CORE_CONCEPTS_H_
#define VH_CORE_CONCEPTS_H_

#include <concepts>
#include <cstdint>

namespace esc::core {
///
template <class T>
concept Id = requires(T t) {
               { t.Get() } -> std::same_as<uintptr_t>;
             } && std::constructible_from<uintptr_t>;
}  // namespace esc::core

#endif  // VH_CORE_CONCEPTS_H_
