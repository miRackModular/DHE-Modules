#pragma once

namespace dhe {
namespace curve_sequencer_2 {
enum class Source { Level, In, Out };

static auto constexpr source_count = static_cast<int>(Source::Out) + 1;
} // namespace curve_sequencer_2
} // namespace dhe