#pragma once

namespace dhe {
namespace envelope {
namespace mode {
template <typename Signals> class IdleMode {
public:
  IdleMode(Signals &signals) : signals_{signals} {}

  void enter() { signals_.show_active(false); }
  void execute() {}
  void exit() { signals_.show_active(false); }

private:
  Signals &signals_;
};
} // namespace mode
} // namespace envelope
} // namespace dhe
