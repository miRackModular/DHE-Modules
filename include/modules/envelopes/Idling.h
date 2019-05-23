#pragma once
#include "StageState.h"

#include <functional>
#include <utility>

namespace dhe {

class Idling : public StageState {
public:
  explicit Idling(std::function<void()> start_generating,
                  std::function<void(bool)> const &set_active)
      : StageState{
            std::move(start_generating), // Start generating on stage gate rise
            []() {},                     // Ignore stage gate fall
            [set_active]() { set_active(false); }, // Become inactive on entry
            [](float) {}                           // Do nothing on each step
        } {}
};
} // namespace dhe
