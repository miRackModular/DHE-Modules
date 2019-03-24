#include <engine.hpp>

#include "display/controls.h"
#include "display/panel.h"
#include "stage/components/defer-gate.h"
#include "stage/components/eoc-generator.h"
#include "stage/components/stage-gate.h"
#include "stage/components/stage-generator.h"
#include "stage/modes/deferring-mode.h"
#include "stage/modes/following-mode.h"
#include "stage/modes/generating-mode.h"
#include "util/duration.h"
#include "util/signal.h"

namespace DHE {

class Stage : public rack::Module {
public:
  Stage()
      : Module{PARAMETER_COUNT, INPUT_COUNT, OUTPUT_COUNT},
        mode{&following_mode} {
    mode->enter();
  }

  void step() override {
    defer_gate.step();
    mode->step();
    eoc_generator.step();
  }

  auto duration() const -> float {
    auto rotation = params[DURATION_KNOB].value;
    return DHE::duration(rotation);
  }
  auto sampleTime() const -> float {
    return rack::engineGetSampleTime();
  }

  auto defer_gate_in() const -> bool { return inputs[DEFER_GATE_IN].value > 0.1; }
  void on_defer_gate_rise() { enter(&deferring_mode); }
  void do_defer() { send_input(); }
  void on_defer_gate_fall() { enter(&following_mode); }

  void do_follow() { send_level(); }

  auto stage_gate_in() const -> bool { return inputs[STAGE_TRIGGER_IN].value > 0.1; }
  void on_stage_gate_rise() { enter(&generating_mode); }
  void on_stage_gate_fall() {}

  void on_generate_start() {}
  void do_generate() { stage_generator.step(); }
  void on_generate_end() {
    eoc_generator.start();
    enter(&following_mode);
  }

  void on_eoc_start() { set_eoc(true); }
  void on_eoc_end() { set_eoc(false); }

  void hold_input() { held_voltage = envelope_in(); }
  void send_input() { send_out(envelope_in()); }
  void send_level() { send_out(level()); }
  void send_phase(float phase) {
    send_out(scale(taper(phase), held_voltage, level()));
  }

  void set_active(bool active) {
    outputs[ACTIVE_OUT].value = active ? 10.f : 0.f;
  }

  enum ParameterIIds { DURATION_KNOB, LEVEL_KNOB, CURVE_KNOB, PARAMETER_COUNT };

  enum InputIds { ENVELOPE_IN, STAGE_TRIGGER_IN, DEFER_GATE_IN, INPUT_COUNT };

  enum OutputIds { MAIN_OUT, EOC_OUT, ACTIVE_OUT, OUTPUT_COUNT };

private:
  auto curvature() const -> float {
    auto rotation = params[CURVE_KNOB].value;
    return Sigmoid::curvature(rotation);
  }

  auto envelope_in() const -> float { return inputs[ENVELOPE_IN].value; }

  void enter(Mode *incoming) {
    mode->exit();
    mode = incoming;
    mode->enter();
  }

  auto level() const -> float {
    auto rotation = params[LEVEL_KNOB].value;
    return Signal::unipolar_range.scale(rotation);
  }

  void set_eoc(bool eoc) { outputs[EOC_OUT].value = eoc ? 10.f : 0.f; }

  void send_out(float voltage) { outputs[MAIN_OUT].value = voltage; }

  auto taper(float phase) const -> float {
    return Sigmoid::j_taper(phase, curvature());
  }

  DeferGate<Stage> defer_gate{this};
  StageGate<Stage> stage_gate{this};

  StageGenerator<Stage> stage_generator{this};
  EocGenerator<Stage> eoc_generator{this};

  DeferringMode<Stage> deferring_mode{this};
  FollowingMode<Stage> following_mode{this};
  GeneratingMode<Stage> generating_mode{this};
  Mode *mode;

  float held_voltage = 0.f;
};

class StagePanel : public Panel<StagePanel> {
public:
  explicit StagePanel(Stage *module) : Panel{module, hp} {
    auto widget_right_edge = width();

    auto column_1 = width() / 4.f + 0.333333f;
    auto column_2 = widget_right_edge / 2.f;
    auto column_3 = widget_right_edge - column_1;

    auto y = 25.f;
    auto dy = 18.5f;

    install(column_2, y, knob<LargeKnob>(Stage::LEVEL_KNOB));

    y += dy;
    install(column_2, y, knob<LargeKnob>(Stage::CURVE_KNOB));

    y += dy;
    install(column_2, y, knob<LargeKnob>(Stage::DURATION_KNOB));

    y = 82.f;
    dy = 15.f;

    install(column_1, y, input(Stage::DEFER_GATE_IN));
    install(column_3, y, output(Stage::ACTIVE_OUT));

    y += dy;
    install(column_1, y, input(Stage::STAGE_TRIGGER_IN));
    install(column_3, y, output(Stage::EOC_OUT));

    y += dy;
    install(column_1, y, input(Stage::ENVELOPE_IN));
    install(column_3, y, output(Stage::MAIN_OUT));
  }

  static constexpr auto module_slug = "stage";

private:
  static constexpr auto hp = 5;
};
} // namespace DHE
rack::Model *modelStage = rack::Model::create<DHE::Stage, DHE::StagePanel>(
    "DHE-Modules", "Stage", "Stage", rack::ENVELOPE_GENERATOR_TAG);
