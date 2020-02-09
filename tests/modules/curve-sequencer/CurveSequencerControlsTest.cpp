#include "modules/curve-sequencer/CurveSequencerControls.h"

#include "components/Taper.h"
#include "controls/CurvatureInputs.h"
#include "controls/DurationInputs.h"
#include "controls/LevelInputs.h"
#include "modules/curve-sequencer/StepMode.h"

#include <engine/Light.hpp>
#include <engine/Param.hpp>
#include <engine/Port.hpp>
#include <gtest/gtest.h>

static auto constexpr stepCount{8};

using ::testing::Test;
using Controls = dhe::curve_sequencer::CurveSequencerControls<stepCount>;
using dhe::curve_sequencer::AdvanceCondition;
using dhe::curve_sequencer::StepMode;

class CurveSequencerControlsTest : public Test {
protected:
  std::vector<rack::engine::Input> inputs{Controls::InputCount};
  std::vector<rack::engine::Output> outputs{Controls::OutputCount};
  std::vector<rack::engine::Param> params{Controls::ParameterCount};
  std::vector<rack::engine::Light> lights{Controls::LightCount};

  Controls controls{inputs, outputs, params, lights};
};

TEST_F(CurveSequencerControlsTest, isGated_ifGateButtonIsPressed) {
  params[Controls::GateButton].setValue(1.F);

  ASSERT_TRUE(controls.isGated());
}

TEST_F(CurveSequencerControlsTest, isGated_ifGateInputIsHigh) {
  inputs[Controls::GateInput].setVoltage(10.F);

  ASSERT_TRUE(controls.isGated());
}

TEST_F(CurveSequencerControlsTest, isNotGated_ifGateButtonIsNotPressedAndGateInputIsLow) {
  ASSERT_FALSE(controls.isGated());
}

TEST_F(CurveSequencerControlsTest, isLooping_ifLoopButtonIsPressed) {
  params[Controls::LoopButton].setValue(1.F);

  ASSERT_TRUE(controls.isLooping());
}

TEST_F(CurveSequencerControlsTest, isLooping_ifLoopInputIsHigh) {
  inputs[Controls::LoopInput].setVoltage(10.F);

  ASSERT_TRUE(controls.isLooping());
}

TEST_F(CurveSequencerControlsTest, isNotLooping_ifLoopButtonIsNotPressedAndLoopInputIsLow) {
  ASSERT_FALSE(controls.isLooping());
}

TEST_F(CurveSequencerControlsTest, isReset_ifResetButtonIsPressed) {
  params[Controls::ResetButton].setValue(1.F);

  ASSERT_TRUE(controls.isReset());
}

TEST_F(CurveSequencerControlsTest, isReset_ifResetInputIsHigh) {
  inputs[Controls::ResetInput].setVoltage(10.F);

  ASSERT_TRUE(controls.isReset());
}

TEST_F(CurveSequencerControlsTest, isNotReset_ifResetButtonIsNotPressedAndResetInputIsLow) {
  ASSERT_FALSE(controls.isReset());
}

TEST_F(CurveSequencerControlsTest, isRunning_ifRunButtonIsPressed) {
  params[Controls::RunButton].setValue(1.F);

  ASSERT_TRUE(controls.isRunning());
}

TEST_F(CurveSequencerControlsTest, isRunning_ifRunInputIsHigh) {
  inputs[Controls::RunInput].setVoltage(10.F);

  ASSERT_TRUE(controls.isRunning());
}

TEST_F(CurveSequencerControlsTest, isNotRunning_ifRunButtonIsNotPressedAndRunInputIsLow) {
  ASSERT_FALSE(controls.isRunning());
}

TEST_F(CurveSequencerControlsTest, stepIsEnabled_ifStepEnabledButtonIsPressed) {
  auto constexpr step = 0;
  params[Controls::EnabledButtons + step].setValue(1.F);

  ASSERT_TRUE(controls.isEnabled(step));
}

TEST_F(CurveSequencerControlsTest, stepIsEnabled_ifStepEnabledInputIsHigh) {
  auto constexpr step = 7;
  inputs[Controls::EnabledInputs + step].setVoltage(10.F);

  ASSERT_TRUE(controls.isEnabled(step));
}

TEST_F(CurveSequencerControlsTest, stepIsNotEnabled_ifStepEnabledButtonIsNotPressedAndStepEnabledInputIsLow) {
  auto constexpr step = 2;
  ASSERT_FALSE(controls.isEnabled(step));
}

TEST_F(CurveSequencerControlsTest, selectionStart_reportsSelectionStartKnobValue) {
  params[Controls::SelectionStartKnob].setValue(3.F);

  ASSERT_EQ(controls.selectionStart(), 3);
}

TEST_F(CurveSequencerControlsTest, selectionLength_reportsSelectionLengthKnobValue) {
  params[Controls::SelectionLengthKnob].setValue(5.F);

  ASSERT_EQ(controls.selectionLength(), 5);
}

TEST_F(CurveSequencerControlsTest, input_reportsVoltageAtInputPort) {
  inputs[Controls::CurveSequencerInput].setVoltage(5.3F);

  ASSERT_EQ(controls.input(), 5.3F);
}

TEST_F(CurveSequencerControlsTest, output_setsVoltageAtOutputPort) {
  controls.output(5.3F);

  ASSERT_EQ(outputs[Controls::CurveSequencerInput].getVoltage(), 5.3F);
}

TEST_F(CurveSequencerControlsTest, output_reportsVoltageAtOutputPort) {
  outputs[Controls::CurveSequencerInput].setVoltage(7.1F);
  ASSERT_EQ(controls.output(), 7.1F);
}

TEST_F(CurveSequencerControlsTest, stepCurvature_reportsCurvatureForStepCurvatureParam) {
  auto constexpr step = 5;
  auto constexpr curveKnobRotation = 0.3F;
  params[Controls::CurveKnobs + step].setValue(curveKnobRotation);

  ASSERT_EQ(controls.curvature(step), dhe::curvature(curveKnobRotation));
}

TEST_F(CurveSequencerControlsTest, stepDuration_reportsDurationForDurationRangeSwitchAndStepDurationParam) {
  auto constexpr step = 7;
  auto constexpr durationKnobRotation = 0.75F;
  auto constexpr durationRangeSelection = 2; // Long duration

  params[Controls::DurationKnobs + step].setValue(durationKnobRotation);
  params[Controls::DurationRangeSwitch].setValue(static_cast<float>(durationRangeSelection));

  ASSERT_EQ(controls.duration(step), dhe::duration(durationKnobRotation, *dhe::durationRanges[durationRangeSelection]));
}

TEST_F(CurveSequencerControlsTest, stepLevel_reportsLevelForLevelRangeSwitchAndStepLevelParam) {
  auto constexpr step = 3;
  auto constexpr levelKnobRotation = 0.35F;
  auto constexpr levelRangeSelection = 1; // unipolar

  params[Controls::LevelKnobs + step].setValue(levelKnobRotation);
  params[Controls::LevelRangeSwitch].setValue(static_cast<float>(levelRangeSelection));

  ASSERT_EQ(controls.level(step), dhe::level(levelKnobRotation, *dhe::signalRanges[levelRangeSelection]));
}

TEST_F(CurveSequencerControlsTest, stepShape_isTaperSelectedByStepShapeSwitch) {
  auto constexpr step = 3;

  auto shapeSelection = 0; // J
  params[Controls::ShapeSwitches + step].setValue(static_cast<float>(shapeSelection));
  EXPECT_EQ(controls.taper(step), dhe::taper::variableTapers[shapeSelection]);

  shapeSelection = 1; // S
  params[Controls::ShapeSwitches + step].setValue(static_cast<float>(shapeSelection));
  EXPECT_EQ(controls.taper(step), dhe::taper::variableTapers[shapeSelection]);
}

TEST_F(CurveSequencerControlsTest, stepMode_isModeSelectedByStepModeSwitch) {
  auto constexpr step = 6;

  auto modeSelectedBySwitch = StepMode::Curve;
  params[Controls::ModeSwitches + step].setValue(static_cast<float>(modeSelectedBySwitch));
  EXPECT_EQ(controls.mode(step), modeSelectedBySwitch);

  modeSelectedBySwitch = StepMode::Hold;
  params[Controls::ModeSwitches + step].setValue(static_cast<float>(modeSelectedBySwitch));
  EXPECT_EQ(controls.mode(step), modeSelectedBySwitch);
}

TEST_F(CurveSequencerControlsTest, stepCondition_isConditionSelectedByStepConditionSwitch) {
  auto constexpr step = 0;

  auto conditionSelectedBySwitch = AdvanceCondition::TimerExpires;
  params[Controls::ConditionSwitches + step].setValue(static_cast<float>(conditionSelectedBySwitch));
  EXPECT_EQ(controls.condition(step), conditionSelectedBySwitch);

  conditionSelectedBySwitch = AdvanceCondition::GateIsHigh;
  params[Controls::ConditionSwitches + step].setValue(static_cast<float>(conditionSelectedBySwitch));
  EXPECT_EQ(controls.condition(step), conditionSelectedBySwitch);

  conditionSelectedBySwitch = AdvanceCondition::GateIsLow;
  params[Controls::ConditionSwitches + step].setValue(static_cast<float>(conditionSelectedBySwitch));
  EXPECT_EQ(controls.condition(step), conditionSelectedBySwitch);

  conditionSelectedBySwitch = AdvanceCondition::GateRises;
  params[Controls::ConditionSwitches + step].setValue(static_cast<float>(conditionSelectedBySwitch));
  EXPECT_EQ(controls.condition(step), conditionSelectedBySwitch);

  conditionSelectedBySwitch = AdvanceCondition::GateFalls;
  params[Controls::ConditionSwitches + step].setValue(static_cast<float>(conditionSelectedBySwitch));
  EXPECT_EQ(controls.condition(step), conditionSelectedBySwitch);

  conditionSelectedBySwitch = AdvanceCondition::GateChanges;
  params[Controls::ConditionSwitches + step].setValue(static_cast<float>(conditionSelectedBySwitch));
  EXPECT_EQ(controls.condition(step), conditionSelectedBySwitch);
}

TEST_F(CurveSequencerControlsTest, showInactive_dimsStepProgressLights) {
  auto constexpr step = 3;
  auto constexpr completedProgressLightIndex = step + step;
  auto constexpr remainingProgressLightIndex = step + step + 1;

  controls.showInactive(step);

  EXPECT_EQ(lights[Controls::ProgressLights + completedProgressLightIndex].getBrightness(), 0.F);
  EXPECT_EQ(lights[Controls::ProgressLights + remainingProgressLightIndex].getBrightness(), 0.F);
}