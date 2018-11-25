#include <utility>

#include "dhe-modules.h"
#include "module-widget.h"
#include "module.h"

#include "util/knob.h"
#include "util/range.h"
#include "util/signal.h"

namespace DHE {

struct RangerLevel {
  const Knob knob;

  explicit RangerLevel(Knob knob) : knob{std::move(knob)} {}

  auto operator()(float limit1, float limit2) const -> float {
    return scale(knob(), limit1, limit2);
  }
};

struct RangerLimit {
  const Knob knob;
  const rack::Param &range_selector;

  RangerLimit(Knob knob, const rack::Param &range_selector)
      : knob{std::move(knob)}, range_selector{range_selector} {}

  auto operator()() const -> float {
    auto range = range_selector.value > 0.1 ? Signal::unipolar_range
                                            : Signal::bipolar_range;
    return range.scale(knob());
  }
};

struct Ranger : Module {
  enum ParameterIds {
    LEVEL_KNOB,
    LEVEL_AV,
    LIMIT_1_KNOB,
    LIMIT_1_AV,
    LIMIT_1_RANGE,
    LIMIT_2_KNOB,
    LIMIT_2_AV,
    LIMIT_2_RANGE,
    PARAMETER_COUNT
  };
  enum InputIds { LEVEL_CV, LIMIT_1_CV, LIMIT_2_CV, INPUT_COUNT };
  enum OutputIds { MAIN_OUT, OUTPUT_COUNT };

  RangerLevel level{Knob::modulated(this, LEVEL_KNOB, LEVEL_CV, LEVEL_AV)};
  RangerLimit upper{Knob::modulated(this, LIMIT_1_KNOB, LIMIT_1_CV, LIMIT_1_AV),
                    params[LIMIT_1_RANGE]};
  RangerLimit lower{Knob::modulated(this, LIMIT_2_KNOB, LIMIT_2_CV, LIMIT_2_AV),
                    params[LIMIT_2_RANGE]};

  Ranger() : Module{PARAMETER_COUNT, INPUT_COUNT, OUTPUT_COUNT} {}

  void send_main_out(float voltage) { outputs[MAIN_OUT].value = voltage; }

  void step() override { send_main_out(level(lower(), upper())); }
};

struct RangerWidget : public ModuleWidget {
  explicit RangerWidget(rack::Module *module)
      : ModuleWidget(module, 6, "ranger") {
    auto widget_right_edge = width();

    auto left_x = width() / 3.5f + 0.333333333f;
    auto right_x = widget_right_edge - left_x;

    auto y = 24.f;
    auto delta_y = 16.f;
    auto panel_buffer = 4.f;

    install_knob("medium", Ranger::LEVEL_KNOB, {left_x, y});
    install_output(Ranger::MAIN_OUT, {right_x, y});
    y += delta_y;
    install_input(Ranger::LEVEL_CV, {left_x, y});
    install_knob("tiny", Ranger::LEVEL_AV, {right_x, y});

    y += delta_y + panel_buffer;

    install_knob("medium", Ranger::LIMIT_1_KNOB, {left_x, y});
    install_switch(Ranger::LIMIT_1_RANGE, {right_x, y});
    y += delta_y;
    install_input(Ranger::LIMIT_1_CV, {left_x, y});
    install_knob("tiny", Ranger::LIMIT_1_AV, {right_x, y});

    y += delta_y + panel_buffer;

    install_knob("medium", Ranger::LIMIT_2_KNOB, {left_x, y});
    install_switch(Ranger::LIMIT_2_RANGE, {right_x, y});
    y += delta_y;
    install_input(Ranger::LIMIT_2_CV, {left_x, y});
    install_knob("tiny", Ranger::LIMIT_2_AV, {right_x, y});
  }
};
} // namespace DHE

rack::Model *modelRanger = rack::Model::create<DHE::Ranger, DHE::RangerWidget>(
    "DHE-Modules", "Ranger", "Ranger", rack::UTILITY_TAG, rack::WAVESHAPER_TAG);
