#pragma once

#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include "../PluginProcessor.h"
#include "LookAndFeel.h"

namespace MidiForge {

class MainComponent : public juce::Component {
public:
  explicit MainComponent(MidiForgeAudioProcessor& p);
  void paint(juce::Graphics&) override;
  void resized() override;

private:
  MidiForgeAudioProcessor& processor;
  std::unique_ptr<juce::Component> header;
  std::unique_ptr<juce::Component> sidebar;
  std::unique_ptr<juce::Component> tabs;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

} // namespace MidiForge
