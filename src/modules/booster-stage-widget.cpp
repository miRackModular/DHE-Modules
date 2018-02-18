#include <componentlibrary.hpp>

#include "booster-stage-module.h"
#include "booster-stage-widget.h"

namespace DHE {

BoosterStageWidget::BoosterStageWidget(rack::Module *module) : Widget(module, 8, "res/BoosterStage.svg") {
  auto widget_right_edge{width()};

  auto left_x{7.f};
  auto center_x{widget_right_edge / 2.f};
  auto right_x{widget_right_edge - left_x};
  auto center_left_x = left_x + (right_x - left_x) / 3.f;
  auto center_right_x = right_x - (right_x - left_x) / 3.f;

  auto top_row_y{25.f};
  auto row_spacing{18.5f};

  auto row{0};
  install_input<rack::PJ301MPort>(BoosterStageModule::LEVEL_CV, {left_x, top_row_y + row*row_spacing});
  install_knob<rack::RoundBlackKnob>(BoosterStageModule::LEVEL_KNOB, {center_x, top_row_y + row*row_spacing});
  install_switch<rack::CKSS>(BoosterStageModule::LEVEL_SWITCH, {right_x, top_row_y + row*row_spacing});

  row++;
  install_input<rack::PJ301MPort>(BoosterStageModule::SHAPE_CV, {left_x, top_row_y + row*row_spacing});
  install_knob<rack::RoundBlackKnob>(BoosterStageModule::SHAPE_KNOB, {center_x, top_row_y + row*row_spacing});
  install_switch<rack::CKSS>(BoosterStageModule::SHAPE_SWITCH, {right_x, top_row_y + row*row_spacing});

  row++;
  install_input<rack::PJ301MPort>(BoosterStageModule::DURATION_CV, {left_x, top_row_y + row*row_spacing});
  install_knob<rack::RoundBlackKnob>(BoosterStageModule::DURATION_KNOB, {center_x, top_row_y + row*row_spacing});
  install_switch<rack::CKSSThree>(BoosterStageModule::DURATION_SWITCH, {right_x, top_row_y + row*row_spacing}, 2, 1);

  top_row_y = 82.f;
  row_spacing = 15.f;

  row = 0;
  install_input<rack::PJ301MPort>(BoosterStageModule::DEFER_INPUT, {left_x, top_row_y + row*row_spacing});
  install_button<rack::LEDButton>(BoosterStageModule::DEFER_BUTTON, {center_left_x, top_row_y + row*row_spacing});
  install_light<rack::MediumLight<rack::GreenLight>>(BoosterStageModule::DEFER_BUTTON_LIGHT, {center_left_x, top_row_y + row*row_spacing});
  install_button<rack::LEDButton>(BoosterStageModule::ACTIVE_BUTTON, {center_right_x, top_row_y + row*row_spacing});
  install_light<rack::MediumLight<rack::GreenLight>>(BoosterStageModule::ACTIVE_BUTTON_LIGHT, {center_right_x, top_row_y + row*row_spacing});
  install_output<rack::PJ301MPort>(BoosterStageModule::ACTIVE_OUTPUT, {right_x, top_row_y + row*row_spacing});

  row++;
  install_input<rack::PJ301MPort>(BoosterStageModule::TRIG_INPUT, {left_x, top_row_y + row*row_spacing});
  install_button<rack::LEDButton>(BoosterStageModule::TRIG_BUTTON, {center_left_x, top_row_y + row*row_spacing});
  install_light<rack::MediumLight<rack::GreenLight>>(BoosterStageModule::TRIG_BUTTON_LIGHT, {center_left_x, top_row_y + row*row_spacing});
  install_button<rack::LEDButton>(BoosterStageModule::EOC_BUTTON, {center_right_x, top_row_y + row*row_spacing});
  install_light<rack::MediumLight<rack::GreenLight>>(BoosterStageModule::EOC_BUTTON_LIGHT, {center_right_x, top_row_y + row*row_spacing});
  install_output<rack::PJ301MPort>(BoosterStageModule::EOC_OUTPUT, {right_x, top_row_y + row*row_spacing});

  row++;
  install_input<rack::PJ301MPort>(BoosterStageModule::IN_INPUT, {left_x, top_row_y + row*row_spacing});
  install_output<rack::PJ301MPort>(BoosterStageModule::OUT_OUTPUT, {right_x, top_row_y + row*row_spacing});
}
}