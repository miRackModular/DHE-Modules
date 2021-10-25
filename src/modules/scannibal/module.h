#pragma once

#include "anchor.h"
#include "control-ids.h"
#include "controller.h"
#include "generator.h"
#include "params.h"

#include "components/cxmath.h"
#include "params/curvature-config.h"
#include "params/duration-config.h"
#include "params/level-config.h"
#include "params/presets.h"
#include "signals/curvature-inputs.h"
#include "signals/duration-inputs.h"
#include "signals/level-inputs.h"

#include "rack.hpp"

namespace dhe {
namespace scannibal {

static auto constexpr brightness_range = Range{0.F, 1.F};

template <typename P>
static inline auto level(P const &level_knob, P const &range_switch) -> float {
  auto const range = level_range(range_switch);
  auto const rotation = dhe::rotation_of(level_knob);
  return range.scale(rotation);
}

struct RelativeDuration : public LinearKnob<RelativeDuration> {
  static auto constexpr range = Range{0.F, 2.F};
  static auto constexpr initial = 1.F;
  static auto constexpr unit = "";
};

template <int N> class Module : public rack::engine::Module {
  using Input = InputIds<N>;
  using Light = LightIds<N>;
  using Output = OutputIds;
  using Param = ParamIds<N>;

public:
  Module() {
    config(Param::Count, Input::Count, Output::Count, Light::Count);

    auto step_knob = configParam(Param::Length, 1.F, N, N, "Steps", "");
    step_knob->snapEnabled = true;

    config_level_range_switch(this, Param::LevelRange);

    configInput(Input::InA, "A");
    configInput(Input::InB, "B");
    configInput(Input::InC, "C");
    configInput(Input::Phase, "Phase");

    configOutput(Output::StepNumber, "Step number");
    configOutput(Output::StepPhase, "Step phase");
    configOutput(Output::Out, "Scanner");

    for (auto step = 0; step < N; step++) {
      auto const step_name = "Step " + std::to_string(step + 1) + " ";
      configLight(Light::Progress + step + step, step_name + "phase");
      Stepper<AnchorSources>::config(this, Param::StepPhase0AnchorSource + step,
                                     step_name + "phase 0 anchor source",
                                     AnchorSource::Out);
      config_level_knob(this, Param::StepPhase0AnchorLevel + step,
                        Param::LevelRange, step_name + "phase 0 level");
      configInput(Input::StepPhase0AnchorLevelCv + step,
                  step_name + "phase 0 level CV");
      Stepper<AnchorModes>::config(this, Param::StepPhase0AnchorMode + step,
                                   step_name + "phase 0 anchor mode",
                                   AnchorMode::Sample);

      Stepper<AnchorSources>::config(this, Param::StepPhase1AnchorSource + step,
                                     step_name + "phase 1 anchor source",
                                     AnchorSource::Level);
      config_level_knob(this, Param::StepPhase1AnchorLevel + step,
                        Param::LevelRange, step_name + "phase 1 level");
      configInput(Input::StepPhase1AnchorLevelCv + step,
                  step_name + "phase 1 level CV");
      Stepper<AnchorModes>::config(this, Param::StepPhase1AnchorMode + step,
                                   step_name + "phase 1 anchor mode",
                                   AnchorMode::Track);

      config_curve_shape_switch(this, Param::StepShape + step,
                                step_name + "shape");
      config_curvature_knob(this, Param::StepCurvature + step,
                            step_name + "curvature");
      configInput(Input::StepCurvatureCv + step, step_name + "curvature CV");

      RelativeDuration::config(this, Param::StepDuration + step,
                               step_name + "relative duration");
      configInput(Input::StepDurationCv + step,
                  step_name + "relative duration CV");
    }
  }

  ~Module() override = default;

  void process(ProcessArgs const & /*args*/) override { controller_.execute(); }

  auto anchor_mode(AnchorType type, int step) const -> AnchorMode {
    auto const base = type == AnchorType::Phase0 ? Param::StepPhase0AnchorMode
                                                 : Param::StepPhase1AnchorMode;
    auto const selection = position_of(params[base + step]);
    return static_cast<AnchorMode>(selection);
  }

  auto anchor_level(AnchorType type, int step) const -> float {
    auto const base_knob_param = type == AnchorType::Phase0
                                     ? Param::StepPhase0AnchorLevel
                                     : Param::StepPhase1AnchorLevel;
    return scannibal::level(params[base_knob_param + step],
                            params[Param::LevelRange]);
  }

  auto anchor_source(AnchorType type, int step) const -> AnchorSource {
    auto const base = type == AnchorType::Phase0
                          ? Param::StepPhase0AnchorSource
                          : Param::StepPhase1AnchorSource;
    auto const selection = position_of(params[base + step]);
    return static_cast<AnchorSource>(selection);
  }

  auto curvature(int step) const -> float {
    return dhe::curvature(params[Param::StepCurvature + step],
                          inputs[Input::StepCurvatureCv + step]);
  }

  auto duration(int step) const -> float {
    return cx::clamp(dhe::rotation(params[Param::StepDuration + step],
                                   inputs[Input::StepDurationCv + step]),
                     0.F, 1.F);
  }

  void exit_step(int step) { set_lights(step, 0.F, 0.F); }

  auto in_a() const -> float { return voltage_at(inputs[Input::InA]); }

  auto in_b() const -> float { return voltage_at(inputs[Input::InB]); }

  auto in_c() const -> float { return voltage_at(inputs[Input::InC]); }

  auto length() const -> float { return value_of(params[Param::Length]); }

  auto phase() const -> float { return voltage_at(inputs[Input::Phase]); }

  auto output() const -> float { return voltage_at(outputs[Output::Out]); }

  void output(float voltage) { outputs[Output::Out].setVoltage(voltage); }

  void show_position(int step, float phase) {
    auto const completed_brightness = brightness_range.scale(phase);
    auto const remaining_brightness = 1.F - completed_brightness;
    set_lights(step, completed_brightness, remaining_brightness);
    outputs[Output::StepNumber].setVoltage(static_cast<float>(step + 1) * 10.F /
                                           static_cast<float>(N));
    outputs[Output::StepPhase].setVoltage(phase * 10.F);
  }

  auto taper(int step) const -> sigmoid::Taper const & {
    auto const selection = position_of(params[Param::StepShape + step]);
    return sigmoid::tapers[selection];
  }

  auto dataToJson() -> json_t * override {
    auto *data = json_object();
    json_object_set_new(data, preset_version_key, json_integer(0));
    return data;
  }

private:
  using AnchorT = Anchor<Module>;
  using GeneratorT = Generator<Module, AnchorT>;
  using ControllerT = Controller<Module, GeneratorT, N>;

  AnchorT phase_0_anchor_{*this, AnchorType::Phase0};
  AnchorT phase_1_anchor_{*this, AnchorType::Phase1};
  GeneratorT generator_{*this, phase_0_anchor_, phase_1_anchor_};
  ControllerT controller_{*this, generator_};

  void set_lights(int step, float completed_brightness,
                  float remaining_brightness) {
    auto const completed_light = Light::Progress + step + step;
    auto const remaining_light = completed_light + 1;
    lights[completed_light].setBrightness(completed_brightness);
    lights[remaining_light].setBrightness(remaining_brightness);
  }
};
} // namespace scannibal

} // namespace dhe
