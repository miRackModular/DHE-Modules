#pragma once

#include <engine.hpp>

#include "controls/switch.h"
#include "util/range.h"

namespace DHE {
namespace Signal {
constexpr auto unipolar_range = Range{0.f, 10.f};
constexpr auto bipolar_range = Range{-5.f, 5.f};

inline auto range_switch(const rack::Module *module, int index)
    -> Switch<Range> {
  return Switch<Range>::two(module, index, bipolar_range, unipolar_range);
}
} // namespace Signal
} // namespace DHE