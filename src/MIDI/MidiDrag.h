#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_basics/juce_audio_basics.h>

namespace MidiForge {

class MidiDrag {
public:
  // Writes the MidiFile to a temp .mid file and starts a native OS file drag,
  // which FL Studio accepts when dropped on the Piano Roll / Channel Rack.
  // Must be called from within a mouseDrag callback.
  static bool startDraggingMidi(juce::Component* source, const juce::MidiFile& mf,
                                const juce::String& clipName);
};

} // namespace MidiForge
