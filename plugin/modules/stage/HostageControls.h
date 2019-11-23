#pragma once

#include "controls/DurationInputs.h"

#include <engine/Param.hpp>
#include <engine/Port.hpp>

namespace dhe {
namespace stage {

  using rack::engine::Input;
  using rack::engine::Output;
  using rack::engine::Param;

  class HostageControls {
  public:
    HostageControls(std::vector<Input> const &inputs, std::vector<Param> const &params, std::vector<Output> &outputs) :
        inputs{inputs}, params{params}, outputs{outputs} {};

    auto duration() const -> float {
      return selectableDuration(params[DurationKnob], inputs[DurationCvInput], params[DurationRangeSwitch]);
    }

    auto envelopeIn() const -> float { return voltageAt(inputs[EnvelopeInput]); }

    auto isDeferring() const -> bool { return isHigh(inputs[DeferInput]); }

    auto isGated() const -> bool { return isHigh(inputs[TriggerInput]); }

    auto isSustainMode() const -> bool { return positionOf(params[ModeSwitch]) == 1; }

    void sendActive(bool active) {
      auto const voltage = unipolarSignalRange.scale(active);
      outputs[ActiveOutput].setVoltage(voltage);
    }

    void sendEoc(bool eoc) {
      auto const voltage = unipolarSignalRange.scale(eoc);
      outputs[EocOutput].setVoltage(voltage);
    }

    void sendOut(float voltage) { outputs[EnvelopeOutput].setVoltage(voltage); }

    enum ParameterIds { DurationKnob, DurationRangeSwitch, ModeSwitch, ParameterCount };

    enum InputIds { DeferInput, DurationCvInput, EnvelopeInput, TriggerInput, InputCount };

    enum OutputIds { ActiveOutput, EnvelopeOutput, EocOutput, OutputCount };

  private:
    std::vector<Input> const &inputs;
    std::vector<Param> const &params;
    std::vector<Output> &outputs;
  };
} // namespace stage
} // namespace dhe
