#include "components/sigmoid.h"
#include "controls/curvature-inputs.h"
#include "controls/duration-inputs.h"
#include "controls/level-inputs.h"
#include "fake-rack/Controls.h"
#include "modules/curve-sequencer-2/controls.h"
#include "modules/curve-sequencer-2/source.h"

#include <doctest.h>

namespace test {
namespace curve_sequencer_2 {
namespace step_controls {
static auto constexpr step_count{8};
using Controls = dhe::curve_sequencer_2::Controls<fake::Port, fake::Param,
                                                  fake::Light, step_count>;
using dhe::curve_sequencer_2::Source;

TEST_CASE("curve_sequencer_2::StepControls") {
  std::vector<fake::Port> inputs{Controls::InputCount};
  std::vector<fake::Port> outputs{Controls::OutputCount};
  std::vector<fake::Param> params{Controls::ParamCount};
  std::vector<fake::Light> lights{Controls::LightCount};

  Controls controls{inputs, outputs, params, lights};

  SUBCASE("trackStartSource(step)") {
    auto constexpr step = 6;
    params[Controls::TrackStartSwitches + step].setValue(0.F); // SNAP
    CHECK_FALSE(controls.track_start_source(step));

    params[Controls::TrackStartSwitches + step].setValue(1.F); // TRACK
    CHECK(controls.track_start_source(step));
  }

  SUBCASE("startSource(step) is the source selected by the step start source "
          "switch") {
    auto constexpr step = 7;

    auto source_selected_by_switch = Source::Level;
    params[Controls::StartSourceSwitches + step].setValue(
        static_cast<float>(source_selected_by_switch));
    CHECK_EQ(controls.start_source(step), source_selected_by_switch);

    source_selected_by_switch = Source::In;
    params[Controls::StartSourceSwitches + step].setValue(
        static_cast<float>(source_selected_by_switch));
    CHECK_EQ(controls.start_source(step), source_selected_by_switch);

    source_selected_by_switch = Source::Out;
    params[Controls::StartSourceSwitches + step].setValue(
        static_cast<float>(source_selected_by_switch));
    CHECK_EQ(controls.start_source(step), source_selected_by_switch);
  }

  SUBCASE("startLevel(step) reports level for sequence level switch and step "
          "start level param") {
    auto constexpr step = 3;
    auto constexpr start_level_knob_rotation = 0.35F;
    auto constexpr level_range_selection = 1; // unipolar

    params[Controls::StartLevelKnobs + step].setValue(
        start_level_knob_rotation);
    params[Controls::LevelRangeSwitch].setValue(
        static_cast<float>(level_range_selection));

    CHECK_EQ(controls.start_level(step),
             dhe::level(start_level_knob_rotation,
                        dhe::signal_ranges[level_range_selection]));
  }

  SUBCASE("endSource(step) is the source selected by the step start source "
          "switch") {
    auto constexpr step = 7;

    auto source_selected_by_switch = Source::Level;
    params[Controls::EndSourceSwitches + step].setValue(
        static_cast<float>(source_selected_by_switch));
    CHECK_EQ(controls.end_source(step), source_selected_by_switch);

    source_selected_by_switch = Source::In;
    params[Controls::EndSourceSwitches + step].setValue(
        static_cast<float>(source_selected_by_switch));
    CHECK_EQ(controls.end_source(step), source_selected_by_switch);

    source_selected_by_switch = Source::Out;
    params[Controls::EndSourceSwitches + step].setValue(
        static_cast<float>(source_selected_by_switch));
    CHECK_EQ(controls.end_source(step), source_selected_by_switch);
  }

  SUBCASE("endLevel(step) reports level for sequence level switch and step end "
          "level param") {
    auto constexpr step = 3;
    auto constexpr end_level_knob_rotation = 0.35F;
    auto constexpr level_range_selection = 1; // unipolar

    params[Controls::EndLevelKnobs + step].setValue(end_level_knob_rotation);
    params[Controls::LevelRangeSwitch].setValue(
        static_cast<float>(level_range_selection));

    CHECK_EQ(controls.end_level(step),
             dhe::level(end_level_knob_rotation,
                        dhe::signal_ranges[level_range_selection]));
  }

  SUBCASE("trackEndSource(step)") {
    auto constexpr step = 1;
    params[Controls::TrackEndSwitches + step].setValue(0.F); // SNAP
    CHECK_FALSE(controls.track_end_source(step));

    params[Controls::TrackEndSwitches + step].setValue(1.F); // TRACK
    CHECK(controls.track_end_source(step));
  }

  SUBCASE("taper(step) is the taper selected by step shape switch") {
    auto constexpr step = 3;

    auto shape_selection = 0; // J
    params[Controls::ShapeSwitches + step].setValue(
        static_cast<float>(shape_selection));
    CHECK_EQ(controls.taper(step), dhe::sigmoid::tapers[shape_selection]);

    shape_selection = 1; // S
    params[Controls::ShapeSwitches + step].setValue(
        static_cast<float>(shape_selection));
    CHECK_EQ(controls.taper(step), dhe::sigmoid::tapers[shape_selection]);
  }

  SUBCASE("curvature(step) reports curvature for the step curvature param") {
    auto constexpr step = 5;
    auto constexpr curve_knob_rotation = 0.3F;
    params[Controls::CurveKnobs + step].setValue(curve_knob_rotation);

    CHECK_EQ(controls.curvature(step), dhe::curvature(curve_knob_rotation));
  }

  SUBCASE("duration(step) reports duration for duration range switch and step "
          "duration param") {
    auto constexpr step = 7;
    auto constexpr duration_knob_rotation = 0.75F;
    auto constexpr duration_range_selection = 2; // Long duration

    params[Controls::DurationKnobs + step].setValue(duration_knob_rotation);
    params[Controls::DurationRangeSwitch].setValue(
        static_cast<float>(duration_range_selection));

    CHECK_EQ(controls.duration(step),
             dhe::duration(duration_knob_rotation,
                           dhe::duration_ranges[duration_range_selection]));
  }

  SUBCASE("showInactive(step) dims step progress lights") {
    auto constexpr step = 3;
    auto constexpr completed_progress_light_index = step + step;
    auto constexpr remaining_progress_light_index = step + step + 1;

    controls.show_inactive(step);

    CHECK_EQ(lights[Controls::ProgressLights + completed_progress_light_index]
                 .getBrightness(),
             0.F);
    CHECK_EQ(lights[Controls::ProgressLights + remaining_progress_light_index]
                 .getBrightness(),
             0.F);
  }

  SUBCASE("isEnabled(step)") {
    SUBCASE("true if enabled button is pressed") {
      auto constexpr step = 3;
      params[Controls::EnabledButtons + step].setValue(1.F);

      CHECK(controls.is_enabled(step));
    }

    SUBCASE("true if enabled input is high") {
      auto constexpr step = 6;
      inputs[Controls::EnabledInputs + step].setVoltage(10.F);

      CHECK(controls.is_enabled(step));
    }

    SUBCASE("false if enabled button is not pressed and enabled input is low") {
      auto constexpr step = 7;

      params[Controls::EnabledButtons + step].setValue(0.F);
      inputs[Controls::EnabledInputs + step].setVoltage(0.F);

      CHECK_FALSE(controls.is_enabled(step));
    }
  }
}
} // namespace step_controls
} // namespace curve_sequencer_2
} // namespace test
