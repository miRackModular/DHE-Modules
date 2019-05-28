#include "modules/controls/Controls.h"

#include "util/Range.h"

namespace dhe {

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Local functions
//
///////////////////////////////////////////////////////////////////////////////////////////////////

template <int N> class FrameWidgetParamQuantity : public rack::engine::ParamQuantity {
public:
  void setFrameNames(std::array<std::string, N> const &names) { frameNames = names; }
  auto getDisplayValueString() -> std::string override {
    auto const frame = static_cast<int>(getValue());
    return frameNames[frame];
  }

private:
  std::array<std::string, N> frameNames;
};

/**
 * Configures the display for a frame widget that represents a sequence of states.
 */
template <int N>
void configFrameWidgetStates(rack::engine::Module *module, int paramId, std::string const &paramName,
                             std::array<std::string, N> const &stateNames, int initialState) {
  module->configParam<FrameWidgetParamQuantity<N>>(paramId, 0.F, (float) (N - 1), (float) initialState, paramName);
  auto const controlDisplay = dynamic_cast<FrameWidgetParamQuantity<N> *>(module->paramQuantities[paramId]);
  controlDisplay->setFrameNames(stateNames);
}

/**
 * Creates a function that scales the supplied rotation to the given range.
 */
static inline auto scaled(std::function<float()> const &rotation, Range const &range) -> std::function<float()> {
  return [rotation, range]() { return range.scale(rotation()); };
}

/**
 * Creates a function that scales the supplied rotation to the supplied range.
 */
static inline auto scaled(std::function<float()> const &rotation, std::function<Range const *()> const &range)
    -> std::function<float()> {
  return [rotation, range]() { return range()->scale(rotation()); };
}

/**
 * Creates a function that tapers the supplied rotation and scales the result to the given range.
 */
static inline auto taperedAndScaled(std::function<float()> const &rotation, taper::FixedTaper const &taper,
                                    Range const &range) -> std::function<float()> {
  return [rotation, range, &taper]() {
    auto const tapered = taper.apply(rotation());
    auto const scaled = range.scale(tapered);
    return scaled;
  };
}

/**
 * Creates a function that tapers the supplied rotation and scales the result to the supplied range.
 */
static inline auto taperedAndScaled(std::function<float()> const &rotation, taper::FixedTaper const &taper,
                                    std::function<Range const *()> const &range) -> std::function<float()> {
  return [rotation, range, &taper]() {
    auto const tapered = taper.apply(rotation());
    auto const scaled = range()->scale(tapered);
    return scaled;
  };
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Public functions
//
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace attenuator {
  const auto range = Range{0.F, 1.F};

  void config(rack::engine::Module *module, int knobId, std::string const &knobName) {
    knob::configPercentage(module, knobId, knobName, range);
  }
} // namespace attenuator

namespace attenuverter {
  const auto range = Range{-1.F, 1.F};

  void config(rack::engine::Module *module, int knobId, std::string const &knobName) {
    knob::configPercentage(module, knobId, knobName, range);
  }
} // namespace attenuverter

namespace button {
  void config(rack::engine::Module *module, int buttonId, std::string const &buttonName,
              std::array<std::string, 2> const &stateNames, int initialState) {
    configFrameWidgetStates<2>(module, buttonId, buttonName, stateNames, initialState);
  }

  auto state(rack::engine::Module *module, int buttonId) -> std::function<bool()> {
    return [module, buttonId]() -> bool { return module->params[buttonId].getValue() > 0.5F; };
  }
} // namespace button

namespace gain {
  const auto range = Range{-0.F, 2.F};

  void config(rack::engine::Module *module, int knobId, std::string const &knobName) {
    knob::configPercentage(module, knobId, knobName, range);
  }
} // namespace gain

namespace input {
  auto isConnected(rack::engine::Module *module, int inputId) -> std::function<bool()> {
    return [module, inputId]() -> bool { return module->inputs[inputId].isConnected() > 1.F; };
  }

  auto isHigh(rack::engine::Module *module, int inputId) -> std::function<bool()> {
    return [module, inputId]() -> bool { return module->inputs[inputId].getVoltage() > 1.F; };
  }

  auto isHigh(rack::engine::Module *module, int inputId, int buttonId) -> std::function<bool()> {
    return [module, inputId, buttonId]() -> bool {
      return module->inputs[inputId].getVoltage() > 1.F || module->params[buttonId].getValue() > 0.5F;
    };
  }
} // namespace input

namespace knob {
  const auto rotationRange = Range{0.F, 1.F};

  void config(rack::engine::Module *module, int knobId, std::string const &knobName, std::string const &units,
              Range const &range, float initialRotation) {
    module->configParam(knobId, 0.F, 1.F, initialRotation, knobName, units, 0.F, range.size(), range.lowerBound);
  }

  void configPercentage(rack::engine::Module *module, int knobId, std::string const &knobName, Range const &range) {
    module->configParam(knobId, 0.F, 1.F, 0.5F, knobName, "%", 0.F, range.size() * 100.F, range.lowerBound * 100.F);
  }

  auto rotation(rack::engine::Module *module, int knobId) -> std::function<float()> {
    return [module, knobId]() -> float { return module->params[knobId].getValue(); };
  }

  auto rotation(rack::engine::Module *module, int knobId, int cvId) -> std::function<float()> {
    static auto constexpr attenuation = 0.1F;

    return [module, knobId, cvId]() -> float {
      auto const rotation = module->params[knobId].getValue();
      auto const controlVoltage = module->inputs[cvId].getVoltage();
      auto const modulation = controlVoltage * attenuation;
      return rotation + modulation;
    };
  }

  auto rotation(rack::engine::Module *module, int knobId, int cvId, int avId) -> std::function<float()> {
    static auto constexpr attenuationRange = Range{-0.1F, 0.1F};
    return [module, knobId, cvId, avId]() -> float {
      auto const rotation = module->params[knobId].getValue();
      auto const controlVoltage = module->inputs[cvId].getVoltage();
      auto const attenuverterRotation = module->params[avId].getValue();
      auto const attenuation = attenuationRange.scale(attenuverterRotation);
      auto const modulation = controlVoltage * attenuation;

      return rotation + modulation;
    };
  }

  auto scaled(rack::engine::Module *module, int knobId, Range const &range) -> std::function<float()> {
    return scaled(rotation(module, knobId), range);
  }

  auto scaled(rack::engine::Module *module, int knobId, int cvId, Range const &range) -> std::function<float()> {
    return scaled(rotation(module, knobId, cvId), range);
  }

  auto scaled(rack::engine::Module *module, int knobId, int cvId, int avId, Range const &range)
      -> std::function<float()> {
    return scaled(rotation(module, knobId, cvId, avId), range);
  }

  auto scaled(rack::engine::Module *module, int knobId, int cvId, std::function<Range const *()> const &range)
      -> std::function<float()> {
    return scaled(rotation(module, knobId, cvId), range);
  }

  auto scaled(rack::engine::Module *module, int knobId, int cvId, int avId, std::function<Range const *()> const &range)
      -> std::function<float()> {
    return scaled(rotation(module, knobId, cvId, avId), range);
  }

  auto taperedAndScaled(rack::engine::Module *module, int knobId, taper::FixedTaper const &taper, Range const &range)
      -> std::function<float()> {
    return taperedAndScaled(rotation(module, knobId), taper, range);
  }

  auto taperedAndScaled(rack::engine::Module *module, int knobId, int cvId, int avId, taper::FixedTaper const &taper,
                        Range const &range) -> std::function<float()> {
    return taperedAndScaled(rotation(module, knobId, cvId, avId), taper, range);
  }

  auto taperedAndScaled(rack::engine::Module *module, int knobId, int cvId, taper::FixedTaper const &taper,
                        std::function<Range const *()> const &range) -> std::function<float()> {
    return taperedAndScaled(rotation(module, knobId, cvId), taper, range);
  }

} // namespace knob

namespace toggle {
  template <int N>
  void config(rack::engine::Module *module, int toggleId, std::string const &toggleName,
              std::array<std::string, N> const &stateNames, int initialState) {
    configFrameWidgetStates<N>(module, toggleId, toggleName, stateNames, initialState);
  }

  // Instantiate for arrays with 2, 3, and 4 names
  template void config<2>(rack::engine::Module *module, int toggleId, std::string const &toggleName,
                          std::array<std::string, 2> const &stateNames, int initialState);
  template void config<3>(rack::engine::Module *module, int toggleId, std::string const &toggleName,
                          std::array<std::string, 3> const &stateNames, int initialState);
  template void config<4>(rack::engine::Module *module, int toggleId, std::string const &toggleName,
                          std::array<std::string, 4> const &stateNames, int initialState);
} // namespace toggle

} // namespace dhe
