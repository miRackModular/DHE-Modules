#pragma once

#include <stages/components/state-machine.h>

namespace DHE {
template <typename M> class StageGenerator : public PhaseAccumulator {
public:
  explicit StageGenerator(M *module, std::function<void()> on_stage_complete)
      : module{module}, on_stage_complete{std::move(on_stage_complete)} {}

  auto duration() const -> float override { return module->duration(); }

  auto sample_time() const -> float override { return module->sample_time(); }

  void on_advance(float phase) const override { module->generate(phase); }

  void on_finish() const override { on_stage_complete(); }

private:
  M *const module;
  const std::function<void()> on_stage_complete;
};

template <typename M> class Generating : public StageState<M> {
public:
  explicit Generating(M *module, PhaseAccumulator *generator,
                      const std::function<void()> &on_stage_gate_rise)
      : StageState<M>{module, on_stage_gate_rise}, generator{generator} {}

  void enter() override {
    this->become_active();
    this->prepare_to_generate();
    generator->start();
  }
  void step() override { generator->step(); }

  PhaseAccumulator *generator;
};

template <typename M> class StageStateMachine : public StateMachine<M> {
public:
  explicit StageStateMachine(M *module) : StateMachine<M>{module} {}

protected:
  void start_generating() override { this->enter(&generating); };

private:
  StageGenerator<M> stage_generator{this->module,
                                    [this]() { this->finish_stage(); }};

  Generating<M> generating{this->module, &stage_generator,
                           [this]() { start_generating(); }};
};
} // namespace DHE