#pragma once

#include "Panel.h"
#include "modules/Stage.h"

namespace dhe {
class StagePanel : public Panel<StagePanel> {
public:
  explicit StagePanel(Stage *module);
  static constexpr auto module_slug = "stage";
  static constexpr auto hp = 5;
};
} // namespace dhe
