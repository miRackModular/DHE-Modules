#include <utility>

#include "modules/Stage.h"

#include "util/sigmoid.h"
#include "util/signal.h"

namespace DHE {
Stage::Stage()
    : state_machine{[this]() -> bool { return defer_gate_is_active(); },
                    [this]() -> bool { return defer_gate_in(); },
                    [this]() -> bool { return stage_gate_in(); },
                    [this]() -> float { return duration->seconds(); },
                    [this](float) { forward(); },
                    [this]() { prepare_to_generate(); },
                    [this](float phase) { generate(phase); },
                    [this](bool active) { set_active(active); },
                    [this](bool eoc) { set_eoc(eoc); }} {
  config(PARAMETER_COUNT, INPUT_COUNT, OUTPUT_COUNT);

  configParam(LEVEL_KNOB, 0.f, 1.f, 0.5f, "Level", " V", 0.f, 10.f, 0.f);
  configParam(CURVE_KNOB, 0.f, 1.f, 0.5f, "Curvature", "%", 0.f, 200.f, -100.f);

  Duration::config(this, DURATION_KNOB);

  duration = std::unique_ptr<Duration::Control>(
      new Duration::Control(params[DURATION_KNOB]));

  level = std::unique_ptr<LevelControl>(new LevelControl(params[LEVEL_KNOB]));

  shape = std::unique_ptr<CurvatureControl>(
      new CurvatureControl(params[CURVE_KNOB]));

  state_machine.start();
}

auto Stage::defer_gate_in() -> bool {
  return inputs[DEFER_GATE_IN].getVoltage() > 0.1;
}

auto Stage::defer_gate_is_active() const -> bool {
  return inputs[DEFER_GATE_IN].active;
}

auto Stage::envelope_in() -> float { return inputs[ENVELOPE_IN].getVoltage(); }

void Stage::forward() { send_out(envelope_in()); }

void Stage::generate(float phase) {
  auto const taperedPhase = shape->taper(phase);
  send_out(scale(taperedPhase, start_voltage, level->voltage()));
}

void Stage::prepare_to_generate() { start_voltage = envelope_in(); }

void Stage::send_out(float voltage) { outputs[MAIN_OUT].setVoltage(voltage); }

void Stage::set_active(bool active) {
  const auto voltage = active ? 10.f : 0.f;
  outputs[ACTIVE_OUT].setVoltage(voltage);
}

void Stage::set_eoc(bool eoc) {
  const auto voltage = eoc ? 10.f : 0.f;
  outputs[EOC_OUT].setVoltage(voltage);
}

auto Stage::stage_gate_in() -> bool {
  return inputs[STAGE_TRIGGER_IN].getVoltage() > 0.1;
}

void Stage::process(const ProcessArgs &args) {
  state_machine.step(args.sampleTime);
}
} // namespace DHE
