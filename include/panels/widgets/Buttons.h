#pragma once
#include <utility>

#include <app/SvgSwitch.hpp>
#include <componentlibrary.hpp>

#include "Buttons.h"

namespace DHE {

template <typename P> class Button : public rack::app::SvgSwitch {
public:
  explicit Button(const std::string &name = "button") {
    momentary = true;
    addFrame(P::svg(name + "-1"));
    addFrame(P::svg(name + "-2"));
  }
}; // namespace DHE

template <typename P> class ReverseButton : public Button<P> {
public:
  ReverseButton() : Button<P>("button-reversed") {}
};

} // namespace DHE
