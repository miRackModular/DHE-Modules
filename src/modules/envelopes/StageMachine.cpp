#include "modules/envelopes/StageMachine.h"

#include "modules/components/Range.h"

#include <algorithm>

namespace dhe {

void StageMachine::process(float sampleTime) {
  auto const newState = identifyState();
  if (state != newState) {
    enter(newState);
  }

  switch (state) {
  case Generating:
    generate(sampleTime);
    break;
  case TrackingLevel:
    sendOut(level());
    break;
  case Deferring:
  case TrackingInput:
    sendOut(envelopeIn());
  }

  advanceEoc(sampleTime);
  sendActive(state == Deferring || state == Generating);
  sendEoc(isEoc);
}

StageMachine::State StageMachine::identifyState() {
  if (deferIsHigh()) {
    return Deferring;
  }
  if (state == Deferring) {
    return TrackingInput;
  }
  if (triggerRise()) {
    return Generating;
  }
  return state;
}

void StageMachine::enter(StageMachine::State newState) {
  if (state == Deferring) {
    resetTrigger();
  }

  if (newState == Generating) {
    resetGenerator();
  }
  state = newState;
}

void StageMachine::resetTrigger() { triggerWasHigh = false; }

bool StageMachine::triggerRise() {
  auto const isHigh = triggerIsHigh();
  auto const isRise = isHigh && !triggerWasHigh;
  triggerWasHigh = isHigh;
  return isRise;
}

void StageMachine::resetGenerator() {
  startVoltage = envelopeIn();
  stagePhase = 0.F;
}

void StageMachine::generate(float sampleTime) {
  if (stagePhase < 1.F) {
    stagePhase = std::min(1.F, stagePhase + sampleTime / duration());
    sendOut(scale(taper(stagePhase), startVoltage, level()));
    if (stagePhase == 1.F) {
      finishGenerating();
    }
  }
}

void StageMachine::finishGenerating() {
  startEoc();
  enter(TrackingLevel);
}

void StageMachine::startEoc() {
  isEoc = true;
  eocPhase = 0.F;
}

void StageMachine::advanceEoc(float sampleTime) {
  if (eocPhase < 1.F) {
    eocPhase = std::min(1.F, eocPhase + sampleTime / 1e-3F);
    if (eocPhase == 1.F) {
      finishEoc();
    }
  }
}

void StageMachine::finishEoc() { isEoc = false; }
} // namespace dhe