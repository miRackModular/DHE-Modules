#pragma once

#include "Module.h"

#include "func/FuncChannel.h"

#include <memory>

namespace dhe {
class Func : public Module {
public:
  Func();

  void process(const ProcessArgs &args) override;

  enum ParameterIds {
    KNOB,
    OPERATOR_SWITCH,
    ADDITION_RANGE_SWITCH,
    MULTIPLICATION_RANGE_SWITCH,
    PARAMETER_COUNT
  };

  enum InputIds { IN, INPUT_COUNT };

  enum OutputIds { OUT, OUTPUT_COUNT };

  std::unique_ptr<FuncChannel> channel;
};
} // namespace dhe
