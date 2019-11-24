#pragma once

#include "Event.h"
#include "components/Range.h"

#include <algorithm>

namespace dhe {
namespace stage {

  template <typename Controls, typename DeferMode, typename InputMode, typename GenerateMode, typename LevelMode>
  class StageEngine {
  public:
    StageEngine(Controls &controls, DeferMode &deferMode, InputMode &inputMode, GenerateMode &generateMode,
                LevelMode &levelMode) :
        controls{controls},
        deferMode{deferMode},
        inputMode{inputMode},
        generateMode{generateMode},
        levelMode{levelMode} {}

    void process(float sampleTime) {
      auto const newState = identifyState();
      if (state != newState) {
        enter(newState);
      }

      switch (state) {
      case Generating:
        generate(sampleTime);
        break;
      case TrackingLevel:
        levelMode.execute();
        break;
      case Deferring:
        deferMode.execute();
        break;
      case TrackingInput:
        inputMode.execute();
        break;
      }

      advanceEoc(sampleTime);
      controls.showEoc(isEoc);
    }

  private:
    enum State {
      Deferring,
      Generating,
      TrackingInput,
      TrackingLevel,
    };

    auto identifyState() -> StageEngine::State {
      if (controls.isDeferring()) {
        return Deferring;
      }
      if (state == Deferring) {
        return TrackingInput;
      }
      if (triggerRise()) {
        enter(Generating);
        return Generating;
      }
      return state;
    }

    void enter(State newState) {
      switch (state) {
      case Generating:
        generateMode.exit();
        break;
      case Deferring:
        deferMode.exit();
        resetTrigger();
        break;
      case TrackingInput:
        inputMode.exit();
        break;
      case TrackingLevel:
        levelMode.exit();
        break;
      }

      state = newState;

      switch (state) {
      case Deferring:
        deferMode.enter();
        break;
      case Generating:
        generateMode.enter();
        break;
      case TrackingInput:
        inputMode.enter();
        break;
      case TrackingLevel:
        levelMode.enter();
        break;
      }
    }

    void resetTrigger() { triggerWasHigh = false; }

    auto triggerRise() -> bool {
      auto const isHigh = controls.isTriggered();
      auto const isRise = isHigh && !triggerWasHigh;
      triggerWasHigh = isHigh;
      return isRise;
    }

    void generate(float sampleTime) {
      auto const event = generateMode.execute(sampleTime);
      if (event == Event::Completed) {
        beginEocPulse();
        enter(TrackingLevel);
      }
    }
    void advanceEoc(float sampleTime) {
      if (eocPhase < 1.F) {
        eocPhase = std::min(1.F, eocPhase + sampleTime / 1e-3F);
        if (eocPhase == 1.F) {
          finishEocPulse();
        }
      }
    }

    void beginEocPulse() {
      isEoc = true;
      eocPhase = 0.F;
    }

    void finishEocPulse() { isEoc = false; }

    float eocPhase{1.F};
    bool isEoc{false};
    State state{TrackingInput};
    bool triggerWasHigh{false};
    Controls &controls;
    DeferMode &deferMode;
    InputMode &inputMode;
    GenerateMode &generateMode;
    LevelMode &levelMode;
  };
} // namespace stage
} // namespace dhe
