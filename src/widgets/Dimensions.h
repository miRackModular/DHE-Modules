#pragma once

#include "PanelAssets.h"

#include <string>
#include <widget/Widget.hpp>

namespace dhe {
auto constexpr px_per_hp = 15.F;
auto constexpr mm_per_in = 25.4F;
auto constexpr px_per_in = 75.F;
auto constexpr mm_per_hp = 5.08F;
auto constexpr px_per_mm = px_per_in / mm_per_in;

auto constexpr hp2px(float hp) -> float { return hp * px_per_hp; }
auto constexpr hp2mm(float hp) -> float { return hp * mm_per_hp; }
auto constexpr mm2px(float mm) -> float { return mm * px_per_mm; }
auto constexpr mm2hp(float mm) -> float { return mm / mm_per_hp; }

static inline auto mm2px(float xmm, float ymm) -> rack::math::Vec {
  return {mm2px(xmm), mm2px(ymm)};
}
static inline auto mm2px(rack::math::Vec const &mm) -> rack::math::Vec {
  return mm2px(mm.x, mm.y);
}

static inline void positionCentered(rack::widget::Widget *widget, float xmm,
                                    float ymm) {
  auto const &center = mm2px(xmm, ymm);
  auto const &offset = widget->box.size.div(2);
  widget->setPosition(center.minus(offset));
}

auto constexpr moduleHeight = 128.5F; // mm
auto constexpr portRadius = 4.2F;
auto constexpr buttonRadius = 3.F;
auto constexpr lightRadius = 1.088F;
auto constexpr lightDiameter = lightRadius * 2.F;
auto constexpr buttonPortDistance = 7.891F;

} // namespace dhe
