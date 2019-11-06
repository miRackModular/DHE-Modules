#pragma once

#include "Advancing.h"
#include "CurveSequencerControls.h"
#include "Generating.h"
#include "Idle.h"
#include "SequenceMode.h"
#include "Sustaining.h"
#include "components/Latch.h"
#include "controls/CommonInputs.h"

#include <vector>

namespace dhe {
namespace curve_sequencer {
  template <int N, typename InputType, typename OutputType, typename ParamType, typename LightType>
  class CurveSequencer {
    using Controls = CurveSequencerControls<N>;

  public:
    CurveSequencer(std::vector<InputType> &inputs, std::vector<OutputType> &outputs, std::vector<ParamType> &params,
                   std::vector<LightType> &lights) :
        inputs{inputs}, outputs{outputs}, params{params}, lights{lights} {}

    void execute(float sampleTime) {
      runLatch.clock(isRunning());
      gateLatch.clock(gate());

      if (runLatch.isLow()) {
        return;
      }

      do {
        auto const next = executeMode(sampleTime);
        if (next.mode == mode) {
          return;
        }
        updateState(next);
      } while (mode != SequenceMode::Idle);
    }

  private:
    auto executeMode(float sampleTime) const -> SequencerState {
      switch (mode) {
      case SequenceMode::Idle:
        return idleMode.execute(gateLatch);
      case SequenceMode::Advancing:
        return advancingMode.execute(step);
      case SequenceMode::Generating:
        return generatingMode.execute(gateLatch, sampleTime);
      case SequenceMode::Sustaining:
        return sustainingMode.execute(gateLatch);
      }
    }

    auto gate() const -> bool { return isHigh(inputs[Controls::GateInput]) || isPressed(params[Controls::GateButton]); }

    auto isRunning() const -> bool {
      return isHigh(inputs[Controls::RunInput]) || isPressed(params[Controls::RunButton]);
    }

    void updateState(SequencerState next) {
      gateLatch.clock(gateLatch.isHigh()); // To remove the edge
      step = next.step;
      mode = next.mode;
      switch (next.mode) {
      case SequenceMode::Generating:
        generatingMode.enter(step);
        return;
      case SequenceMode::Sustaining:
        sustainingMode.enter(step);
        return;
      case SequenceMode::Idle:
      case SequenceMode::Advancing:
        return;
      }
    }

    int step{0};
    Latch runLatch{};
    Latch gateLatch{};
    std::vector<InputType> &inputs;
    std::vector<OutputType> &outputs;
    std::vector<ParamType> &params;
    std::vector<LightType> &lights;
    SequenceMode mode{SequenceMode::Idle};
    Advancing<N> advancingMode{inputs, params};
    Generating<N> generatingMode{inputs, params, lights};
    Idle<N> idleMode{params};
    Sustaining<N> sustainingMode{inputs, params, lights};
  };
} // namespace curve_sequencer
} // namespace dhe
