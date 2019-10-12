#include "modules/CurveSequencer.h"

#include "modules/controls/CommonConfig.h"
#include "modules/controls/CurvatureConfig.h"
#include "modules/controls/CurvatureInputs.h"
#include "modules/controls/DurationConfig.h"
#include "modules/controls/DurationInputs.h"
#include "modules/controls/LevelConfig.h"
#include "modules/controls/LevelInputs.h"
#include "modules/curve-sequencer/ComboStep.h"
#include "modules/curve-sequencer/GenerateStep.h"
#include "modules/curve-sequencer/SustainStep.h"

namespace dhe {

using dhe::curve_sequencer::GenerateStep;
using dhe::curve_sequencer::SustainStep;
using rack::engine::Module;

template <int N> CurveSequencer<N>::CurveSequencer() : sequence{*this, steps, N} {
  config(ParameterCount, InputCount, OutputCount, LightCount);

  configButton(this, RunButton, "Run", {"RUN input", "Yes"}, 1);
  configButton(this, GateButton, "Gate", {"GATE input", "High"}, 0);
  configButton(this, ResetButton, "Reset", {"RESET input", "High"}, 0);

  configParam(StartKnob, 1.F, N, 1.F, "Start step");
  configParam(StepsKnob, 1.F, N, N, "Sequence length", " steps");

  configLevelRangeSwitch(this, LevelRangeSwitch);
  configDurationRangeSwitch(this, DurationRangeSwitch);

  auto generateModeNames = std::array<std::string, GenerateStep::modeCount>{"Interrupt if gate rises",
                                                                            "Interrupt if gate falls",
                                                                            "Interrupt if gate changes",
                                                                            "Skip/interrupt if gate is high",
                                                                            "Skip/interrupt if gate is low",
                                                                            "Skip",
                                                                            "Run full duration"};

  auto sustainModeNames = std::array<std::string, SustainStep::modeCount>{};
  std::copy_n(generateModeNames.begin(), SustainStep::modeCount, sustainModeNames.begin());

  steps.reserve(N);

  for (int stepIndex = 0; stepIndex < N; stepIndex++) {
    configToggle<GenerateStep::modeCount>(this, GenerateModeSwitches + stepIndex, "Generate mode", generateModeNames,
                                          GenerateStep::defaultMode);
    configToggle<SustainStep::modeCount>(this, SustainModeSwitches + stepIndex, "Sustain mode", sustainModeNames,
                                         SustainStep::defaultMode);
    configLevelKnob(this, LevelKnobs + stepIndex, LevelRangeSwitch, "Level");
    configCurveShapeSwitch(this, ShapeSwitches + stepIndex, "Shape");
    configCurvatureKnob(this, CurveKnobs + stepIndex, "Curvature");
    configDurationKnob(this, DurationKnobs + stepIndex, DurationRangeSwitch, "Duration");
    configButton(this, EnabledButtons + stepIndex, "Enabled", {"ENABLED input", "Yes"}, 1);

    lights[GeneratingLights + stepIndex].setBrightness(0.F);
    lights[SustainingLights + stepIndex].setBrightness(0.F);

    steps.emplace_back(new curve_sequencer::ComboStep(*this, stepIndex));
  }
}

template <int N> auto CurveSequencer<N>::curvature(int stepIndex) const -> float {
  return dhe::curvature(params[CurveKnobs + stepIndex]);
}

template <int N> auto CurveSequencer<N>::duration(int stepIndex) const -> float {
  return selectableDuration(params[DurationKnobs + stepIndex], params[DurationRangeSwitch]);
}

template <int N> auto CurveSequencer<N>::gate() const -> bool {
  return inputIsHigh(inputs[GateInput]) || buttonIsPressed(params[GateButton]);
}

template <int N> auto CurveSequencer<N>::generateMode(int stepIndex) const -> int {
  return static_cast<int>(paramValue(params[GenerateModeSwitches + stepIndex]));
}

template <int N> auto CurveSequencer<N>::isEnabled(int stepIndex) const -> bool {
  return inputIsHigh(inputs[EnabledInputs + stepIndex]) || buttonIsPressed(params[EnabledButtons + stepIndex]);
}

template <int N> auto CurveSequencer<N>::isRunning() const -> bool {
  return inputIsHigh(inputs[RunInput]) || buttonIsPressed(params[RunButton]);
}

template <int N> auto CurveSequencer<N>::level(int stepIndex) const -> float {
  return selectableLevel(params[LevelKnobs + stepIndex], params[LevelRangeSwitch]);
}

template <int N> auto CurveSequencer<N>::output() const -> float {
  return const_cast<rack::engine::Output &>(outputs[OutOutput]).getVoltage();
}

template <int N> void CurveSequencer<N>::process(const ProcessArgs &args) { sequence.process(args.sampleRate); }

template <int N> auto CurveSequencer<N>::selectionLength() const -> int { return paramValue(params[StepsKnob]); }

template <int N> auto CurveSequencer<N>::selectionStart() const -> int { return paramValue(params[StartKnob]) - 1; }

template <int N> void CurveSequencer<N>::setGenerating(int stepIndex, bool state) {
  lights[GeneratingLights + stepIndex].setBrightness(state ? 10.F : 0.F);
}

template <int N> void CurveSequencer<N>::setOutput(float voltage) { outputs[OutOutput].setVoltage(voltage); }

template <int N> void CurveSequencer<N>::setSustaining(int stepIndex, bool state) {
  lights[SustainingLights + stepIndex].setBrightness(state ? 10.F : 0.F);
}

template <int N> auto CurveSequencer<N>::sustainMode(int stepIndex) const -> int {
  return static_cast<int>(paramValue(params[SustainModeSwitches + stepIndex]));
}

template class CurveSequencer<4>;
template class CurveSequencer<8>;
template class CurveSequencer<16>;
} // namespace dhe
