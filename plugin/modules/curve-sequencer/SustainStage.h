#pragma once

#include "SequenceMode.h"
#include "StageMode.h"
#include "components/Latch.h"

namespace dhe {
namespace curve_sequencer {
  template <typename Controls> class SustainStage {
  public:
    SustainStage(Controls &controls, int stepCount) : controls{controls}, stepMask{stepCount - 1} {}

    void enter(int entryStep) {
      step = entryStep;
      showActive(true);
    }

    auto execute(dhe::Latch const &gateLatch) const -> SequenceMode {
      if (isActive(sustainMode(), gateLatch)) {
        return SequenceMode::Sustaining;
      }
      showActive(false);
      return SequenceMode::Advancing;
    };

    void exit() { showActive(false); }

  private:
    auto sustainMode() const -> StageMode { return controls.sustainMode(step); }

    void showActive(bool state) const { controls.showSustaining(step, state); }

    int step{0};
    Controls &controls;
    int const stepMask;
  };
} // namespace curve_sequencer
} // namespace dhe
