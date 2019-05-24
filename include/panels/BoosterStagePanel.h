#pragma once

#include "Panel.h"
#include "modules/BoosterStage.h"

namespace dhe {

class BoosterStagePanel : public Panel<BoosterStagePanel> {
public:
  explicit BoosterStagePanel(BoosterStage *module);
  static constexpr auto moduleSlug = "booster-stage";
  static constexpr auto hp = 8;
};
} // namespace dhe
