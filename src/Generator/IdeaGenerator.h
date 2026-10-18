#pragma once

#include "../Core/Types.h"
#include "../Core/RandomGen.h"

namespace MidiForge {

class IdeaGenerator {
public:
  void generateFull(SessionIdea& idea);
  void regenerateTrack(SessionIdea& idea, const juce::String& trackName, int seedOffset=0);

  // "Variacion" knob: deterministic extra humanisation layered on top of a
  // finished idea (velocity variance, micro-timing, octave sparkle).
  static void applyCreativity(SessionIdea& idea, float amount, int seedSalt = 0);
  static void applyCreativityToTrack(TrackIdea& track, float amount, juce::Random& rng);
private:
  void clearTrack(SessionIdea& idea, const juce::String& name);
  void pickAll(SessionIdea& idea, RandomGen& rng);
  void runAll(SessionIdea& idea, RandomGen& rng);
};

} // namespace MidiForge
