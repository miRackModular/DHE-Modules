#pragma once

#include "CommonInputs.h"
#include "modules/components/Range.h"

#include <array>
#include <engine/Param.hpp>
#include <engine/Port.hpp>

namespace dhe {

/**
 * This curvature creates a gentle inverted S taper, increasing sensitivity in the middle of the knob rotation and
 * decreasing sensitivity toward the extremes.
 */
static auto constexpr durationKnobTaperCurvature = 0.8018017F;

/**
 * Each duration range is of the form [n, 1000n]. Given ranges of that form,
 * this curvature tapers the rotation so a knob positioned dead center
 * yields a duration equal to 1/10 of the range's upper bound (to within 7
 * decimal places).
 */
static auto constexpr durationKnobTaper = taper::FixedJTaper{durationKnobTaperCurvature};

auto constexpr shortDurationRange = Range{0.001F, 1.F};
auto constexpr mediumDurationRange = Range{0.01F, 10.F};
auto constexpr longDurationRange = Range{0.1F, 100.F};

extern std::array<Range const *, 3> const durationRanges;

static inline auto duration(rack::engine::Param const &knob, Range const &range) -> float {
  return taperedAndScaledRotation(knob, durationKnobTaper, range);
}

static inline auto selectableDuration(rack::engine::Param const &knob, rack::engine::Input const &cvInput,
                                      rack::engine::Param const &toggle) -> float {
  auto const range = selectedRange<3>(toggle, durationRanges);
  return taperedAndScaledRotation(knob, cvInput, durationKnobTaper, *range);
}

static inline auto selectableDuration(rack::engine::Param const &knob, rack::engine::Param const &toggle) -> float {
  auto const range = selectedRange<3>(toggle, durationRanges);
  return taperedAndScaledRotation(knob, durationKnobTaper, *range);
}

} // namespace dhe
