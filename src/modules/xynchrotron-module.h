#pragma once

#include <util/controls.h>
#include "module.h"
namespace DHE {

struct XynchrotronModule : Module {
  float roll_angle = 0.f;
  float pole_angle = 0.f;
  XynchrotronModule() : Module{PARAMETER_COUNT, INPUT_COUNT, OUTPUT_COUNT} {
  }

  static const Range &gain_range() {
    static constexpr auto gain_range = Range{0.f, 2.f};
    return gain_range;
  }

  float length(int knob, int cv, int av) {
    static auto length_range = Range{1e-2f, 1.f};
    return length_range.clamp(modulated(knob, cv, av));
  }

  float gain(int knob, int cv) {
    return gain_range().scale(modulated(knob, cv));
  }

  float period() {
    auto speed_rotation = modulated(ZING_KNOB, ZING_CV, ZING_CV_ATTENUVERTER);
    auto period_rotation = DHE::UNIPOLAR_PHASE_RANGE.clamp(1.f - speed_rotation);
    return Duration::scaled(period_rotation, Duration::MEDIUM_RANGE);
  }

  float increment_angle(float angle, float multiplier = 1.f) {
    static auto two_pi = 2.f*std::acos(-1.f);
    angle += multiplier*two_pi*rack::engineGetSampleTime()/period();
    if (angle > two_pi) angle -= two_pi;
    return angle;
  }

  void step() override {
    auto base_radius = length(ROCK_KNOB, ROCK_CV, ROCK_CV_ATTENUVERTER);
    auto roller_radius = length(ROLL_KNOB, ROLL_CV, ROLL_CV_ATTENUVERTER);
    auto pole_length = length(CURL_KNOB, CURL_CV, CURL_CV_ATTENUVERTER);

    auto position = param(ROLL_POSITION_SWITCH) > 0.5f ? 1.f : -1.f;
    auto roll_radius = std::abs(base_radius + position*roller_radius);

    auto direction = param(DIRECTION_SWITCH) > 0.5f ? 1.f : -1.f;
    roll_angle = increment_angle(roll_angle, direction);
    auto pole_angle_multiplier = direction*roll_radius/roller_radius;
    pole_angle = increment_angle(pole_angle, pole_angle_multiplier);

    auto x = roll_radius*std::cos(roll_angle) - position*pole_length*std::cos(pole_angle);
    auto y = roll_radius*std::sin(roll_angle) - pole_length*std::sin(pole_angle);

    auto roulette_radius = roll_radius + pole_length;
    auto roulette_scale = 5.f/roulette_radius;
    auto x_gain = gain(X_GAIN_KNOB, X_GAIN_CV);
    auto y_gain = gain(Y_GAIN_KNOB, Y_GAIN_CV);
    auto x_offset = param(X_RANGE_SWITCH) > 0.5f ? 5.f : 0.f;
    auto y_offset = param(Y_RANGE_SWITCH) > 0.5f ? 5.f : 0.f;
    outputs[X_OUT].value = x_gain*(x*roulette_scale + x_offset);
    outputs[Y_OUT].value = y_gain*(y*roulette_scale + y_offset);
  }
  enum ParameterIds {
    ROCK_KNOB,
    ROCK_CV_ATTENUVERTER,
    ROLL_KNOB,
    DIRECTION_SWITCH,
    ROLL_CV_ATTENUVERTER,
    ROLL_POSITION_SWITCH,
    CURL_KNOB,
    CURL_CV_ATTENUVERTER,
    ZING_KNOB,
    ZING_CV_ATTENUVERTER,
    X_GAIN_KNOB,
    Y_GAIN_KNOB,
    X_RANGE_SWITCH,
    Y_RANGE_SWITCH,
    PARAMETER_COUNT
  };
  enum InputIds {
    ROCK_CV,
    ROLL_CV,
    CURL_CV,
    ZING_CV,
    X_GAIN_CV,
    Y_GAIN_CV,
    INPUT_COUNT
  };
  enum OutputIds {
    X_OUT,
    Y_OUT,
    OUTPUT_COUNT
  };
};

}
