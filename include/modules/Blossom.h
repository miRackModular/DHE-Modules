#pragma once

#include "modules/components/Rotor.h"
#include "modules/controls/CommonInputs.h"

#include <engine/Module.hpp>

namespace dhe {

static auto constexpr bounceRange = Range{1.F, 17.F};
static auto constexpr spinKnobTaperCurvature = -0.8F;
static auto constexpr spinKnobTaper = taper::FixedSTaper{spinKnobTaperCurvature};
static auto constexpr spinRange = Range{-10.F, 10.F};

class Blossom : public rack::engine::Module {
public:
  Blossom();
  void process(const ProcessArgs &args) override;

  enum ParameterIds {
    SpinKnob,
    SpinAvKNob,
    BounceRatioKnob,
    BounceRatioAvKnob,
    BounceRatioModeSwitch,
    BounceDepthKnob,
    BounceDepthAvKnob,
    BouncePhaseOffsetKnob,
    BouncePhaseOffsetAvKnob,
    XGainKnob,
    YGainKnob,
    XRangeSwitch,
    YRangeSwitch,
    ParameterCount
  };
  enum InputIds {
    SpinCvInput,
    BounceRatioCvInput,
    BounceDepthCvInput,
    BouncePhaseCvInput,
    XGainCvInput,
    YGainCvInput,
    InputCount
  };
  enum OutputIds { XOutput, YOutput, OutputCount };

private:
  inline auto bounce() const -> float {
    return scaledRotation(this, BounceRatioKnob, BounceRatioCvInput, BounceRatioAvKnob, bounceRange);
  }

  inline auto bounceIsFree() const -> bool { return switchPosition(this, BounceRatioModeSwitch) == 1; }

  inline auto depth() const -> float { return rotation(this, BounceDepthKnob, BounceDepthCvInput, BounceDepthAvKnob); }

  inline auto phase() const -> float {
    return scaledRotation(this, BouncePhaseOffsetKnob, BouncePhaseCvInput, BouncePhaseOffsetAvKnob, {-0.5, 0.5});
  }

  inline auto spin() const -> float {
    return taperedAndScaledRotation(this, SpinKnob, SpinCvInput, SpinAvKNob, spinKnobTaper, spinRange);
  }

  inline auto xGain() const -> float { return scaledRotation(this, XGainKnob, XGainCvInput, gainRange); }
  inline auto xOffset() const -> float { return selected<float, 2>(this, XRangeSwitch, {0.F, 1.F}); };

  inline auto yGain() const -> float { return scaledRotation(this, YGainKnob, YGainCvInput, gainRange); }
  inline auto yOffset() const -> float { return selected<float, 2>(this, YRangeSwitch, {0.F, 1.F}); };

  Rotor spinner{};
  Rotor bouncer{};
};
} // namespace dhe
