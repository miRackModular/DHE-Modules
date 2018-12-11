#include <math.h>

#include "dhe-modules.h"
#include "module-widget.h"

#include "display/controls.h"
#include "util/knob.h"
#include "util/sigmoid.h"
#include "util/signal.h"

namespace DHE {

class XycloidRotor {
public:
  void advance(float delta, float offset = 0.f) {
    this->offset = offset;
    phase += delta;
    phase -= std::trunc(phase);
  }

  auto x() const -> float { return std::cos(two_pi * (phase + offset)); }
  auto y() const -> float { return std::sin(two_pi * (phase + offset)); }

private:
  float const two_pi{2.f * std::acos(-1.f)};
  float phase{0.f};
  float offset{0.f};
};

class Xycloid : public rack::Module {
public:
  Xycloid() : Module{PARAMETER_COUNT, INPUT_COUNT, OUTPUT_COUNT} {}

  void set_musical_wobble_ratios(bool is_musical) {
    wobble_ratio_offset = is_musical ? 0.f : 1.f;
  }

  auto is_musical_wobble_ratios() const -> bool {
    return wobble_ratio_offset == 0.f;
  }

  void step() override {
    auto wobble_ratio = this->wobble_ratio();
    auto wobble_phase_offset = wobble_phase_in();
    if (wobble_ratio < 0.f)
      wobble_phase_offset *= -1.f;

    auto throb_speed = this->throb_speed();
    auto wobble_speed = wobble_ratio * throb_speed;
    auto wobble_depth = this->wobble_depth();
    auto throb_depth = 1.f - wobble_depth;

    throbber.advance(throb_speed);
    wobbler.advance(wobble_speed, wobble_phase_offset);
    auto x = throb_depth * throbber.x() + wobble_depth * wobbler.x();
    auto y = throb_depth * throbber.y() + wobble_depth * wobbler.y();

    outputs[X_OUT].value = 5.f * x_gain_in() * (x + x_offset());
    outputs[Y_OUT].value = 5.f * y_gain_in() * (y + y_offset());
  }

  enum ParameterIds {
    WOBBLE_RATIO,
    WOBBLE_RATIO_AV,
    WOBBLE_TYPE,
    WOBBLE_DEPTH,
    WOBBLE_DEPTH_AV,
    THROB_SPEED,
    THROB_SPEED_AV,
    X_GAIN,
    Y_GAIN,
    X_RANGE,
    Y_RANGE,
    WOBBLE_RATIO_TYPE,
    WOBBLE_PHASE,
    PARAMETER_COUNT
  };
  enum InputIds {
    WOBBLE_RATIO_CV,
    WOBBLE_DEPTH_CV,
    THROB_SPEED_CV,
    X_GAIN_CV,
    Y_GAIN_CV,
    INPUT_COUNT
  };
  enum OutputIds { X_OUT, Y_OUT, OUTPUT_COUNT };

private:
  auto is_wobble_ratio_free() const -> bool {
    return params[WOBBLE_RATIO_TYPE].value > 0.1f;
  }

  auto modulated(const ParameterIds &knob_param, const InputIds &cv_input) const
      -> float {
    auto rotation = params[knob_param].value;
    auto cv = inputs[cv_input].value;
    return Knob::modulated(rotation, cv);
  }

  auto modulated(const ParameterIds &knob_param, const InputIds &cv_input,
                 const ParameterIds &av_param) const -> float {
    auto rotation = params[knob_param].value;
    auto cv = inputs[cv_input].value;
    auto av = params[av_param].value;
    return Knob::modulated(rotation, cv, av);
  }

  auto offset(int param) const -> float {
    auto is_uni = params[param].value > 0.5f;
    return is_uni ? 1.f : 0.f;
  }

  auto throb_speed() const -> float {
    constexpr auto speed_taper_curvature = 0.8f;
    auto rotation = modulated(THROB_SPEED, THROB_SPEED_CV, THROB_SPEED_AV);
    auto scaled = throb_speed_knob_range.scale(rotation);
    auto tapered = Sigmoid::inverse(scaled, speed_taper_curvature);
    return -10.f * tapered * rack::engineGetSampleTime();
  }

  auto wobble_depth() const -> float {
    auto rotation = modulated(WOBBLE_DEPTH, WOBBLE_DEPTH_CV, WOBBLE_DEPTH_AV);
    return wobble_depth_range.clamp(rotation);
  }

  auto wobble_phase_in() const -> float {
    auto rotation = params[WOBBLE_PHASE].value;
    return rotation - 0.5f;
  }

  auto wobble_range() const -> const Range & {
    static constexpr auto wobble_max = 16.f;
    static constexpr auto inward_wobble_range = Range{0.f, wobble_max};
    static constexpr auto outward_wobble_range = Range{0.f, -wobble_max};
    static constexpr auto bidirectional_wobble_range =
        Range{wobble_max, -wobble_max};
    static const std::vector<Range> wobble_ratio_ranges{
        inward_wobble_range, bidirectional_wobble_range, outward_wobble_range};

    return wobble_ratio_ranges[wobble_type()];
  }

  auto wobble_ratio() const -> float {
    auto wobble_ammount =
        modulated(WOBBLE_RATIO, WOBBLE_RATIO_CV, WOBBLE_RATIO_AV);
    auto wobble_ratio =
        wobble_range().scale(wobble_ammount) + wobble_ratio_offset;
    return is_wobble_ratio_free() ? wobble_ratio : std::round(wobble_ratio);
  }

  auto wobble_type() const -> int {
    auto param = params[WOBBLE_TYPE].value;
    return static_cast<int>(param);
  }
  auto x_offset() const -> float { return offset(X_RANGE); }

  auto x_gain_in() const -> float {
    return Knob::gain_multiplier(modulated(X_GAIN, X_GAIN_CV));
  }

  auto y_gain_in() const -> float {
    return Knob::gain_multiplier(modulated(Y_GAIN, Y_GAIN_CV));
  }

  auto y_offset() const -> float { return offset(Y_RANGE); }

  json_t *toJson() override {
    json_t *configuration = json_object();
    json_object_set_new(configuration, "musical_wobble_ratios",
                        json_boolean(is_musical_wobble_ratios()));
    return configuration;
  }

  void fromJson(json_t *configuration) override {
    json_t *musical_wobble_ratios =
        json_object_get(configuration, "musical_wobble_ratios");
    set_musical_wobble_ratios(json_is_true(musical_wobble_ratios));
  }

  static constexpr auto throb_speed_knob_range = Range{-1.f, 1.f};
  static constexpr auto wobble_depth_range = Range{0.f, 1.f};

  float wobble_ratio_offset{0.f};
  XycloidRotor wobbler{};

  XycloidRotor throbber{};
};

class XycloidLargeKnob : public LargeKnob {
public:
  XycloidLargeKnob() : LargeKnob("xycloid") {}
};

class XycloidSmallKnob : public SmallKnob {
public:
  XycloidSmallKnob() : SmallKnob("xycloid") {}
};

class XycloidTinyKnob : public TinyKnob {
public:
  XycloidTinyKnob() : TinyKnob("xycloid") {}
};

class XycloidSwitch2 : public ThumbSwitch2 {
public:
  XycloidSwitch2() : ThumbSwitch2{"xycloid"} {}
};

class XycloidSwitch3 : public ThumbSwitch3 {
public:
  XycloidSwitch3() : ThumbSwitch3{"xycloid"} {}
};

struct XycloidWidget : public ModuleWidget<XycloidWidget, Xycloid> {
  static constexpr auto resource_name = "xycloid";

  explicit XycloidWidget(Xycloid *module) : ModuleWidget(module, 11) {
    auto widget_right_edge = width();

    auto column_1 = widget_right_edge / 7.f;
    auto column_4 = widget_right_edge - column_1;
    auto column_2 = (column_4 - column_1) / 3.f + column_1;
    auto column_3 = widget_right_edge - column_2;

    auto y = 30.f;
    auto dy = 22.f;

    install(column_1, y, input_jack(Xycloid::WOBBLE_RATIO_CV));
    install(column_2, y, knob<XycloidTinyKnob>(Xycloid::WOBBLE_RATIO_AV));
    install(column_3, y, knob<XycloidLargeKnob>(Xycloid::WOBBLE_RATIO));
    install(column_4, y,
            thumb_switch<XycloidSwitch2>(Xycloid::WOBBLE_RATIO_TYPE, 1));

    y += dy;
    install(column_1, y, input_jack(Xycloid::WOBBLE_DEPTH_CV));
    install(column_2, y, knob<XycloidTinyKnob>(Xycloid::WOBBLE_DEPTH_AV));
    install(column_3, y, knob<XycloidLargeKnob>(Xycloid::WOBBLE_DEPTH));
    install(column_4, y, thumb_switch<XycloidSwitch3>(Xycloid::WOBBLE_TYPE, 2));

    y += dy;
    install(column_1, y, input_jack(Xycloid::THROB_SPEED_CV));
    install(column_2, y, knob<XycloidTinyKnob>(Xycloid::THROB_SPEED_AV));
    install(column_3, y, knob<XycloidLargeKnob>(Xycloid::THROB_SPEED, 0.65f));
    install(column_4, y, knob<XycloidSmallKnob>(Xycloid::WOBBLE_PHASE));

    y = 82.f;
    dy = 15.f;

    auto default_gain = Knob::gain_range.normalize(1.f);

    y += dy;
    install(column_1, y, input_jack(Xycloid::X_GAIN_CV));
    install(column_2, y, knob<XycloidSmallKnob>(Xycloid::X_GAIN, default_gain));
    install(column_3, y, thumb_switch<XycloidSwitch2>(Xycloid::X_RANGE));
    install(column_4, y, output_jack(Xycloid::X_OUT));

    y += dy;
    install(column_1, y, input_jack(Xycloid::Y_GAIN_CV));
    install(column_2, y, knob<XycloidSmallKnob>(Xycloid::Y_GAIN, default_gain));
    install(column_3, y, thumb_switch<XycloidSwitch2>(Xycloid::Y_RANGE));
    install(column_4, y, output_jack(Xycloid::Y_OUT));
  }

  void appendContextMenu(rack::Menu *menu) override {
    auto xycloid = dynamic_cast<Xycloid *>(module);
    assert(xycloid);

    menu->addChild(rack::construct<rack::MenuLabel>());
    menu->addChild(
        rack::construct<rack::MenuLabel>(&rack::MenuLabel::text, "Options"));
    menu->addChild(new BooleanOption(
        "Musical Ratios",
        [xycloid](bool setting) {
          xycloid->set_musical_wobble_ratios(setting);
        },
        [xycloid] { return xycloid->is_musical_wobble_ratios(); }));
  }
};

} // namespace DHE
rack::Model *modelXycloid =
    rack::Model::create<DHE::Xycloid, DHE::XycloidWidget>(
        "DHE-Modules", "Xycloid", "Xycloid", rack::FUNCTION_GENERATOR_TAG,
        rack::LFO_TAG);
