#include "modules/controls/Curvature.h"
#include "modules/controls/Controls.h"
#include "util/sigmoid.h"

namespace dhe {
namespace curvature {
static float constexpr curveKnobCurvature = 0.65f;

auto rotationToTaper() -> std::function<float(float)> {
  return [](float rotation) -> float { return sigmoid::curvature(rotation); };
}

auto taperToRotation() -> std::function<float(float)> {
  return [](float taper) -> float {
    return sigmoid::s_taper(taper, -curveKnobCurvature);
  };
}

auto selectableTaper(rack::engine::Module *module, int knobId, int cvId,
                     int switchId) -> std::function<float(float)> {
  using namespace control;
  auto const rotation = knob::rotation(module, knobId, cvId);
  auto const shapeSwitch = &module->params[switchId];
  return [rotation, shapeSwitch](float input) -> float {
    auto const curvature = sigmoid::curvature(rotation());
    auto const shapeSelection = static_cast<int>(shapeSwitch->getValue());
    auto const shape = sigmoid::shapes[shapeSelection];

    return shape->taper(input, curvature);
  };
}

auto shape(rack::engine::Module *module, int switchId)
    -> std::function<sigmoid::Shape const *()> {
  auto switchParam = &module->params[switchId];
  return [switchParam]() -> sigmoid::Shape const * {
    auto const selection = static_cast<int>(switchParam->getValue());
    return sigmoid::shapes[selection];
  };
}
} // namespace curvature
} // namespace dhe
