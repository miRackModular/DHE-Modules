#pragma once

#include <algorithm>

namespace dhe {
namespace stage {

  template <typename Controls> class Stage {
  public:
    Stage(Controls &controls) : controls{controls} {}

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
        controls.sendOut(controls.level());
        break;
      case Deferring:
      case TrackingInput:
        controls.sendOut(controls.envelopeIn());
      }

      advanceEoc(sampleTime);
      controls.sendActive(state == Deferring || state == Generating);
      controls.sendEoc(isEoc);
    }

  private:
    enum State {
      Deferring,
      Generating,
      TrackingInput,
      TrackingLevel,
    };

    auto identifyState() -> Stage::State {
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
      if (state == Deferring) {
        resetTrigger();
      }

      if (newState == Generating) {
        resetGenerator();
      }
      state = newState;
    }

    void resetTrigger() { triggerWasHigh = false; }

    auto triggerRise() -> bool {
      auto const isHigh = controls.isTriggered();
      auto const isRise = isHigh && !triggerWasHigh;
      triggerWasHigh = isHigh;
      return isRise;
    }

    void resetGenerator() {
      startVoltage = controls.envelopeIn();
      stagePhase = 0.F;
    }

    void generate(float sampleTime) {
      if (stagePhase < 1.F) {
        auto const duration = controls.duration();
        stagePhase = std::min(1.F, stagePhase + sampleTime / duration);

        auto const *taper = controls.taper();
        auto const level = controls.level();
        auto const curvature = controls.curvature();
        auto const taperedPhase = taper->apply(stagePhase, curvature);
        controls.sendOut(scale(taperedPhase, startVoltage, level));
        if (stagePhase == 1.F) {
          finishGenerating();
        }
      }
    }

    void finishGenerating() {
      startEoc();
      enter(TrackingLevel);
    }

    void startEoc() {
      isEoc = true;
      eocPhase = 0.F;
    }

    void advanceEoc(float sampleTime) {
      if (eocPhase < 1.F) {
        eocPhase = std::min(1.F, eocPhase + sampleTime / 1e-3F);
        if (eocPhase == 1.F) {
          finishEoc();
        }
      }
    }

    void finishEoc() { isEoc = false; }

    float eocPhase{1.F};
    bool isEoc{false};
    float stagePhase{0.F};
    float startVoltage{0.F};
    State state{TrackingInput};
    bool triggerWasHigh{false};
    Controls &controls;
  };
} // namespace stage
} // namespace dhe
