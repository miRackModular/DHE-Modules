#pragma once

#include "control-ids.h"
#include "params.h"

#include "controls/knobs.h"
#include "controls/ports.h"
#include "panels/panel-widget.h"

#include "rack.hpp"

#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace dhe {

namespace curve_sequencer {
auto constexpr step_x = hp2mm(10.F);
auto constexpr step_dx = hp2mm(2.25F);

using ProgressLight =
    rack::componentlibrary::SmallLight<rack::componentlibrary::GreenRedLight>;

template <typename PanelT>
class SelectionKnob : public Knob::Widget<PanelT, Small> {
public:
  static inline auto create(rack::engine::Module *module, float xmm, float ymm,
                            int index, std::function<void(int)> const &action)
      -> SelectionKnob * {
    auto knob = rack::createParamCentered<SelectionKnob>(mm2px(xmm, ymm),
                                                         module, index);
    knob->knob_changed_to_ = action;
    return knob;
  }

  void onChange(const rack::event::Change &e) override {
    Knob::Widget<PanelT, Small>::onChange(e);
    knob_changed_to_(static_cast<int>(this->getParamQuantity()->getValue()));
  }

private:
  std::function<void(int)> knob_changed_to_;
};

class StartMarker : public rack::widget::SvgWidget {
public:
  StartMarker(std::string const &module_svg_dir, float x, float y) {
    setSvg(load_svg(module_svg_dir, "marker-start"));
    position_centered(this, x, y);
  }

  void set_selection_start(int step) {
    auto constexpr base_x = step_x - 2.F * light_diameter;
    auto const x = base_x + step_dx * static_cast<float>(step);
    this->box.pos.x = mm2px(x);
  }
};

template <int N> class EndMarker : public rack::widget::SvgWidget {
public:
  EndMarker(std::string const &module_svg_dir, float x, float y) {
    setSvg(load_svg(module_svg_dir, "marker-end"));
    position_centered(this, x, y);
  }

  void set_selection_start(int step) {
    this->selection_start_ = step;
    move();
  }
  void set_selection_length(int length) {
    this->selection_length_ = length;
    move();
  }

private:
  void move() {
    auto const selection_end =
        (selection_start_ + selection_length_ - 1) & step_mask_;
    auto const x = step_x + step_dx * static_cast<float>(selection_end);
    this->box.pos.x = mm2px(x);
  }

  int selection_start_{};
  int selection_length_{};
  int const step_mask_ = N - 1;
};

template <typename TSize> struct Panel : public PanelWidget<Panel<TSize>> {
  static auto constexpr N = TSize::step_count;
  static auto constexpr panel_file = TSize::panel_file;
  static auto constexpr svg_dir = "curve-sequencer";
  static auto constexpr step_width = 2.25F;
  static auto constexpr sequence_controls_width = 13.F;
  static auto constexpr hp =
      static_cast<int>(sequence_controls_width + N * step_width);

  Panel(rack::engine::Module *module) : PanelWidget<Panel<TSize>>{module} {
    auto constexpr left = hp2mm(2.F);
    auto constexpr right = hp2mm(hp - 2.F);
    auto constexpr top = hp2mm(4.F);
    auto constexpr bottom = hp2mm(23);

    auto constexpr input_top = top + hp2mm(2.75);
    auto constexpr input_bottom = bottom - port_radius - 1.F;
    auto constexpr input_dy = (input_bottom - input_top) / 4.F;

    auto constexpr run_y = input_top + 0.F * input_dy;
    auto constexpr loop_y = input_top + 1.F * input_dy;
    auto constexpr selection_y = input_top + 2.F * input_dy;
    auto constexpr gate_y = input_top + 3.F * input_dy;
    auto constexpr reset_y = input_top + 4.F * input_dy;

    auto constexpr active_y = top + light_radius;

    InPort::install(this, Input::Run, left, run_y);
    Button::install<Toggle>(this, Param::Run, left + button_port_distance,
                            run_y);

    InPort::install(this, Input::Loop, left, loop_y);
    Button::install<Toggle>(this, Param::Loop, left + button_port_distance,
                            loop_y);

    auto *start_marker = new StartMarker(svg_dir, 0.F, active_y);
    this->addChild(start_marker);

    auto *end_marker = new EndMarker<N>(svg_dir, 0.F, active_y);
    this->addChild(end_marker);

    auto const on_selection_start_change = [start_marker,
                                            end_marker](int step) {
      start_marker->set_selection_start(step);
      end_marker->set_selection_start(step);
    };
    this->addParam(SelectionKnob<Panel<TSize>>::create(
        module, left, selection_y, Param::SelectionStart,
        on_selection_start_change));

    auto const on_selection_end_change = [end_marker](int length) {
      end_marker->set_selection_length(length);
    };
    auto constexpr selection_length_x = left + hp2mm(2.F);
    this->addParam(SelectionKnob<Panel<TSize>>::create(
        module, selection_length_x, selection_y, Param::SelectionLength,
        on_selection_end_change));

    InPort::install(this, Input::Gate, left, gate_y);
    Button::install<Momentary>(this, Param::Gate, left + button_port_distance,
                               gate_y);

    InPort::install(this, Input::Reset, left, reset_y);
    Button::install<Momentary>(this, Param::Reset, left + button_port_distance,
                               reset_y);

    auto constexpr generate_mode_y = top + hp2mm(1.61F);
    auto constexpr advance_mode_y = top + hp2mm(3.25F);
    auto constexpr level_y = top + hp2mm(5.75F);
    auto constexpr shape_y = top + hp2mm(8.75F);
    auto constexpr curve_y = top + hp2mm(11.0F);
    auto constexpr duration_y = top + hp2mm(14.0F);
    auto constexpr enabled_port_y = bottom - port_radius;
    auto constexpr enabled_button_y =
        enabled_port_y - port_radius - button_radius - 1.F;

    auto const generate_mode_labels = std::vector<std::string>{
        "curve", "hold", "sustain", "input", "chase", "level"};
    auto const advance_mode_labels =
        std::vector<std::string>{"time", "rise", "fall", "edge", "high", "low"};

    for (auto step = 0; step < N; step++) {
      auto const x = step_x + step_dx * (float)step;
      this->addChild(rack::createLightCentered<ProgressLight>(
          mm2px(x, active_y), module, Light::StepProgress + step + step));

      Stepper<GenerateModes>::install(this, Param::StepGenerateMode + step, x,
                                      generate_mode_y);
      Stepper<AdvanceModes>::install(this, Param::StepAdvanceMode + step, x,
                                     advance_mode_y);

      Knob::install<Small>(this, Param::StepLevel + step, x, level_y);

      ThumbSwitch<2>::install(this, Param::StepShape + step, x, shape_y);
      Knob::install<Small>(this, Param::StepCurvature + step, x, curve_y);

      Knob::install<Small>(this, Param::StepDuration + step, x, duration_y);

      Button::install<Toggle>(this, Param::StepEnabled + step, x,
                              enabled_button_y);
      InPort::install(this, Input::StepEnabled + step, x, enabled_port_y);
    }

    auto constexpr out_y = bottom - port_radius - 1.F;
    auto constexpr eos_y = top + hp2mm(2.75);

    InPort::install(this, Input::Main, right, eos_y);

    ThumbSwitch<2>::install(this, Param::LevelRange, right, level_y);
    ThumbSwitch<3>::install(this, Param::DurationRange, right, duration_y);
    OutPort::install(this, Output::Main, right, out_y);
  }

private:
  using Param = ParamIds<N>;
  using Input = InputIds<N>;
  using Light = LightIds<N>;
  using Output = OutputIds;
}; // namespace dhe
} // namespace curve_sequencer
} // namespace dhe