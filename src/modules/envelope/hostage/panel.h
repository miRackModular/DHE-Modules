#pragma once

#include "control-ids.h"

#include "controls/knobs.h"
#include "controls/ports.h"
#include "controls/switches.h"
#include "panels/panel-widget.h"

#include "rack.hpp"

namespace dhe {
namespace envelope {
namespace hostage {
struct Panel : public PanelWidget<Panel> {

public:
  static auto constexpr hp = 5;
  static auto constexpr panel_file = "hostage";
  static auto constexpr svg_dir = "hostage";

  Panel(rack::engine::Module *module) : PanelWidget<Panel>{module} {
    auto constexpr width = hp2mm(hp);

    auto constexpr column1 = width / 4.F + 0.333333F;
    auto constexpr column2 = width / 2.F;
    auto constexpr column3 = width - column1;

    auto y = 25.F;
    auto dy = 18.5F;

    ThumbSwitch<2>::install(this, Param::ModeSwitch, column2, y);

    y += dy;
    InPort::install(this, Input::DurationCvInput, column1, y);
    ThumbSwitch<3>::install(this, Param::DurationRangeSwitch, column3, y);

    y += dy;
    Knob::install<Large>(this, Param::DurationKnob, column2, y);

    y = 82.F;
    dy = 15.F;

    InPort::install(this, Input::DeferInput, column1, y);
    OutPort::install(this, Output::ActiveOutput, column3, y);

    y += dy;
    InPort::install(this, Input::TriggerInput, column1, y);
    OutPort::install(this, Output::EocOutput, column3, y);

    y += dy;
    InPort::install(this, Input::EnvelopeInput, column1, y);
    OutPort::install(this, Output::EnvelopeOutput, column3, y);
  }
};
} // namespace hostage
} // namespace envelope
} // namespace dhe
