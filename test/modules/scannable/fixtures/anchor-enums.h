#pragma once

#include "modules/scannable/anchor.h"
#include <array>
#include <ostream>
#include <string>

namespace test {
namespace scannable {
using dhe::scannable::anchor_mode_count;
using dhe::scannable::anchor_source_count;
using dhe::scannable::anchor_type_count;
using dhe::scannable::AnchorMode;
using dhe::scannable::AnchorSource;
using dhe::scannable::AnchorType;

static auto constexpr anchor_types = std::array<AnchorType, anchor_type_count>{
    AnchorType::Start,
    AnchorType::End,
};

static inline auto name_of(AnchorType type) -> std::string {
  switch (type) {
  case AnchorType::Start:
    return "AnchorType::Start";
  case AnchorType::End:
    return "AnchorType::End";
  default:
    return "Unknown AnchorType " + std::to_string(static_cast<int>(type));
  }
}

static auto constexpr anchor_modes = std::array<AnchorMode, anchor_mode_count>{
    AnchorMode::Sample,
    AnchorMode::Track,
};

static inline auto name_of(AnchorMode mode) -> std::string {
  switch (mode) {
  case AnchorMode::Sample:
    return "AnchorMode::Sample";
  case AnchorMode::Track:
    return "AnchorMode::Track";
  default:
    return "Unknown AnchorMode " + std::to_string(static_cast<int>(mode));
  }
}

static auto constexpr anchor_sources =
    std::array<AnchorSource, anchor_source_count>{
        AnchorSource::InA,   AnchorSource::InB, AnchorSource::InC,
        AnchorSource::Level, AnchorSource::Out,
    };

static inline auto name_of(AnchorSource source) -> std::string {
  switch (source) {
  case AnchorSource::InA:
    return "AnchorSource::InA";
  case AnchorSource::InB:
    return "AnchorSource::InB";
  case AnchorSource::InC:
    return "AnchorSource::InC";
  case AnchorSource::Level:
    return "AnchorSource::Level";
  case AnchorSource::Out:
    return "AnchorSource::Out";
  default:
    return "Unknown AnchorSource " + std::to_string(static_cast<int>(source));
  }
}
} // namespace scannable
} // namespace test

namespace dhe {
namespace scannable {

static inline auto operator<<(std::ostream &os, AnchorMode mode)
    -> std::ostream & {
  os << test::scannable::name_of(mode);
  return os;
}

static inline auto operator<<(std::ostream &os,
                              dhe::scannable::AnchorSource source)
    -> std::ostream & {
  os << test::scannable::name_of(source);
  return os;
}

} // namespace scannable
} // namespace dhe
