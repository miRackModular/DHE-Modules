#pragma once

#include "modules/components/Latch.h"
#include "modules/curve-sequencer/SequenceControls.h"
#include "modules/curve-sequencer/Step.h"

#include <memory>
#include <vector>

namespace dhe {
namespace curve_sequencer {

  /**
   * Controls the sequence in a CurveSequencer module.
   */
  class Sequence {
  public:
    Sequence(SequenceControls &controls, Latch &runLatch, Latch &gateLatch, std::vector<std::unique_ptr<Step>> &steps);
    ~Sequence() = default;
    void process(float sampleTime);

  private:
    void start(float sampleTime);

    SequenceControls &controls;
    Latch &runLatch;
    Latch &gateLatch;
    std::vector<std::unique_ptr<Step>> &steps;
  };

  Sequence::Sequence(SequenceControls &controls, Latch &runLatch, Latch &gateLatch,
                     std::vector<std::unique_ptr<Step>> &steps) :
      controls{controls}, runLatch{runLatch}, gateLatch{gateLatch}, steps{steps} {}
  void Sequence::process(float sampleTime) {
    runLatch.step();
    gateLatch.step();

    if (runLatch.isLow()) {
      return;
    }
    if (gateLatch.isRise()) {
      start(sampleTime);
    }
  }
  void Sequence::start(float sampleTime) {
    auto const first = controls.selectionStart();
    auto const length = controls.selectionLength();
    auto const mask = steps.size() - 1;

    for (int i = 0; i < length; i++) {
      auto const index = (first + i) & mask;
      auto &step = steps[index];
      if (step->isAvailable()) {
        step->process(sampleTime);
        return;
      }
    }
  }
} // namespace curve_sequencer
} // namespace dhe