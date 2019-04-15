#pragma once

#include "modules/Module.h"

#include "components/rotor.h"
#include "util/range.h"

namespace DHE {

class Xycloid : public Module {
public:
  Xycloid();

  auto is_musical_wobble_ratios() const -> bool;
  void set_musical_wobble_ratios(bool is_musical);
  void step() override;

  enum ParameterIds {
    WOBBLE_RATIO_KNOB,
    WOBBLE_RATIO_AV,
    WOBBLE_RANGE_SWITCH,
    WOBBLE_DEPTH_KNOB,
    WOBBLE_DEPTH_AV,
    THROB_SPEED_KNOB,
    THROB_SPEED_AV,
    X_GAIN_KNOB,
    Y_GAIN_KNOB,
    X_RANGE_SWITCH,
    Y_RANGE_SWITCH,
    WOBBLE_RATIO_FREEDOM_SWITCH,
    WOBBLE_PHASE_KNOB,
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
  auto is_wobble_ratio_free() const -> bool;
  auto offset(int param) const -> float;
  auto throb_speed() const -> float;
  auto wobble_depth() const -> float;
  auto wobble_phase_in() const -> float;
  auto wobble_ratio_range() const -> const Range &;
  auto wobble_ratio() const -> float;
  auto x_offset() const -> float;
  auto x_gain_in() const -> float;
  auto y_gain_in() const -> float;
  auto y_offset() const -> float;
  json_t *toJson() override;
  void fromJson(json_t *configuration) override;

  static constexpr auto throb_speed_knob_range = Range{-1.f, 1.f};
  static constexpr auto wobble_depth_range = Range{0.f, 1.f};

  float wobble_ratio_offset{0.f};
  Rotor wobbler{};
  Rotor throbber{};
};
} // namespace DHE