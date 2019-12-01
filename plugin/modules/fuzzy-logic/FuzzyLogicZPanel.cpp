#include "FuzzyLogicZPanel.h"

#include "FuzzyLogicZ.h"
#include "widgets/Jacks.h"
#include "widgets/Knobs.h"
#include "widgets/Screws.h"

namespace dhe {

FuzzyLogicZPanel::FuzzyLogicZPanel(rack::engine::Module *module) {
  setModule(module);
  setPanel(backgroundSvg<FuzzyLogicZPanel>());
  installScrews(this);

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
  installInput(this, module, abOuterColumn, y, FuzzyLogicZ::AInputs + 0);
  button<ToggleButton>(abButtonColumn, y, FuzzyLogicZ::NotAButtons + 0);
  button<ToggleButton>(cdButtonColumn, y, FuzzyLogicZ::NotAButtons + 1);
  installInput(this, module, cdOuterColumn, y, FuzzyLogicZ::AInputs + 1);

  y = top + 0.5F * dy;
  toggle<2>(switchColumn, y, FuzzyLogicZ::LevelRangeSwitch);

  y = top + 1.F * dy;
  installInput(this, module, abOuterColumn, y, FuzzyLogicZ::BInputs + 0);
  button<ToggleButton>(abButtonColumn, y, FuzzyLogicZ::NotBButtons + 0);
  button<ToggleButton>(cdButtonColumn, y, FuzzyLogicZ::NotBButtons + 1);
  installInput(this, module, cdOuterColumn, y, FuzzyLogicZ::BInputs + 1);

  y = top + 2.F * dy;
  installOutput(this, module, abOuterColumn, y, FuzzyLogicZ::AndOutputs + 0);
  installOutput(this, module, abInnerColumn, y, FuzzyLogicZ::NandOutputs + 0);
  installOutput(this, module, cdInnerColumn, y, FuzzyLogicZ::NandOutputs + 1);
  installOutput(this, module, cdOuterColumn, y, FuzzyLogicZ::AndOutputs + 1);

  y = top + 3.F * dy;
  installOutput(this, module, abOuterColumn, y, FuzzyLogicZ::OrOutputs + 0);
  installOutput(this, module, abInnerColumn, y, FuzzyLogicZ::NorOutputs + 0);
  installOutput(this, module, cdInnerColumn, y, FuzzyLogicZ::NorOutputs + 1);
  installOutput(this, module, cdOuterColumn, y, FuzzyLogicZ::OrOutputs + 1);

  y = top + 4.F * dy;
  installOutput(this, module, abOuterColumn, y, FuzzyLogicZ::XorOutputs + 0);
  installOutput(this, module, abInnerColumn, y, FuzzyLogicZ::XnorOutputs + 0);
  installOutput(this, module, cdInnerColumn, y, FuzzyLogicZ::XnorOutputs + 1);
  installOutput(this, module, cdOuterColumn, y, FuzzyLogicZ::XorOutputs + 1);

  y = top + 5.F * dy;
  installOutput(this, module, abOuterColumn, y, FuzzyLogicZ::ImplicationOutputs + 0);
  installOutput(this, module, abInnerColumn, y, FuzzyLogicZ::NonimplicationOutputs + 0);
  installOutput(this, module, cdInnerColumn, y, FuzzyLogicZ::NonimplicationOutputs + 1);
  installOutput(this, module, cdOuterColumn, y, FuzzyLogicZ::ImplicationOutputs + 1);

  y = top + 6.F * dy;
  installOutput(this, module, abOuterColumn, y, FuzzyLogicZ::ConverseImplicationOutputs + 0);
  installOutput(this, module, abInnerColumn, y, FuzzyLogicZ::ConverseNonimplicationOutputs + 0);
  installOutput(this, module, cdInnerColumn, y, FuzzyLogicZ::ConverseNonimplicationOutputs + 1);
  installOutput(this, module, cdOuterColumn, y, FuzzyLogicZ::ConverseImplicationOutputs + 1);
}
} // namespace dhe
