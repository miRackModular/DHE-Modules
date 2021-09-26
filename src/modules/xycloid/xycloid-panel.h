#pragma once

#include "widgets/control-widgets.h"
#include "widgets/screws.h"
#include "xycloid-controls.h"

#include <app/ModuleWidget.hpp>

namespace dhe {

namespace xycloid {

class XycloidPanel : public rack::app::ModuleWidget {
  using Controls = XycloidControls;
  using Jack = Jack<XycloidPanel>;
  using Knob = Knob<XycloidPanel>;

public:
  static auto constexpr svg_dir = "xycloid";

  explicit XycloidPanel(rack::engine::Module *module) {
    auto constexpr hp = 11;

    setModule(module);
    setPanel(load_svg(svg_dir, "xycloid"));
    install_screws(this, hp);

    auto constexpr width = hp2mm(hp);

    auto constexpr column1 = width / 7.F;
    auto constexpr column4 = width - column1;
    auto constexpr column2 = (column4 - column1) / 3.F + column1;
    auto constexpr column3 = width - column2;

    auto y = 25.F;
    auto dy = 18.5F;

    addInput(Jack::input(module, column1, y, Controls::SpeedCvInput));
    addParam(Knob::tiny(module, column2, y, Controls::SpeedAvKnob));
    addParam(Knob::large(module, column3, y, Controls::SpeedKnob));

    y += dy;
    addInput(Jack::input(module, column1, y, Controls::RatioCvInput));
    addParam(Knob::tiny(module, column2, y, Controls::RatioAvKnob));
    addParam(Knob::large(module, column3, y, Controls::RatioKnob));
    addParam(Toggle::thumb(2, svg_dir, module, column4, y,
                           Controls::FreeRatioSwitch));

    y += dy;
    addInput(Jack::input(module, column1, y, Controls::DepthCvInput));
    addParam(Knob::tiny(module, column2, y, Controls::DepthAvKnob));
    addParam(Knob::large(module, column3, y, Controls::DepthKnob));
    addParam(Toggle::thumb(3, svg_dir, module, column4, y,
                           Controls::DirectionSwitch));

    y += dy;
    addInput(Jack::input(module, column1, y, Controls::PhaseCvInput));
    addParam(Knob::tiny(module, column2, y, Controls::PhaseOffsetAvKnob));
    addParam(Knob::large(module, column3, y, Controls::PhaseOffsetKnob));

    y = 82.F;
    dy = 15.F;
    auto constexpr output_port_offset = 1.25F;

    y += dy;
    addInput(Jack::input(module, column1, y, Controls::XGainCvInput));
    addParam(Knob::small(module, column2, y, Controls::XGainKnob));
    addParam(
        Toggle::thumb(2, svg_dir, module, column3, y, Controls::XRangeSwitch));
    addOutput(Jack::output(module, column4, y + output_port_offset,
                           Controls::XOutput));

    y += dy;
    addInput(Jack::input(module, column1, y, Controls::YGainCvInput));
    addParam(Knob::small(module, column2, y, Controls::YGainKnob));
    addParam(
        Toggle::thumb(2, svg_dir, module, column3, y, Controls::YRangeSwitch));
    addOutput(Jack::output(module, column4, y + output_port_offset,
                           Controls::YOutput));
  }
};
} // namespace xycloid
} // namespace dhe
