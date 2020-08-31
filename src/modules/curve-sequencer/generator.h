#pragma once
#include "components/phase-timer.h"

namespace dhe {
namespace curve_sequencer {
template <typename Controls, typename Source> class Generator {
public:
  Generator(Controls &controls, Source &start_source, Source &end_source)
      : controls_{controls}, start_source_{start_source}, end_source_{
                                                              end_source} {}

  void start(int step) {
    step_ = step;
    controls_.show_progress(step_, 0.F);
    start_source_.snap(step_);
    end_source_.snap(step_);
  }

  auto generate(float /*sample_time*/) -> bool { return false; }

  void stop() { controls_.show_inactive(step_); }

private:
  Controls &controls_;
  Source &start_source_;
  Source &end_source_;
  PhaseTimer timer_{};
  int step_{0};
};
} // namespace curve_sequencer
} // namespace dhe