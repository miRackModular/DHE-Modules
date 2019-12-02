#pragma once

#include "CurveSequencerControls.h"
#include "widgets/ControlWidgets.h"
#include "widgets/Screws.h"

#include <app/ModuleWidget.hpp>
#include <componentlibrary.hpp>
#include <functional>
#include <string>
#include <utility>

namespace dhe {

namespace curve_sequencer {
  auto constexpr stepX = hp2mm(10.F);
  auto constexpr stepDx = hp2mm(2.25F);

  using ProgressLight = rack::componentlibrary::SmallLight<rack::componentlibrary::GreenRedLight>;

  class GenerateModeStepper : public Toggle {
  public:
    GenerateModeStepper(std::string const &moduleSlug, rack::engine::Module *module, float x, float y, int index) :
        Toggle("stepper-mode", modeCount, moduleSlug, module, x, y, index) {}
  };

  class SustainModeStepper : public Toggle {
  public:
    SustainModeStepper(std::string const &moduleSlug, rack::engine::Module *module, float x, float y, int index) :
        Toggle("stepper-advance", advanceConditionCount, moduleSlug, module, x, y, index) {}
  };

  class SelectionKnob : public Knob {
  public:
    SelectionKnob(std::function<void(int)> action, std::string const &moduleSlug, rack::engine::Module *module, float x,
                  float y, int index) :
        Knob{moduleSlug, "knob-small", module, x, y, index}, knobChangedTo{std::move(action)} {
      snap = true;
    }

    void onChange(const rack::event::Change &e) override {
      Knob::onChange(e);
      knobChangedTo(static_cast<int>(this->paramQuantity->getValue()));
    }

  private:
    std::function<void(int)> knobChangedTo;
  };

  class StartMarker : public rack::widget::SvgWidget {
  public:
    StartMarker(std::string const &moduleSlug, float x, float y) {
      setSvg(controlSvg(moduleSlug, "marker-start"));
      positionCentered(this, x, y);
    }

    void setSelectionStart(int step) {
      auto constexpr baseX = stepX - 2.F * lightDiameter;
      auto const x = baseX + stepDx * static_cast<float>(step);
      this->box.pos.x = mm2px(x);
    }
  };

  template <int N> class EndMarker : public rack::widget::SvgWidget {
  public:
    EndMarker(std::string const &moduleSlug, float x, float y) {
      setSvg(controlSvg(moduleSlug, "marker-end"));
      positionCentered(this, x, y);
    }

    void setSelectionStart(int step) {
      this->selectionStart = step;
      move();
    }
    void setSelectionLength(int length) {
      this->selectionLength = length;
      move();
    }

  private:
    void move() {
      auto const selectionEnd = (selectionStart + selectionLength - 1) & stepMask;
      auto const x = stepX + stepDx * static_cast<float>(selectionEnd);
      this->box.pos.x = mm2px(x);
    }

    int selectionStart{};
    int selectionLength{};
    int const stepMask = N - 1;
  }; // namespace curve_sequencer

  template <int N> class CurveSequencerPanel : public rack::app::ModuleWidget {
    using Controls = CurveSequencerControls<N>;

  public:
    static std::string const moduleSlug;

    CurveSequencerPanel(rack::engine::Module *module) {
      static auto constexpr stepWidth = 2.25F;
      static auto constexpr sequenceControlsWidth = 13.F;
      static auto constexpr hp = static_cast<int>(sequenceControlsWidth + N * stepWidth);

      this->setModule(module);
      this->setPanel(backgroundSvg(moduleSlug));
      installScrews(this, hp);

      auto constexpr left = hp2mm(2.F);
      auto constexpr right = hp2mm(hp - 2.F);
      auto constexpr top = hp2mm(4.F);
      auto constexpr bottom = hp2mm(23);

      auto constexpr inputTop = top + hp2mm(2.75);
      auto constexpr inputBottom = bottom - portRadius - 1.F;
      auto constexpr inputDy = (inputBottom - inputTop) / 4.F;

      auto constexpr runY = inputTop + 0.F * inputDy;
      auto constexpr loopY = inputTop + 1.F * inputDy;
      auto constexpr selectionY = inputTop + 2.F * inputDy;
      auto constexpr gateY = inputTop + 3.F * inputDy;
      auto constexpr resetY = inputTop + 4.F * inputDy;

      auto constexpr activeY = top + lightRadius;

      installInput(this, module, left, runY, Controls::RunInput);
      addParam(Button::toggle(moduleSlug, module, left + buttonPortDistance, runY, Controls::RunButton));

      installInput(this, module, left, loopY, Controls::LoopInput);
      addParam(Button::toggle(moduleSlug, module, left + buttonPortDistance, loopY, Controls::LoopButton));

      auto *startMarker = new StartMarker(moduleSlug, 0.F, activeY);
      this->addChild(startMarker);

      auto *endMarker = new EndMarker<N>(moduleSlug, 0.F, activeY);
      this->addChild(endMarker);

      auto onSelectionStartChange = [startMarker, endMarker](int step) {
        startMarker->setSelectionStart(step);
        endMarker->setSelectionStart(step);
      };
      addParam(new SelectionKnob(onSelectionStartChange, moduleSlug, module, left, selectionY,
                                 Controls::SelectionStartKnob));

      auto onSelectionEndChange = [endMarker](int length) { endMarker->setSelectionLength(length); };
      auto constexpr selectionLengthX = left + hp2mm(2.F);
      addParam(new SelectionKnob(onSelectionEndChange, moduleSlug, module, selectionLengthX, selectionY,
                                 Controls::SelectionLengthKnob));

      installInput(this, module, left, gateY, Controls::GateInput);
      addParam(Button::momentary(moduleSlug, module, left + buttonPortDistance, gateY, Controls::GateButton));

      installInput(this, module, left, resetY, Controls::ResetInput);
      addParam(Button::momentary(moduleSlug, module, left + buttonPortDistance, resetY, Controls::ResetButton));

      auto constexpr generatingModeY = top + hp2mm(2.25F);
      auto constexpr sustainingModeY = top + hp2mm(4.5F);
      auto constexpr levelY = top + hp2mm(6.75F);
      auto constexpr shapeY = top + hp2mm(9.25F);
      auto constexpr curveY = top + hp2mm(11.75F);
      auto constexpr durationY = top + hp2mm(14.25F);
      auto constexpr enabledPortY = bottom - portRadius;
      auto constexpr enabledButtonY = enabledPortY - portRadius - buttonRadius - 1.F;

      for (int step = 0; step < N; step++) {
        auto const x = stepX + stepDx * (float) step;
        installLight<ProgressLight>(this, module, x, activeY, Controls::ProgressLights + step + step);

        addParam(new GenerateModeStepper{moduleSlug, module, x, generatingModeY, Controls::ModeSwitches + step});
        addParam(new SustainModeStepper{moduleSlug, module, x, sustainingModeY, Controls::ConditionSwitches + step});

        addParam(Knob::small(moduleSlug, module, x, levelY, Controls::LevelKnobs + step));

        addParam(Toggle::stepper(2, moduleSlug, module, x, shapeY, Controls::ShapeSwitches + step));
        addParam(Knob::small(moduleSlug, module, x, curveY, Controls::CurveKnobs + step));

        addParam(Knob::small(moduleSlug, module, x, durationY, Controls::DurationKnobs + step));

        addParam(Button::toggle(moduleSlug, module, x, enabledButtonY, Controls::EnabledButtons + step));
        installInput(this, module, x, enabledPortY, Controls::EnabledInputs + step);
      }

      auto constexpr outY = bottom - portRadius - 1.F;
      auto constexpr eosY = top + hp2mm(2.75);

      installInput(this, module, right, eosY, Controls::CurveSequencerInput);

      addParam(Toggle::stepper(2, moduleSlug, module, right, levelY, Controls::LevelRangeSwitch));
      addParam(Toggle::stepper(3, moduleSlug, module, right, durationY, Controls::DurationRangeSwitch));
      installOutput(this, module, right, outY, Controls::CurveSequencerOutput);
    }
  }; // namespace dhe

  template <int N> const std::string CurveSequencerPanel<N>::moduleSlug = "curve-sequencer-" + std::to_string(N);
} // namespace curve_sequencer
} // namespace dhe
