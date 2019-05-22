#pragma once

#include <memory>

#include "Module.h"
#include "modules/controls/CurvatureControl.h"
#include "modules/controls/LevelControl.h"

namespace dhe {

class Tapers : public Module {
public:
  Tapers();

  void process(const ProcessArgs &args) override;

  enum ParameterIds {
    LEVEL_1_KNOB,
    LEVEL_1_AV,
    RANGE_1_SWITCH,
    CURVE_1_KNOB,
    CURVE_1_AV,
    SHAPE_1_SWITCH,
    LEVEL_2_KNOB,
    LEVEL_2_AV,
    RANGE_2_SWITCH,
    CURVE_2_KNOB,
    CURVE_2_AV,
    SHAPE_2_SWITCH,
    PARAMETER_COUNT
  };
  enum InputIds { LEVEL_1_CV, CURVE_1_CV, LEVEL_2_CV, CURVE_2_CV, INPUT_COUNT };
  enum OutputIds { OUT_1, OUT_2, OUTPUT_COUNT };

private:
  std::unique_ptr<LevelControl> level1;
  std::unique_ptr<CurvatureControl> curvature1;
  std::unique_ptr<LevelControl> level2;
  std::unique_ptr<CurvatureControl> curvature2;
};

} // namespace dhe
