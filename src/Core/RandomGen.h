#pragma once

#include <juce_core/juce_core.h>

namespace MidiForge {

class RandomGen {
public:
  explicit RandomGen(int seed) : seedVal(seed), rng(seed) {}
  int nextInt(int min, int max) {
    if(max <= min) return min;
    return rng.nextInt(max - min + 1) + min;
  }
  double nextDouble() { return rng.nextDouble(); }
  int getSeed() const { return seedVal; }

private:
  int seedVal{12345};
  juce::Random rng;
};

} // namespace MidiForge
