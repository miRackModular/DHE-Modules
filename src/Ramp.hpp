#pragma once

// A linear ramp that progresses from 0 to 1.
// When the ramp reaches 1, it remains at 1
// until reset or restarted.
struct Ramp {
    bool running = false;
    float value = 0.0;

    // Starts the ramp with a value of 0.
    void start();

    // Stops the ramp with a value of 0.
    void stop();

    // Advances the ramp value by an amount that,
    // at the engine's current sample rate, would
    // transition from 0 to 1 over the given duration
    // (in seconds).
    void step(float duration);
};
