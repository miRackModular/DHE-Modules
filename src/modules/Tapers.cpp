#include "modules/Tapers.h"

#include "modules/controls/Controls.h"
#include "modules/controls/CurvatureControls.h"
#include "modules/controls/Level.h"
#include "modules/controls/TaperControls.h"

namespace dhe {

Tapers::Tapers() {
  config(ParameterCount, InputCount, OutputCount);

  level::configKnob(this, LevelKnob1, LevelRangeSwitch1, "Level 1");
  level::configSwitch(this, LevelRangeSwitch1, "Level 1 range", 0);
  configAttenuverter(this, LevelAvKnob1, "Level 1 CV gain");

  configCurvatureKnob(this, CurveKnob1, "Curvature 1");
  configAttenuverter(this, CurveAvKnob1, "Curvature 1 CV gain");
  configShapeSwitch(this, ShapeSwitch1, "Shape 1");

  level::configKnob(this, LevelKnob2, LevelRangeSwitch2, "Level 2");
  level::configSwitch(this, LevelRangeSwitch2, "Level 2 range", 0);
  configAttenuverter(this, LevelAvKnob2, "Level 2 CV gain");

  configCurvatureKnob(this, CurveKnob2, "Curvature 2");
  configAttenuverter(this, CurveAv2, "Curvature 2 CV gain");
  configShapeSwitch(this, ShapeSwitch2, "Shape 2");

  levelRotation1 = rotationFunction(this, LevelKnob1, Level1Cv, LevelAvKnob1);
  levelRange1 = rangeSelectorFunction<2>(this, LevelRangeSwitch1, level::ranges);
  taper1 = selectableShapeTaperFunction(this, CurveKnob1, Curve1Cv, CurveAvKnob1, ShapeSwitch1);

  levelRotation2 = rotationFunction(this, LevelKnob2, Level2Cv, LevelAvKnob2);
  levelRange2 = rangeSelectorFunction<2>(this, LevelRangeSwitch2, level::ranges);
  taper2 = selectableShapeTaperFunction(this, CurveKnob2, CurveCv2, CurveAv2, ShapeSwitch2);
}

void Tapers::process(const ProcessArgs & /*args*/) {
  outputs[TaperOutput1].setVoltage(levelRange1()->scale(taper1(levelRotation1())));
  outputs[Taper2Output].setVoltage(levelRange2()->scale(taper2(levelRotation2())));
}
} // namespace dhe
