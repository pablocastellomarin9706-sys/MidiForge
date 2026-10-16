#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../../PluginProcessor.h"

namespace MidiForge {

class Sidebar : public juce::Component, public juce::ListBoxModel {
public:
  explicit Sidebar(MidiForgeAudioProcessor& p);
  void paint(juce::Graphics&) override;
  void resized() override;

  int getNumRows() override;
  void paintListBoxItem(int rowNumber, juce::Graphics&, int width, int height, bool rowIsSelected) override;

private:
  MidiForgeAudioProcessor& processor;
  juce::Label titleLabel;
  juce::ListBox historyList;
  juce::StringArray history;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Sidebar)
};

} // namespace MidiForge
