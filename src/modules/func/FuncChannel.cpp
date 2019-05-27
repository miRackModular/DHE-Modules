#include "modules/func/FuncChannel.h"

#include "modules/controls/Controls.h"
#include "modules/controls/Level.h"

namespace dhe {

const std::array<Range const *, 4> multiplierRanges{&attenuator::range, &attenuverter::range, &gain::range,
                                                    &gain::invertibleRange};

const std::array<Range const *, 4> offsetRanges{&level::rectifiedBipolarRange, &level::bipolarRange,
                                                &level::unipolarRange, &level::invertibleUnipolarRange};

class FuncOperandKnobParamQuantity : public rack::engine::ParamQuantity {
public:
  auto getLabel() -> std::string override { return channel->currentOperandName(); }

  auto getDisplayValue() -> float override {
    auto const rotation = getValue();
    auto const operandRange = channel->currentOperandRange();
    auto const operand = operandRange->scale(rotation);
    return operand;
  }

  void setDisplayValue(float operand) override {
    auto const operandRange = channel->currentOperandRange();
    auto const rotation = operandRange->normalize(operand);
    setValue(rotation);
  }

  void setChannel(FuncChannel *newChannel) { channel = newChannel; }

private:
  FuncChannel *channel;
};

FuncChannel::FuncChannel(rack::engine::Module *module, int inputIndex, int operandIndex, int outputIndex,
                         int operationSwitchIndex, int offsetRangeSwitchIndex, int multiplierRangeSwitchIndex,
                         std::string const &channelName) :
    input{&module->inputs[inputIndex]},
    operand{&module->params[operandIndex]},
    output{&module->outputs[outputIndex]},
    operationSwitch{&module->params[operationSwitchIndex]},
    channelName{channelName} {
  module->configParam<FuncOperandKnobParamQuantity>(operandIndex, 0.F, 1.F, knob::centered);
  toggle::config<2>(module, operationSwitchIndex, "Operator" + channelName, {"Add (offset)", "Multiply (scale)"}, 0);
  toggle::config<4>(module, offsetRangeSwitchIndex, "Offset " + channelName + " range",
                    {"0–5 V", "±5 V", "0–10 V", "±10 V"}, 1);
  toggle::config<4>(module, multiplierRangeSwitchIndex, "Multiplier " + channelName + " range",
                    {"0–1", "±1", "0–2", "±2"}, 2);

  offsetRange = range::selector<4>(module, offsetRangeSwitchIndex, offsetRanges);
  multiplierRange = range::selector<4>(module, multiplierRangeSwitchIndex, multiplierRanges);

  auto const operandKnobParamQuantity
      = dynamic_cast<FuncOperandKnobParamQuantity *>(module->paramQuantities[operandIndex]);

  operandKnobParamQuantity->setChannel(this);
}

auto FuncChannel::apply(float upstream) -> float {
  auto const in = input->getNormalVoltage(upstream);
  auto const rotation = operand->getValue();
  auto const voltage = isMultiplication() ? multiply(in, rotation) : add(in, rotation);
  output->setVoltage(voltage);
  return voltage;
}

auto FuncChannel::add(float in, float rotation) const -> float { return in + offsetRange()->scale(rotation); }

auto FuncChannel::multiply(float in, float rotation) const -> float { return in * multiplierRange()->scale(rotation); }

auto FuncChannel::isMultiplication() const -> bool { return operationSwitch->getValue() > 0.5F; }

auto FuncChannel::currentOperandName() const -> std::string {
  auto const operandName = isMultiplication() ? "Multiplier" : "Offset";
  return operandName + channelName;
}

auto FuncChannel::currentOperandRange() const -> const Range * {
  return isMultiplication() ? multiplierRange() : offsetRange();
}

} // namespace dhe
