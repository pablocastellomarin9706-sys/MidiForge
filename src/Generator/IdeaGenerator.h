#pragma once

#include "../Core/Types.h"
#include "../Core/RandomGen.h"

namespace MidiForge {

class IdeaGenerator {
public:
  void generateFull(SessionIdea& idea);
  void regenerateTrack(SessionIdea& idea, const juce::String& trackName, int seedOffset=0);
private:
  void clearTrack(SessionIdea& idea, const juce::String& name);
  void pickAll(SessionIdea& idea, RandomGen& rng);
  void runAll(SessionIdea& idea, RandomGen& rng);
};

} // namespace MidiForge
