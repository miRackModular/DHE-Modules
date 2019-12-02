#include "Gator.h"

#include "controls/CommonInputs.h"

#include <string>

namespace dhe {

Gator::Gator() {
  config(ParameterCount, InputCount, OutputCount);
  for (auto i = 0; i < inputCount; i++) {
    configParam(NegateButtons + i, 0.F, 1.F, 0.F, "Negate input " + std::to_string(i + 1));
  }
}

void Gator::process(ProcessArgs const & /*ignored*/) {
  auto connectedCount = 0;
  auto highCount = 0;

  for (auto i = 0; i < inputCount; i++) {
    if (inputs[i].isConnected()) {
      connectedCount++;
      if (isHigh(inputs[(Inputs + i)]) != isPressed(params[NegateButtons + i])) {
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
