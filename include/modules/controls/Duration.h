#pragma once

#include "Controls.h"
#include "util/Range.h"

#include <array>
#include <engine/Module.hpp>
#include <functional>

namespace dhe {
namespace duration {
  extern Range const shortRange;
  extern Range const mediumRange;
  extern Range const longRange;

  extern std::array<Range const *, 3> const ranges;

  /**
   * Creates a function that yields the duration (in seconds) selected by a knob from the medium duration range.
   */
  auto withMediumRange(rack::engine::Module *module, int knobId) -> std::function<float()>;

  /**
   * Creates a function that yields the duration (in seconds) selected by a modulated knob and a duration range switch.
   * The amount of modulation is determined by the voltage of a CV input.
   */
  auto withSelectableRange(rack::engine::Module *module, int knobId, int cvId, int switchId) -> std::function<float()>;

  void configKnob(rack::engine::Module *module, int knobId, Range const & /*range*/,
                  std::string const &name = "Duration", float initialRotation = knob::centered);

  void configKnob(rack::engine::Module *module, int knobId, int switchId, std::string const &name = "Duration",
                  float initialRotation = knob::centered);

  void configSwitch(rack::engine::Module *module, int switchId, std::string const &name = "Duration Range",
                    int initialPosition = 1);

} // namespace duration
} // namespace dhe
