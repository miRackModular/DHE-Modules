#include "modules/Blossom.h"

#include "modules/components/Taper.h"
#include "modules/controls/Controls.h"
#include "modules/controls/Level.h"
#include "util/Sigmoid.h"

#include <array>
#include <math.h>

namespace dhe {
static auto constexpr throbSpeedKnobCurvature = -0.8F;
static auto constexpr phaseOffsetRange = Range{-180.F, 180.F};
static auto constexpr bounceRatioRange = Range{1.F, 17.F};
static auto constexpr spinRange = Range{-10.F, 10.F};
static auto constexpr spinKnobTaper = taper::FixedSTaper{throbSpeedKnobCurvature};

inline auto rotationToSpin(float rotation) -> float {
  auto const tapered = spinKnobTaper.apply(rotation);
  return spinRange.scale(tapered);
}

inline auto spinToRotation(float spin) -> float {
  auto const tapered = spinRange.normalize(spin);
  return spinKnobTaper.invert(tapered);
}

class SpinKnobParamQuantity : public rack::engine::ParamQuantity {
  auto getDisplayValue() -> float override { return rotationToSpin(getValue()); }

  void setDisplayValue(float spin) override { setValue(spinToRotation(spin)); }
};

class BounceRatioKnobParamQuantity : public rack::engine::ParamQuantity {
public:
  auto getDisplayValue() -> float override {
    auto const rotation = getValue();
    auto const freeBounceRatio = bounceRatioRange.scale(rotation);
    auto const spin = blossom->isBounceFree() ? freeBounceRatio : std::round(freeBounceRatio);
    return spin;
  }

  void setDisplayValue(float bounceRatio) override {
    auto const rotation = bounceRatioRange.normalize(bounceRatio);
    setValue(rotation);
  }

  void setBlossom(Blossom *theBlossom) { blossom = theBlossom; }

private:
  Blossom *blossom;
};

Blossom::Blossom() {
  config(ParameterCount, InputCount, OutputCount);

  static auto constexpr initialSpinHz(0.5F);
  static auto const initialSpinKnobRotation = spinToRotation(initialSpinHz);
  configParam<SpinKnobParamQuantity>(SpinKnob, 0.F, 1.F, initialSpinKnobRotation, "Spin", " Hz");
  attenuverter::config(this, SpinAvKNob, "Spin CV gain");
  spin = knob::taperedAndScaled(this, SpinKnob, SpinCvInput, SpinAvKNob, spinKnobTaper, spinRange);

  configParam<BounceRatioKnobParamQuantity>(BounceRatioKnob, 0.F, 1.F, knob::centered, "Bounce ratio", " per spin");
  auto const bounceRatioParamQuantity = dynamic_cast<BounceRatioKnobParamQuantity *>(paramQuantities[BounceRatioKnob]);
  bounceRatioParamQuantity->setBlossom(this);

  attenuverter::config(this, BounceRatioAvKnob, "Bounce ratio CV gain");
  bounce = knob::scaled(this, BounceRatioKnob, BounceRatioCvInput, BounceRatioAvKnob, bounceRatioRange);

  toggle::config<2>(this, BounceRatioModeSwitch, "Bounce ratio mode", {"Quantized", "Free"}, 1);

  knob::configPercentage(this, BounceDepthKnob, "Bounce depth", {0.F, 1.F});
  attenuverter::config(this, BounceDepthAvKnob, "Bounce depth CV gain");
  depth = knob::rotation(this, BounceDepthKnob, BounceDepthCvInput, BounceDepthAvKnob);

  knob::config(this, BouncePhaseOffsetKnob, "Bounce phase offset", "°", phaseOffsetRange);
  attenuverter::config(this, BouncePhaseOffsetAvKnob, "Bounce phase offset CV gain");

  gain::config(this, XGainKnob, "X gain");
  level::configSwitch(this, XRangeSwitch, "X range", 0);

  gain::config(this, YGainKnob, "Y gain");
  level::configSwitch(this, YRangeSwitch, "Y range", 0);
}

void Blossom::process(const ProcessArgs &args) {
  auto spinDelta = -spin() * args.sampleTime;
  auto bounceRatio = isBounceFree() ? bounce() : std::round(bounce());
  auto bounceDepth = knob::rotationRange.clamp(depth());

  spinner.advance(spinDelta, 0.F);
  bouncer.advance(spinDelta * bounceRatio, phase());

  auto angle = spinner.angle();

  auto radius = (1.F - bounceDepth) + bounceDepth * bouncer.radius();
  auto x = radius * std::cos(angle);
  auto const xVoltage = 5.F * xGain() * (x + xOffset());
  outputs[XOutput].setVoltage(xVoltage);

  auto y = radius * std::sin(angle);
  auto const yVoltage = 5.F * yGain() * (y + yOffset());
  outputs[YOutput].setVoltage(yVoltage);
}

auto Blossom::offset(int param) -> float {
  auto isUni = params[param].getValue() > 0.5F;
  return isUni ? 1.F : 0.F;
}

auto Blossom::isBounceFree() -> bool { return params[BounceRatioModeSwitch].getValue() > 0.1F; }

auto Blossom::phase() -> float {
  auto const rotation = modulated(BouncePhaseOffsetKnob, BouncePhaseCvInput, BouncePhaseOffsetAvKnob);
  return rotation - 0.5F;
}

auto Blossom::xOffset() -> float { return offset(XRangeSwitch); }

auto Blossom::xGain() -> float {
  float gainAmount = modulated(XGainKnob, XGainCvInput);
  return gain::range.scale(gainAmount);
}

auto Blossom::yGain() -> float {
  float gainAmount = modulated(YGainKnob, YGainCvInput);
  return gain::range.scale(gainAmount);
}

auto Blossom::yOffset() -> float { return offset(YRangeSwitch); }

} // namespace dhe
