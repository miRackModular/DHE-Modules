#pragma once

#include <engine.hpp>
#include <modules/FuncChannel.h>

namespace DHE {
class Func : public rack::Module {
public:
  Func();
  void step() override;

  enum ParameterIds {
    KNOB,
    OPERATOR_SWITCH,
    ADDITION_RANGE_SWITCH,
    MULTIPLICATION_RANGE_SWITCH,
    PARAMETER_COUNT
  };

  enum InputIds { IN, INPUT_COUNT };

  enum OutputIds { OUT, OUTPUT_COUNT };

  FuncChannel channel{this, IN, KNOB, OUT};
};
} // namespace DHE