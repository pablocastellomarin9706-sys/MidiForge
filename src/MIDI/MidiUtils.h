#pragma once

#include "../Core/Types.h"
#include <juce_audio_basics/juce_audio_basics.h>

namespace MidiForge {

class MidiUtils {
public:
  static juce::MidiFile buildMidiFile(const SessionIdea& idea);
  static void applyHumanize(TrackIdea& track, double swingPct, double strumBeats, int strumDir);
};

} // namespace MidiForge
