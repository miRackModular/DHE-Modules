#pragma once

namespace DHE {
class PhaseAccumulator {
public:
  PhaseAccumulator() {}
  void start() {
    accumulated = 0.f;
    on_start();
  }

  void step() {
    accumulated += sampleTime() / duration();
    if (accumulated >= 1.0f) {
      accumulated = 1.f;
    };
    if (accumulated >= 1.0f) {
      on_finish();
    };
  }

  auto phase() const -> float { return this->accumulated; }

protected:
  virtual void on_start() const {};
  virtual auto sampleTime() const -> float = 0;
  virtual auto duration() const -> float = 0;
  virtual void on_finish() const {};

private:
  float accumulated = 0.f;
};
} // namespace DHE
