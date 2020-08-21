#pragma once

#include "CommonConfig.h"
#include "components/Range.h"
#include "controls/CommonInputs.h"

#include <engine/Module.hpp>
#include <engine/ParamQuantity.hpp>
#include <functional>
#include <string>

namespace dhe {
class Range;

class LevelKnobParamQuantity : public rack::engine::ParamQuantity {
public:
  auto getDisplayValue() -> float override {
    auto const rotation = getValue();
    return range_()->scale(rotation);
  }

  void setDisplayValue(float display_value) override {
    auto const normalized = range_()->normalize(display_value);
    setValue(normalized);
  }

  void set_range_supplier(std::function<Range const *()> const &supplier) {
    this->range_ = supplier;
  }

private:
  std::function<Range const *()> range_;
};

static inline void
config_level_knob(rack::engine::Module *module, int knob_id,
                  std::function<Range const *()> const &range_supplier,
                  std::string const &name, float initial_position) {
  module->configParam<LevelKnobParamQuantity>(knob_id, 0.F, 1.F,
                                              initial_position, name, " V");
  auto *knob_param_quantity =
      dynamic_cast<LevelKnobParamQuantity *>(module->paramQuantities[knob_id]);
  knob_param_quantity->set_range_supplier(range_supplier);
}

/**
 * Configures the param and display for a level knob with a range selected by a
 * switch.
 */
static inline void
config_level_knob(rack::engine::Module *module, int knob_id, int switch_id,
                  std::string const &name = "Level",
                  float initial_rotation = centered_rotation) {
  auto const range_supplier = [module, switch_id]() -> Range const * {
    return selectedRange<2>(module->params[switch_id], signalRanges);
  };
  config_level_knob(module, knob_id, range_supplier, name, initial_rotation);
}

/**
 * Configures the param and display for a level knob with a fixed range.
 */
static inline void
config_level_knob(rack::engine::Module *module, int knob_id, Range const &range,
                  std::string const &name = "Level",
                  float initial_rotation = centered_rotation) {
  auto const range_supplier = [range]() -> Range const * { return &range; };
  config_level_knob(module, knob_id, range_supplier, name, initial_rotation);
}

/**
 * Configures the param and display for a level range switch.
 */
static inline void
config_level_range_switch(rack::engine::Module *module, int switch_id,
                          std::string const &name = "Level Range",
                          int initial_state = 1) {
  static auto const state_names = std::array<std::string, 2>{"±5 V", "0–10 V"};
  config_toggle<2>(module, switch_id, name, state_names, initial_state);
}
} // namespace dhe
