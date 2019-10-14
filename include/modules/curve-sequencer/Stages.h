#pragma once

#include "modules/components/Latch.h"

#include <array>
#include <string>

namespace dhe {
namespace curve_sequencer {
  static auto constexpr riseMode = 0;
  static auto constexpr fallMode = 1;
  static auto constexpr edgeMode = 2;
  static auto constexpr highMode = 3;
  static auto constexpr lowMode = 4;
  static auto constexpr skipMode = 5;
  static auto constexpr durationMode = 6;

  static auto constexpr generateModeCount = durationMode + 1;
  static auto constexpr generateDefaultMode = durationMode;
  static auto constexpr sustainModeCount = skipMode + 1;
  static auto constexpr sustainDefaultMode = skipMode;

  static auto const generateModeDescriptions = std::array<std::string, generateModeCount>{
      "Stop curve if gate rises",   "Stop curve if gate falls",  "Stop curve if gate changes",
      "Stop curve if gate is high", "Stop curve if gate is low", "No curve",
      "Generate entire curve"};

  static auto const sustainModeDescriptions = std::array<std::string, sustainModeCount>{
      "Sustain until gate rises",   "Sustain until gate falls",  "Sustain until gate changes",
      "Sustain until gate is high", "Sustain until gate is low", "No sustain"};

  static auto isActive(int mode, Latch const &gateLatch) -> bool {
    switch (mode) {
    case riseMode:
      return !gateLatch.isRise();
    case fallMode:
      return !gateLatch.isFall();
    case edgeMode:
      return !gateLatch.isEdge();
    case highMode:
      return !gateLatch.isHigh();
    case lowMode:
      return !gateLatch.isLow();
    default:
      return false;
    }
  }

  template <typename C> class GenerateStage {
  public:
    GenerateStage(C &controls) : controls{controls} {}
    auto execute(int, Latch const &, float) -> bool { return false; }

  private:
    C &controls;
  };

  template <typename C> class SustainStage {
  public:
    SustainStage(C &controls) : controls{controls} {}
    auto execute(int step, Latch const &gateLatch) -> bool {
      auto const active = isActive(controls.sustainMode(step), gateLatch);
      controls.setSustaining(step, active);
      return active;
    }

  private:
    C &controls;
  };
} // namespace curve_sequencer
} // namespace dhe
