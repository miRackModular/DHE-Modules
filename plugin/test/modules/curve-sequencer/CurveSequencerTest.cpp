#include "curve-sequencer/CurveSequencer.h"

#include "components/Latch.h"
#include "curve-sequencer/StepEvent.h"
#include "mocks.h"

#include <gmock/gmock-actions.h>
#include <gtest/gtest.h>

using dhe::Latch;
using dhe::curve_sequencer::CurveSequencer;
using dhe::curve_sequencer::StepEvent;
using ::testing::A;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;

class CurveSequencerTest : public ::testing::Test {
protected:
  static auto constexpr sampleTime{1.F / 12345.F};

  NiceMock<MockControls> controls{};
  NiceMock<MockStepSelector> stepSelector{};
  NiceMock<MockStepController> stepController{};
  CurveSequencer<MockControls, MockStepSelector, MockStepController> curveSequencer{controls, stepSelector,
                                                                                    stepController};

  void givenGate(bool state) { ON_CALL(controls, isGated()).WillByDefault(Return(state)); }

  void givenRun(bool state) { ON_CALL(controls, isRunning()).WillByDefault(Return(state)); }

  void givenInitialized() {
    Mock::VerifyAndClear(&controls);
    Mock::VerifyAndClear(&stepSelector);
    Mock::VerifyAndClear(&stepController);
  }
  void givenIdling() {
    givenInitialized();
    givenRun(true);
  }
};

TEST_F(CurveSequencerTest, runLowWhilePaused_doesNothing) {
  givenRun(false);
  // TODO: Test for no interactions on step selector, generate mode, and sustain mode.
}

TEST_F(CurveSequencerTest, runRiseWhilePaused_BeginsIdling) {
  givenInitialized();
  // TODO: How to test for idling?
}

TEST_F(CurveSequencerTest, gateLowWhileIdling_doesNothing) {
  givenIdling();

  EXPECT_CALL(stepSelector, first(A<Latch const &>())).Times(0);

  curveSequencer.execute(sampleTime);
}

TEST_F(CurveSequencerTest, gateRiseWhileIdling_generatesFirstAvailableStep) {
  givenIdling();
  givenGate(true);

  auto constexpr step{3};

  ON_CALL(stepSelector, first(A<Latch const &>())).WillByDefault(Return(step));

  EXPECT_CALL(stepController, enter(step));
  EXPECT_CALL(stepController, execute(A<Latch const &>(), sampleTime)).WillOnce(Return(StepEvent::Generated));

  curveSequencer.execute(sampleTime);
}

// TODO: Idling + Gate rise sustains first if generating returns sustaining

// TODO: Idling + Gate rise does nothing if no available step
