#pragma once

#include "components/latch.h"

namespace dhe {
namespace phase_sequencer {

using dhe::Latch;

template <typename Generator> class StepController {
public:
  StepController(Generator &generator) : generator_{generator} {}

  void enter(int step) {
    current_step_ = step;
    generator_.start(step);
  }

  void execute(float sample_time) { generator_.generate(sample_time); }

  void exit() { generator_.stop(); }

private:
  int current_step_{0};
  Generator &generator_;
};
} // namespace phase_sequencer
} // namespace dhe
