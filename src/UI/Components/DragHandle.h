#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../../PluginProcessor.h"

namespace MidiForge {

// Draggable handle that exports a MIDI clip for the given track.
class DragHandle : public juce::Component {
public:
  DragHandle(MidiForgeAudioProcessor& p, juce::String trackName);
  void paint(juce::Graphics&) override;
  void mouseDrag(const juce::MouseEvent&) override;
  void mouseDown(const juce::MouseEvent&) override;

  std::function<void()> onRegenerate;
  std::function<void()> onDrag;

private:
  MidiForgeAudioProcessor& processor;
  juce::String trackName;
  bool dragging{false};
  juce::Rectangle<int> regenArea;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DragHandle)
};

} // namespace MidiForge
