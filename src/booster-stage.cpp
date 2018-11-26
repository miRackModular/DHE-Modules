
#include "dhe-modules.h"
#include "module-widget.h"

#include "controls/duration.h"
#include "controls/knob.h"
#include "controls/signal.h"
#include "controls/switch.h"
#include "util/d-flip-flop.h"
#include "util/mode.h"
#include "util/phase-accumulator.h"
#include "util/sigmoid.h"

namespace DHE {

struct BoosterStage : rack::Module {
  enum ParameterIds {
    ACTIVE_BUTTON,
    CURVE_KNOB,
    DEFER_BUTTON,
    DURATION_KNOB,
    DURATION_SWITCH,
    EOC_BUTTON,
    LEVEL_KNOB,
    LEVEL_SWITCH,
    SHAPE_SWITCH,
    TRIGGER_BUTTON,
    PARAMETER_COUNT
  };

  enum InputIds {
    CURVE_CV,
    DEFER_IN,
    DURATION_CV,
    LEVEL_CV,
    MAIN_IN,
    TRIGGER_IN,
    INPUT_COUNT
  };

  enum OutputIds { ACTIVE_OUT, EOC_OUT, MAIN_OUT, OUTPUT_COUNT };

  const Knob curve_knob = Knob::with_cv(this, CURVE_KNOB, CURVE_CV);

  const Knob duration_knob = Knob::with_cv(this, DURATION_KNOB, DURATION_CV);
  const Switch<Range> duration_range =
      Duration::range_switch(this, DURATION_SWITCH);

  const Knob level_knob = Knob::with_cv(this, LEVEL_KNOB, LEVEL_CV);
  const Switch<Range> level_range = Signal::range_switch(this, LEVEL_SWITCH);

  float phase_0_voltage{0.f};
  bool is_active{false};
  bool is_eoc{false};

  PhaseAccumulator envelope{[this] { return sample_time() / duration_in(); }};
  PhaseAccumulator eoc_pulse{[this] { return sample_time() / 1e-3f; }};
  DFlipFlop envelope_trigger{[this] { return trigger_in(); }};

  Mode stage_mode{};
  Mode defer_mode{};

  CompoundMode executor{[this] { return defer_in(); },
                        {&stage_mode, &defer_mode}};

  BoosterStage() : Module{PARAMETER_COUNT, INPUT_COUNT, OUTPUT_COUNT} {
    defer_mode.on_entry([this] { is_active = true; });
    defer_mode.on_step([this] { send_main_out(main_in()); });

    stage_mode.on_entry([this] {
      is_active = false;
      phase_0_voltage = main_in();
      envelope_trigger.enable();
    });
    stage_mode.on_step([this] {
      envelope_trigger.step();
      envelope.step();
      send_main_out(envelope_voltage(envelope.phase()));
    });
    stage_mode.on_exit([this] {
      envelope_trigger.disable();
      envelope.stop();
    });

    envelope_trigger.on_rise([this] {
      phase_0_voltage = main_in();
      envelope.start();
    });

    envelope.on_start([this] { is_active = true; });
    envelope.on_completion([this] {
      is_active = false;
      eoc_pulse.start();
    });

    eoc_pulse.on_start([this] { is_eoc = true; });
    eoc_pulse.on_completion([this] { is_eoc = false; });

    executor.on_step([this] { eoc_pulse.step(); });
    executor.enter();
  }

  auto active_in() const -> bool { return params[ACTIVE_BUTTON].value > 0.1f; }

  auto curve_in() const -> float { return Sigmoid::curvature(curve_knob()); }

  auto defer_in() const -> bool {
    auto defer_button = params[DEFER_BUTTON].value > 0.1f;
    auto defer_input = inputs[DEFER_IN].value > 0.1f;
    return defer_button || defer_input;
  }

  auto duration_in() const -> float {
    return Duration::of(duration_knob(), duration_range());
  }

  auto envelope_voltage(float phase) const -> float {
    return scale(taper(phase), phase_0_voltage, level_in());
  }

  auto eoc_in() const -> bool { return params[EOC_BUTTON].value > 0.1f; }

  bool is_s_taper() const { return params[SHAPE_SWITCH].value > 0.1f; }

  auto level_in() const -> float { return level_range().scale(level_knob()); }

  auto main_in() const -> float { return inputs[MAIN_IN].value; }

  auto sample_time() const -> float { return rack::engineGetSampleTime(); }

  void send_active() {
    auto active = is_active || active_in();
    outputs[ACTIVE_OUT].value = active ? 10.f : 0.f;
  }

  void send_main_out(float voltage) { outputs[MAIN_OUT].value = voltage; }

  void send_eoc() {
    auto eoc = is_eoc || eoc_in();
    outputs[EOC_OUT].value = eoc ? 10.f : 0.f;
  }

  void step() override {
    executor.step();
    send_active();
    send_eoc();
  }

  auto taper(float phase) const -> float {
    return is_s_taper() ? Sigmoid::s_taper(phase, curve_in())
                        : Sigmoid::j_taper(phase, curve_in());
  }

  auto trigger_in() const -> bool {
    auto trigger_button = params[TRIGGER_BUTTON].value > 0.1;
    auto trigger_input = inputs[TRIGGER_IN].value > 0.1;
    return trigger_button || trigger_input;
  }
};

struct BoosterStageWidget : public ModuleWidget {
  explicit BoosterStageWidget(rack::Module *module)
      : ModuleWidget(module, 8, "booster-stage") {
    auto widget_right_edge = width();

    auto left_x = widget_right_edge / 6.f + 0.3333333f;
    auto center_x = widget_right_edge / 2.f;
    auto right_x = widget_right_edge - left_x;
    auto button_port_distance = 7.891f;
    auto center_left_x = left_x + button_port_distance;
    auto center_right_x = right_x - button_port_distance;

    auto top_row_y = 25.f;
    auto row_spacing = 18.5f;

    auto row = 0;
    install_input(BoosterStage::LEVEL_CV,
                  {left_x, top_row_y + row * row_spacing});
    install_knob("large", BoosterStage::LEVEL_KNOB,
                 {center_x, top_row_y + row * row_spacing});
    install_switch(BoosterStage::LEVEL_SWITCH,
                   {right_x, top_row_y + row * row_spacing}, 1, 1);

    row++;
    install_input(BoosterStage::CURVE_CV,
                  {left_x, top_row_y + row * row_spacing});
    install_knob("large", BoosterStage::CURVE_KNOB,
                 {center_x, top_row_y + row * row_spacing});
    install_switch(BoosterStage::SHAPE_SWITCH,
                   {right_x, top_row_y + row * row_spacing});

    row++;
    install_input(BoosterStage::DURATION_CV,
                  {left_x, top_row_y + row * row_spacing});
    install_knob("large", BoosterStage::DURATION_KNOB,
                 {center_x, top_row_y + row * row_spacing});
    install_switch(BoosterStage::DURATION_SWITCH,
                   {right_x, top_row_y + row * row_spacing}, 2, 1);

    top_row_y = 82.f;
    row_spacing = 15.f;

    row = 0;
    install_input(BoosterStage::DEFER_IN,
                  {left_x, top_row_y + row * row_spacing});
    install_button("normal", BoosterStage::DEFER_BUTTON,
                   {center_left_x, top_row_y + row * row_spacing});
    install_button("reverse", BoosterStage::ACTIVE_BUTTON,
                   {center_right_x, top_row_y + row * row_spacing});
    install_output(BoosterStage::ACTIVE_OUT,
                   {right_x, top_row_y + row * row_spacing});

    row++;
    install_input(BoosterStage::TRIGGER_IN,
                  {left_x, top_row_y + row * row_spacing});
    install_button("normal", BoosterStage::TRIGGER_BUTTON,
                   {center_left_x, top_row_y + row * row_spacing});
    install_button("reverse", BoosterStage::EOC_BUTTON,
                   {center_right_x, top_row_y + row * row_spacing});
    install_output(BoosterStage::EOC_OUT,
                   {right_x, top_row_y + row * row_spacing});

    row++;
    install_input(BoosterStage::MAIN_IN,
                  {left_x, top_row_y + row * row_spacing});
    install_output(BoosterStage::MAIN_OUT,
                   {right_x, top_row_y + row * row_spacing});
  }
};
} // namespace DHE
rack::Model *modelBoosterStage =
    rack::Model::create<DHE::BoosterStage, DHE::BoosterStageWidget>(
        "DHE-Modules", "BoosterStage", "Booster Stage",
        rack::ENVELOPE_GENERATOR_TAG);
