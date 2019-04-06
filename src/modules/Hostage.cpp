#include <modules/Hostage.h>

#include "util/duration.h"
#include "util/rotation.h"

namespace DHE {
Hostage::Hostage() : rack::Module{PARAMETER_COUNT, INPUT_COUNT, OUTPUT_COUNT} {
  state_machine.start();
}

void Hostage::step() { state_machine.step(); }

auto Hostage::duration() const -> float {
  auto rotation = modulated(DURATION_KNOB, DURATION_CV);
  return DHE::duration(rotation, *duration_range);
}

void Hostage::forward() { send_out(envelope_in()); }

auto Hostage::defer_gate_is_active() const -> bool {
  return inputs[DEFER_GATE_IN].active;
}

auto Hostage::is_sustain_mode() const -> bool {
  return params[HOSTAGE_MODE_SWITCH].value > 0.5f;
}

void Hostage::set_active(bool active) {
  outputs[ACTIVE_OUT].value = active ? 10.f : 0.f;
}

void Hostage::set_eoc(bool eoc) { outputs[EOC_OUT].value = eoc ? 10.f : 0.f; }

auto Hostage::sample_time() const -> float {
  return rack::engineGetSampleTime();
}

auto Hostage::defer_gate_in() const -> bool {
  return inputs[DEFER_GATE_IN].value > 0.1f;
}

auto Hostage::stage_gate_in() const -> bool {
  return inputs[STAGE_GATE_IN].value > 0.1f;
}

auto Hostage::envelope_in() const -> float { return inputs[MAIN_IN].value; }

auto Hostage::modulated(Hostage::ParameterIds knob_param,
                        Hostage::InputIds cv_input) const -> float {
  auto rotation = params[knob_param].value;
  auto cv = inputs[cv_input].value;
  return Rotation::modulated(rotation, cv);
}

void Hostage::send_out(float voltage) {
  outputs[MAIN_OUT].value = voltage;
}
} // namespace DHE
