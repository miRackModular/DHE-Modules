#pragma once

#include "CurveSequencer.h"
#include "StepController.h"
#include "StepMode.h"
#include "StepSelector.h"
#include "config/CommonConfig.h"
#include "config/CurvatureConfig.h"
#include "config/DurationConfig.h"
#include "config/LevelConfig.h"

#include <components/OneShotPhaseAccumulator.h>
#include <engine/Module.hpp>

namespace dhe {

namespace curve_sequencer {
  static auto constexpr modeCount = static_cast<int>(StepMode::Sustain) + 1;
  static auto constexpr defaultMode = static_cast<int>(StepMode::Curve);
  static auto const modeDescriptions = std::array<std::string, modeCount>{"Curve", "Hold", "Sustain"};

  static auto constexpr advanceConditionCount = static_cast<int>(AdvanceCondition::GateIsLow) + 1;
  static auto constexpr defaultAdvanceCondition = static_cast<int>(AdvanceCondition::TimerExpires);
  static auto const advanceConditionDescriptions = std::array<std::string, advanceConditionCount>{
      "Timer expires", "Gate rises", "Gate falls", "Gate changes", "Gate is high", "Gate is low"};

  template <int N> class CurveSequencerModule : public rack::engine::Module {
    using Controls = CurveSequencerControls<N>;

  public:
    CurveSequencerModule() {
      config(Controls::ParameterCount, Controls::InputCount, Controls::OutputCount, Controls::LightCount);

      configButton(this, Controls::RunButton, "Run", {"from input", "Yes"}, 1);
      configButton(this, Controls::GateButton, "Gate", {"from input", "High"}, 0);
      configButton(this, Controls::LoopButton, "Loop", {"from input", "Yes"}, 0);
      configButton(this, Controls::ResetButton, "Reset", {"from input", "High"}, 0);

      configParam(Controls::SelectionStartKnob, 0.F, N - 1, 0.F, "Start step", "", 0.F, 1.F, 1.F);
      configParam(Controls::SelectionLengthKnob, 1.F, N, N, "Sequence length", " steps");

      configLevelRangeSwitch(this, Controls::LevelRangeSwitch);
      configDurationRangeSwitch(this, Controls::DurationRangeSwitch);

      for (int stepIndex = 0; stepIndex < N; stepIndex++) {
        configToggle<modeCount>(this, Controls::ModeSwitches + stepIndex, "Mode", modeDescriptions, defaultMode);
        configToggle<advanceConditionCount>(this, Controls::ConditionSwitches + stepIndex, "Advance when",
                                            advanceConditionDescriptions, defaultAdvanceCondition);
        configLevelKnob(this, Controls::LevelKnobs + stepIndex, Controls::LevelRangeSwitch, "Level");
        configCurveShapeSwitch(this, Controls::ShapeSwitches + stepIndex, "Shape");
        configCurvatureKnob(this, Controls::CurveKnobs + stepIndex, "Curvature");
        configDurationKnob(this, Controls::DurationKnobs + stepIndex, Controls::DurationRangeSwitch, "Duration");
        configButton(this, Controls::EnabledButtons + stepIndex, "Enabled", {"from input", "Yes"}, 1);

        lights[Controls::ActivityLights + stepIndex].setBrightness(0.F);
      }
    }

    ~CurveSequencerModule() override = default;

    void process(const ProcessArgs &args) override { curveSequencer.execute(args.sampleTime); }

  private:
    OneShotPhaseAccumulator phase{};
    Controls controls{inputs, outputs, params, lights};
    StepController<Controls, OneShotPhaseAccumulator> stepController{controls, phase};
    StepSelector<Controls> selector{controls, N};

    CurveSequencer<Controls, decltype(selector), decltype(stepController)> curveSequencer{controls, selector,
                                                                                          stepController};
  };
} // namespace curve_sequencer

} // namespace dhe
