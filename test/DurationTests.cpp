#include <util/sigmoid.h>
#include "modules/controls/Duration.h"
#include "gmock/gmock.h"

namespace {

using ::testing::AnyNumber;
using ::testing::AtMost;
using ::testing::Expectation;
using ::testing::NiceMock;
using ::testing::Return;

struct DurationTest : public ::testing::Test {

  // tolerance = 6 decimal places
  static constexpr float tolerance = 0.000001f;
  static constexpr float short_range_switch_position = 0.f;
  static constexpr float medium_range_switch_position = 1.f;
  static constexpr float long_range_switch_position = 2.f;

  rack::engine::Param knob_param{};
  rack::engine::Param switch_param{};
  rack::engine::Input cv_input{};

  DHE::Duration duration{};

  DurationTest() {
    cv_input.setVoltage(0.f);
    duration.config(&knob_param, &switch_param, &cv_input);
  }
};

TEST_F(DurationTest, shortRange_minimumDuration_isShortRangeLowerBound) {
  switch_param.setValue(short_range_switch_position);
  knob_param.setValue(0.f);

  const auto expected = DHE::Duration::short_range.lower_bound;
  ASSERT_NEAR(duration(), expected, expected * tolerance);
}

TEST_F(DurationTest, shortRange_middleDuration_isOneTenthOfShortRangeUpperBound) {
  switch_param.setValue(short_range_switch_position);
  knob_param.setValue(0.5f);

  const auto expected = DHE::Duration::short_range.upper_bound / 10.f;
  ASSERT_NEAR(duration(), expected, expected * tolerance);
}

TEST_F(DurationTest, shortRange_maximumDuration_isShortRangeUpperBound) {
  switch_param.setValue(short_range_switch_position);
  knob_param.setValue(1.f);

  const auto expected = DHE::Duration::short_range.upper_bound;
  ASSERT_NEAR(duration(), expected, expected * tolerance);
}

TEST_F(DurationTest, mediumRange_minimumDuration_isMediumRangeLowerBound) {
  switch_param.setValue(medium_range_switch_position);
  knob_param.setValue(0.f);

  const auto expected = DHE::Duration::medium_range.lower_bound;
  ASSERT_NEAR(duration(), expected, expected * tolerance);
}

TEST_F(DurationTest, mediumRange_middleDuration_isOneTenthOfMediumRangeUpperBound) {
  switch_param.setValue(medium_range_switch_position);
  knob_param.setValue(0.5f);

  const auto expected = DHE::Duration::medium_range.upper_bound / 10.f;
  ASSERT_NEAR(duration(), expected, expected * tolerance);
}

TEST_F(DurationTest, mediumRange_maximumDuration_isMediumRangeUpperBound) {
  switch_param.setValue(medium_range_switch_position);
  knob_param.setValue(1.f);

  const auto expected = DHE::Duration::medium_range.upper_bound;
  ASSERT_NEAR(duration(), expected, expected * tolerance);
}

TEST_F(DurationTest, longRange_minimumDuration_isLongRangeLowerBound) {
  switch_param.setValue(long_range_switch_position);
  knob_param.setValue(0.f);

  const auto expected = DHE::Duration::long_range.lower_bound;
  ASSERT_NEAR(duration(), expected, expected * tolerance);
}

TEST_F(DurationTest, longRange_middleDuration_isOneTenthOfLongRangeUpperBound) {
  switch_param.setValue(long_range_switch_position);
  knob_param.setValue(0.5f);

  const auto expected = DHE::Duration::long_range.upper_bound / 10.f;
  ASSERT_NEAR(duration(), expected, expected * tolerance);
}

TEST_F(DurationTest, longRange_maximumDuration_isLongRangeUpperBound) {
  switch_param.setValue(long_range_switch_position);
  knob_param.setValue(1.f);

  const auto expected = DHE::Duration::long_range.upper_bound;
  ASSERT_NEAR(duration(), expected, expected * tolerance);
}
} // namespace