#include "FuzzyLogicHPanel.h"

#include "FuzzyLogicH.h"
#include "widgets/ControlWidgets.h"
#include "widgets/Screws.h"

namespace dhe {

FuzzyLogicHPanel::FuzzyLogicHPanel(rack::engine::Module *module) {
  auto constexpr hp = 9;

  setModule(module);
  setPanel(backgroundSvg(moduleSlug));
  installScrews(this, hp);

  auto const abOuterColumn = hp2mm(1.5F);
  auto const abButtonColumn = hp2mm(3.F);
  auto const abInnerColumn = hp2mm(3.25F);
  auto const switchColumn = hp2mm(4.5F);
  auto const cdInnerColumn = hp2mm(5.75F);
  auto const cdButtonColumn = hp2mm(6.F);
  auto const cdOuterColumn = hp2mm(7.5F);

  auto const top = hp2mm(4.F);
  auto const dy = hp2mm(3.F);

  auto y = top + 0.F * dy;
  installInput(this, module, abOuterColumn, y, FuzzyLogicH::AInputs + 0);
  install<ToggleButton>(this, module, abButtonColumn, y, FuzzyLogicH::NotAButtons + 0);
  install<ToggleButton>(this, module, cdButtonColumn, y, FuzzyLogicH::NotAButtons + 1);
  installInput(this, module, cdOuterColumn, y, FuzzyLogicH::AInputs + 1);

  y = top + 0.5F * dy;
  install<Toggle<2>>(this, module, switchColumn, y, FuzzyLogicH::LevelRangeSwitch);

  y = top + 1.F * dy;
  installInput(this, module, abOuterColumn, y, FuzzyLogicH::BInputs + 0);
  install<ToggleButton>(this, module, abButtonColumn, y, FuzzyLogicH::NotBButtons + 0);
  install<ToggleButton>(this, module, cdButtonColumn, y, FuzzyLogicH::NotBButtons + 1);
  installInput(this, module, cdOuterColumn, y, FuzzyLogicH::BInputs + 1);

  y = top + 2.F * dy;
  installOutput(this, module, abOuterColumn, y, FuzzyLogicH::AndOutputs + 0);
  installOutput(this, module, abInnerColumn, y, FuzzyLogicH::NandOutputs + 0);
  installOutput(this, module, cdInnerColumn, y, FuzzyLogicH::NandOutputs + 1);
  installOutput(this, module, cdOuterColumn, y, FuzzyLogicH::AndOutputs + 1);

  y = top + 3.F * dy;
  installOutput(this, module, abOuterColumn, y, FuzzyLogicH::OrOutputs + 0);
  installOutput(this, module, abInnerColumn, y, FuzzyLogicH::NorOutputs + 0);
  installOutput(this, module, cdInnerColumn, y, FuzzyLogicH::NorOutputs + 1);
  installOutput(this, module, cdOuterColumn, y, FuzzyLogicH::OrOutputs + 1);

  y = top + 4.F * dy;
  installOutput(this, module, abOuterColumn, y, FuzzyLogicH::XorOutputs + 0);
  installOutput(this, module, abInnerColumn, y, FuzzyLogicH::XnorOutputs + 0);
  installOutput(this, module, cdInnerColumn, y, FuzzyLogicH::XnorOutputs + 1);
  installOutput(this, module, cdOuterColumn, y, FuzzyLogicH::XorOutputs + 1);

  y = top + 5.F * dy;
  installOutput(this, module, abOuterColumn, y, FuzzyLogicH::ImplicationOutputs + 0);
  installOutput(this, module, abInnerColumn, y, FuzzyLogicH::NonimplicationOutputs + 0);
  installOutput(this, module, cdInnerColumn, y, FuzzyLogicH::NonimplicationOutputs + 1);
  installOutput(this, module, cdOuterColumn, y, FuzzyLogicH::ImplicationOutputs + 1);

  y = top + 6.F * dy;
  installOutput(this, module, abOuterColumn, y, FuzzyLogicH::ConverseImplicationOutputs + 0);
  installOutput(this, module, abInnerColumn, y, FuzzyLogicH::ConverseNonimplicationOutputs + 0);
  installOutput(this, module, cdInnerColumn, y, FuzzyLogicH::ConverseNonimplicationOutputs + 1);
  installOutput(this, module, cdOuterColumn, y, FuzzyLogicH::ConverseImplicationOutputs + 1);
}
} // namespace dhe
