#pragma once

#include "modules/curve-sequencer/triggers.h"
#include <dheunit/test.h>

namespace test {
namespace curve_sequencer {
using dhe::curve_sequencer::TriggerMode;
using dhe::unit::Tester;
using dhe::unit::TestFunc;

auto constexpr step_count = 8;
struct Controls {
  auto interrupt_on_trigger(int step) const -> bool {
    return interrupt_on_trigger_[step];
  }
  auto trigger_mode(int step) const -> TriggerMode {
    return trigger_mode_[step];
  }

  std::array<bool, step_count> interrupt_on_trigger_{}; // NOLINT
  std::array<TriggerMode, step_count> trigger_mode_{};  // NOLINT
};

using Interrupter = dhe::curve_sequencer::Interrupter<Controls>;

template <typename T>
static inline auto test(T const &interrupter_test) -> TestFunc {
  return [interrupter_test](Tester &t) {
    Controls controls{};
    Interrupter interrupter{controls};
    interrupter_test(t, controls, interrupter);
  };
}
} // namespace curve_sequencer
} // namespace test