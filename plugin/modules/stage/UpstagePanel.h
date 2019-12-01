#pragma once

#include "UpstageControls.h"
#include "widgets/Panel.h"

#include <engine/Module.hpp>

namespace dhe {
namespace stage {
  class UpstagePanel : public Panel<UpstagePanel> {
    using Controls = UpstageControls;

  public:
    UpstagePanel(rack::engine::Module *module) {
      setModule(module);
      setPanel(backgroundSvg<UpstagePanel>());
      installScrews(this);

      auto width = hp2mm(hp);

      auto column1 = width / 4.F + 0.333333333F;
      auto column2 = width / 2.F;
      auto column3 = width - column1;

      auto y = 25.F;
      auto dy = 18.5F;

      knob<LargeKnob>(column2, y, Controls::LevelKnob);

      y += dy;
      installInput(this, module, column1, y, Controls::LevelCvInput);
      toggle<2>(column3, y, Controls::LevelRangeSwitch);

      y += dy;
      button(column1, y, Controls::WaitButton);
      button(column3, y, Controls::TriggerButton);

      y = 82.F;
      dy = 15.F;

      installInput(this, module, column1, y, Controls::WaitInput);

      y += dy;
      installInput(this, module, column1, y, Controls::TriggerInput);
      installOutput(this, module, column3, y, Controls::TriggerOutput);

      y += dy;
      installOutput(this, module, column3, y, Controls::EnvelopeOutput);
    }
    static constexpr auto moduleSlug = "upstage";
    static constexpr auto hp = 5;
  };
} // namespace stage

} // namespace dhe
