#pragma once

#include "CommonConfig.h"
#include "components/Taper.h"
#include "controls/CommonInputs.h"
#include "controls/DurationInputs.h"

#include <array>
#include <engine/Module.hpp>
#include <engine/ParamQuantity.hpp>
#include <functional>

namespace dhe {
class Range;
class DurationKnobParamQuantity : public rack::engine::ParamQuantity {
public:
  auto getDisplayValue() -> float override {
    auto const rotation = getValue();
    auto const tapered = durationKnobTaper.apply(rotation);
    return range_()->scale(tapered);
  }

  void setDisplayValue(float duration_seconds) override {
    auto const tapered = range_()->normalize(duration_seconds);
    auto const rotation = durationKnobTaper.invert(tapered);
    setValue(rotation);
  }

  void
  set_range_supplier(std::function<Range const *()> const &range_supplier) {
    this->range_ = range_supplier;
  }

private:
  std::function<Range const *()> range_;
};

static inline void
config_duration_knob(rack::engine::Module *module, int knob_id,
                     std::function<Range const *()> const &range_supplier,
                     std::string const &name, float initial_position) {
  module->configParam<DurationKnobParamQuantity>(knob_id, 0.F, 1.F,
                                                 initial_position, name, " s");
  auto *knob_param_quantity = dynamic_cast<DurationKnobParamQuantity *>(
      module->paramQuantities[knob_id]);
  knob_param_quantity->set_range_supplier(range_supplier);
}

/**
 * Configures the param and display for a duration knob with a fixed range.
 */
static inline void
config_duration_knob(rack::engine::Module *module, int knob_id,
                     Range const &range, std::string const &name = "Duration",
                     float initial_rotation = centered_rotation) {
  auto const range_supplier = [range]() -> Range const * { return &range; };
  config_duration_knob(module, knob_id, range_supplier, name, initial_rotation);
}

/**
 * Configures the param and display for a duration knob with a range selected by
 * a switch.
 */
static inline void
config_duration_knob(rack::engine::Module *module, int knob_id, int switch_id,
                     std::string const &name = "Duration",
                     float initial_rotation = centered_rotation) {
  auto const range_supplier = [module, switch_id]() -> Range const * {
    return selected_range<3>(module->params[switch_id], durationRanges);
  };
  config_duration_knob(module, knob_id, range_supplier, name, initial_rotation);
}

/**
 * Configures the param and display for a duration range switch.
 */
static inline void
config_duration_range_switch(rack::engine::Module *module, int switch_id,
                             std::string const &name = "Duration Range",
                             int initial_position = 1) {
  static auto const position_names =
      std::array<std::string, 3>{"0.001–1.0 s", "0.01–10.0 s", "0.1–100.0 s"};
  config_toggle<3>(module, switch_id, name, position_names, initial_position);
}
} // namespace dhe
