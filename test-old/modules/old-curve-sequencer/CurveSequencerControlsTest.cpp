#include "modules/old-curve-sequencer/curve-sequencer-controls.h"

#include "components/sigmoid.h"
#include "controls/curvature-inputs.h"
#include "controls/duration-inputs.h"
#include "controls/level-inputs.h"
#include "fake-rack/Controls.h"
#include "modules/old-curve-sequencer/generate-mode.h"

#include <doctest.h>

namespace test {
namespace old_curve_sequencer {
namespace controls {
static auto constexpr stepCount{8};
using Controls = dhe::old_curve_sequencer::CurveSequencerControls<
    fake::Port, fake::Port, fake::Param, fake::Light, stepCount>;
using dhe::old_curve_sequencer::AdvanceMode;
using dhe::old_curve_sequencer::GenerateMode;

TEST_CASE("curve_sequencer::CurveSequencerControls") {
  std::vector<fake::Port> inputs{Controls::InputCount};
  std::vector<fake::Port> outputs{Controls::OutputCount};
  std::vector<fake::Param> params{Controls::ParameterCount};
  std::vector<fake::Light> lights{Controls::LightCount};

  Controls controls{inputs, outputs, params, lights};

  SUBCASE("is_gated()") {
    SUBCASE("true if gate button is pressed") {
      params[Controls::GateButton].setValue(1.F);

      CHECK(controls.is_gated());
    }

    SUBCASE("true if gate input is high") {
      inputs[Controls::GateInput].setVoltage(10.F);

      CHECK(controls.is_gated());
    }

    SUBCASE("true if gate button is not pressed and gate input is low") {
      params[Controls::GateButton].setValue(0.F);
      inputs[Controls::GateInput].setVoltage(0.F);

      CHECK_FALSE(controls.is_gated());
    }
  }

  SUBCASE("is_looping()") {
    SUBCASE("true if loop button is pressed") {
      params[Controls::LoopButton].setValue(1.F);

      CHECK(controls.is_looping());
    }

    SUBCASE("true if loop input is high") {
      inputs[Controls::LoopInput].setVoltage(10.F);

      CHECK(controls.is_looping());
    }

    SUBCASE("true if loop button is not pressed and loop input is low") {
      params[Controls::LoopButton].setValue(0.F);
      inputs[Controls::LoopInput].setVoltage(0.F);

      CHECK_FALSE(controls.is_looping());
    }
  }

  SUBCASE("is_reset()") {
    SUBCASE("true if reset button is pressed") {
      params[Controls::ResetButton].setValue(1.F);

      CHECK(controls.is_reset());
    }

    SUBCASE("true if reset input is high") {
      inputs[Controls::ResetInput].setVoltage(10.F);

      CHECK(controls.is_reset());
    }

    SUBCASE("true if reset button is not pressed and reset input is low") {
      params[Controls::ResetButton].setValue(0.F);
      inputs[Controls::ResetInput].setVoltage(0.F);

      CHECK_FALSE(controls.is_reset());
    }
  }

  SUBCASE("is_running()") {
    SUBCASE("true if run button is pressed") {
      params[Controls::RunButton].setValue(1.F);

      CHECK(controls.is_running());
    }

    SUBCASE("true if run input is high") {
      inputs[Controls::RunInput].setVoltage(10.F);

      CHECK(controls.is_running());
    }

    SUBCASE("true if run button is not pressed and run input is low") {
      params[Controls::RunButton].setValue(0.F);
      inputs[Controls::RunInput].setVoltage(0.F);

      CHECK_FALSE(controls.is_running());
    }
  }

  SUBCASE("is_enabled(step)") {
    SUBCASE("true if enabled button is pressed") {
      auto constexpr step = 3;
      params[Controls::EnabledButtons + step].setValue(1.F);

      CHECK(controls.is_enabled(step));
    }

    SUBCASE("true if enabled input is high") {
      auto constexpr step = 6;
      inputs[Controls::EnabledInputs + step].setVoltage(10.F);

      CHECK(controls.is_enabled(step));
    }

    SUBCASE("true if enabled button is not pressed and enabled input is low") {
      auto constexpr step = 7;

      params[Controls::EnabledButtons + step].setValue(0.F);
      inputs[Controls::EnabledInputs + step].setVoltage(0.F);

      CHECK_FALSE(controls.is_enabled(step));
    }
  }

  SUBCASE("selection_start() reports selection start knob value") {
    params[Controls::SelectionStartKnob].setValue(3.F);

    CHECK_EQ(controls.selection_start(), 3);
  }

  SUBCASE("selection_length() reports selection length knob value") {
    params[Controls::SelectionLengthKnob].setValue(5.F);

    CHECK_EQ(controls.selection_length(), 5);
  }

  SUBCASE("input() reports voltage at input port") {
    inputs[Controls::CurveSequencerInput].setVoltage(5.3F);

    CHECK_EQ(controls.input(), 5.3F);
  }

  SUBCASE("output() reports voltage at output port") {
    outputs[Controls::CurveSequencerOutput].setVoltage(7.1F);

    CHECK_EQ(controls.output(), 7.1F);
  }

  SUBCASE("output(voltage) sets voltage at output port") {
    controls.output(5.3F);

    CHECK_EQ(outputs[Controls::CurveSequencerOutput].getVoltage(), 5.3F);
  }

  SUBCASE("curvature(step)  reports curvature for step curvature param") {
    auto constexpr step = 5;
    auto constexpr curveKnobRotation = 0.3F;
    params[Controls::CurveKnobs + step].setValue(curveKnobRotation);

    CHECK_EQ(controls.curvature(step), dhe::curvature(curveKnobRotation));
  }

  SUBCASE("duration(step) reports duration for duration range switch and step "
          "duration param") {
    auto constexpr step = 7;
    auto constexpr durationKnobRotation = 0.75F;
    auto constexpr durationRangeSelection = 2; // Long duration

    params[Controls::DurationKnobs + step].setValue(durationKnobRotation);
    params[Controls::DurationRangeSwitch].setValue(
        static_cast<float>(durationRangeSelection));

    CHECK_EQ(controls.duration(step),
             dhe::duration(durationKnobRotation,
                           dhe::duration_ranges[durationRangeSelection]));
  }

  SUBCASE(
      "level(step) reports level for level range switch and step level param") {
    auto constexpr step = 3;
    auto constexpr levelKnobRotation = 0.35F;
    auto constexpr levelRangeSelection = 1; // unipolar

    params[Controls::LevelKnobs + step].setValue(levelKnobRotation);
    params[Controls::LevelRangeSwitch].setValue(
        static_cast<float>(levelRangeSelection));

    CHECK_EQ(controls.level(step),
             dhe::level(levelKnobRotation,
                        dhe::signal_ranges[levelRangeSelection]));
  }

  SUBCASE("taper(step) is taper selected by step shape switch") {
    auto constexpr step = 3;

    auto shapeSelection = 0; // J
    params[Controls::ShapeSwitches + step].setValue(
        static_cast<float>(shapeSelection));
    CHECK_EQ(controls.taper(step), dhe::sigmoid::tapers[shapeSelection]);

    shapeSelection = 1; // S
    params[Controls::ShapeSwitches + step].setValue(
        static_cast<float>(shapeSelection));
    CHECK_EQ(controls.taper(step), dhe::sigmoid::tapers[shapeSelection]);
  }

  SUBCASE("mode(step) is mode selected by step mode switch") {
    auto constexpr step = 6;

    auto modeSelectedBySwitch = GenerateMode::Curve;
    params[Controls::ModeSwitches + step].setValue(
        static_cast<float>(modeSelectedBySwitch));
    CHECK_EQ(controls.mode(step), modeSelectedBySwitch);

    modeSelectedBySwitch = GenerateMode::Hold;
    params[Controls::ModeSwitches + step].setValue(
        static_cast<float>(modeSelectedBySwitch));
    CHECK_EQ(controls.mode(step), modeSelectedBySwitch);
  }

  SUBCASE("condition(step) is condition selected by step condition switch") {
    auto constexpr step = 0;

    auto conditionSelectedBySwitch = AdvanceMode::TimerExpires;
    params[Controls::ConditionSwitches + step].setValue(
        static_cast<float>(conditionSelectedBySwitch));
    CHECK_EQ(controls.condition(step), conditionSelectedBySwitch);

    conditionSelectedBySwitch = AdvanceMode::GateIsHigh;
    params[Controls::ConditionSwitches + step].setValue(
        static_cast<float>(conditionSelectedBySwitch));
    CHECK_EQ(controls.condition(step), conditionSelectedBySwitch);

    conditionSelectedBySwitch = AdvanceMode::GateIsLow;
    params[Controls::ConditionSwitches + step].setValue(
        static_cast<float>(conditionSelectedBySwitch));
    CHECK_EQ(controls.condition(step), conditionSelectedBySwitch);

    conditionSelectedBySwitch = AdvanceMode::GateRises;
    params[Controls::ConditionSwitches + step].setValue(
        static_cast<float>(conditionSelectedBySwitch));
    CHECK_EQ(controls.condition(step), conditionSelectedBySwitch);

    conditionSelectedBySwitch = AdvanceMode::GateFalls;
    params[Controls::ConditionSwitches + step].setValue(
        static_cast<float>(conditionSelectedBySwitch));
    CHECK_EQ(controls.condition(step), conditionSelectedBySwitch);

    conditionSelectedBySwitch = AdvanceMode::GateChanges;
    params[Controls::ConditionSwitches + step].setValue(
        static_cast<float>(conditionSelectedBySwitch));
    CHECK_EQ(controls.condition(step), conditionSelectedBySwitch);
  }

  SUBCASE("show_inactive(step) dims step progress lights") {
    auto constexpr step = 3;
    auto constexpr completedProgressLightIndex = step + step;
    auto constexpr remainingProgressLightIndex = step + step + 1;

    controls.show_inactive(step);

    CHECK_EQ(lights[Controls::ProgressLights + completedProgressLightIndex]
                 .getBrightness(),
             0.F);
    CHECK_EQ(lights[Controls::ProgressLights + remainingProgressLightIndex]
                 .getBrightness(),
             0.F);
  }
}
} // namespace controls
} // namespace curve_sequencer
} // namespace test
