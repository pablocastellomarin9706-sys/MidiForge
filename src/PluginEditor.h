#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "UI/MainComponent.h"
#include "UI/LookAndFeel.h"

namespace MidiForge {

class MidiForgeAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
  explicit MidiForgeAudioProcessorEditor(MidiForgeAudioProcessor&);
  ~MidiForgeAudioProcessorEditor() override;

  void paint(juce::Graphics&) override;
  void resized() override;

private:
  MidiForgeAudioProcessor& processor;
  std::unique_ptr<MainComponent> mainComp;
  MidiForgeLookAndFeel lookAndFeel;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiForgeAudioProcessorEditor)
};

} // namespace MidiForge
