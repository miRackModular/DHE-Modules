#include "modules/envelopes/StateMachine.h"

namespace dhe {

StateMachine::StateMachine(std::function<bool()> defer_gate_is_active,
                           std::function<bool()> defer_gate_is_up,
                           std::function<bool()> const &stage_gate_is_up,
                           std::function<void()> const &start_generating,
                           std::function<void(bool)> const &set_active,
                           std::function<void(bool)> const &set_eoc,
                           std::function<void(float)> const &forward)
    : deferGateIsActive{std::move(defer_gate_is_active)},
      stageGateIsUp{stage_gate_is_up}, startGenerating{start_generating},
      deferring{forward, set_active}, forwarding{start_generating, forward,
                                                 set_active},
      idling{start_generating, set_active}, deferGate{
                                                std::move(defer_gate_is_up),
                                                [this]() { enter(&deferring); },
                                                [this]() { stopDeferring(); }},
      eocGenerator{[set_eoc]() { set_eoc(true); },
                   [set_eoc]() { set_eoc(false); }},
      stageGate{stage_gate_is_up, [this]() { onStageGateRise(); },
                [this]() { onStageGateFall(); }} {}

void StateMachine::enter(StageState *incoming) {
  state = incoming;
  state->enter();
}

void StateMachine::finishStage() {
  eocGenerator.start();
  enter(&idling);
}

void StateMachine::onStageGateFall() { state->onStageGateFall(); }

void StateMachine::onStageGateRise() {
  // If DEFER is active, ignore GATE rises.
  // We will check GATE when DEFER falls.
  if (deferGateIsActive())
    return;
  state->onStageGateRise();
}

void StateMachine::start() { state->enter(); }

void StateMachine::step(float sampleTime) {
  deferGate.step();
  stageGate.step();
  state->step(sampleTime);
  eocGenerator.step(sampleTime);
}

void StateMachine::stopDeferring() {
  if (stageGateIsUp()) {
    startGenerating();
  } else {
    finishStage();
  }
}
} // namespace dhe
