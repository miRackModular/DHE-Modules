#include <algorithm>

#include "rack.hpp"

#include "plugin/Plugin.hpp"

#include "panels/BlossomPanel.h"
#include "panels/BoosterStagePanel.h"
#include "panels/CubicPanel.h"
#include "panels/Func6Panel.h"
#include "panels/FuncPanel.h"
#include "panels/HostagePanel.h"
#include "panels/RangerPanel.h"
#include "panels/StagePanel.h"
#include "panels/SwavePanel.h"
#include "panels/TapersPanel.h"
#include "panels/UpstagePanel.h"
#include "panels/XycloidPanel.h"

rack::plugin::Plugin *pluginInstance;

void init(rack::plugin::Plugin *p) {
  pluginInstance = p;

  p->addModel(rack::createModel<dhe::Blossom, dhe::BlossomPanel>("Blossom"));
  p->addModel(rack::createModel<dhe::BoosterStage, dhe::BoosterStagePanel>(
      "BoosterStage"));
  p->addModel(rack::createModel<dhe::Cubic, dhe::CubicPanel>("Cubic"));
  p->addModel(rack::createModel<dhe::Func, dhe::FuncPanel>("Func"));
  p->addModel(rack::createModel<dhe::Func6, dhe::Func6Panel>("Func 6"));
  p->addModel(rack::createModel<dhe::Hostage, dhe::HostagePanel>("Hostage"));
  p->addModel(rack::createModel<dhe::Stage, dhe::StagePanel>("Stage"));
  p->addModel(rack::createModel<dhe::Ranger, dhe::RangerPanel>("Ranger"));
  p->addModel(rack::createModel<dhe::Swave, dhe::SwavePanel>("Swave"));
  p->addModel(rack::createModel<dhe::Tapers, dhe::TapersPanel>("Tapers"));
  p->addModel(rack::createModel<dhe::Upstage, dhe::UpstagePanel>("Upstage"));
  p->addModel(rack::createModel<dhe::Xycloid, dhe::XycloidPanel>("Xycloid"));
}
