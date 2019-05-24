#pragma once

#include "Panel.h"
#include "modules/Hostage.h"

namespace dhe {

class HostagePanel : public Panel<HostagePanel> {
public:
  explicit HostagePanel(Hostage *module);
  static constexpr auto moduleSlug = "hostage";
  static constexpr auto hp = 5;
};
} // namespace dhe
