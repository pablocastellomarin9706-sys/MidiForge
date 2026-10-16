#pragma once

#include "../Core/Types.h"
#include <juce_audio_basics/juce_audio_basics.h>

namespace MidiForge {

class MidiBuilder {
public:
  static juce::MidiFile build(const SessionIdea& idea);
};

} // namespace MidiForge
