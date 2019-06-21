#include "modules/Gator.h"

#include "modules/controls/CommonConfig.h"
#include "modules/controls/CommonInputs.h"

#include <string>

namespace dhe {

template <typename LogicOperator>
auto reduce(std::vector<bool> const &values, bool identity, LogicOperator op) -> bool {
  auto accumulated = identity;
  for (auto const value : values) {
    accumulated = op(accumulated, value);
  }
  return accumulated;
}

Gator::Gator() {
  config(ParameterCount, InputCount, OutputCount);
  for (auto i = 0; i < inputCount; i++) {
    configParam(NegateButtons + i, 0.F, 1.F, 0.F, "Negate input " + std::to_string(i + 1));
  }
}

void Gator::process(const rack::engine::Module::ProcessArgs & /*ignored*/) {
  auto connectedCount = 0;
  auto highCount = 0;

  for (int i = 0; i < inputCount; i++) {
    if (inputs[i].isConnected()) {
      connectedCount++;
      if (inputIsHigh(this, Inputs + i) != buttonIsPressed(this, NegateButtons + i)) {
        highCount++;
      }
    }
  }

  if (connectedCount == 0) {
    setAllOutputsFalse();
  } else {
    setOutputs(AndOutput, NandOutput, highCount == connectedCount);
    setOutputs(OrOutput, NorOutput, highCount > 0);
    setOutputs(OddOutput, EvenOutput, (highCount & 1) > 0);
    setOutputs(XorOutput, XnorOutput, highCount == 1);
  }
}

} // namespace dhe
