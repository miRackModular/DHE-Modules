#include "modules/curve-sequencer/ComboStep.h"

#include "modules/curve-sequencer/Step.h"
#include "modules/curve-sequencer/StepControls.h"

namespace dhe {
namespace curve_sequencer {

  ComboStep::ComboStep(StepControls &controls, int stepIndex) :
      ComboStep{controls, stepIndex, new GenerateStep{controls, stepIndex}, new SustainStep{controls, stepIndex}} {}

  ComboStep::ComboStep(StepControls &controls, int stepIndex, Step *generateStep, Step *sustainStep) :
      controls{controls}, stepIndex{stepIndex}, generateStep{generateStep}, sustainStep{sustainStep} {}

  auto ComboStep::isAvailable() const -> bool { return controls.isEnabled(stepIndex); }

  void ComboStep::process(float sampleTime) {
    if (!controls.isEnabled(stepIndex)) {
      return;
    }
    if (generateStep->isAvailable()) {
      generateStep->process(sampleTime);
    } else if (sustainStep->isAvailable()) {
      sustainStep->process(sampleTime);
    }
  }

} // namespace curve_sequencer
} // namespace dhe