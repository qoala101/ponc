/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "cpp_vector_bool.h"

namespace vh::ponc::cpp {
///
auto At(VectorBool &vector, int index) -> bool & {
  // NOLINTNEXTLINE(*-reinterpret-cast)
  return reinterpret_cast<bool &>(vector[index]);
}
}  // namespace vh::ponc::cpp