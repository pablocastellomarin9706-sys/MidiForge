#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../../PluginProcessor.h"

namespace MidiForge {

class HeaderBar : public juce::Component {
public:
  explicit HeaderBar(MidiForgeAudioProcessor& p);
  void paint(juce::Graphics&) override;
  void resized() override;

  std::function<void()> onIdeaChanged;

private:
  MidiForgeAudioProcessor& processor;
  juce::TextButton generateButton{"GENERATE"};
  juce::TextButton shuffleButton{"Shuffle"};
  juce::Label titleLabel;
  juce::ComboBox keyBox, scaleBox, styleBox;
  juce::Slider barsSlider, seedSlider, swingSlider, brightnessSlider;
  juce::Label keyLabel, scaleLabel, styleLabel, barsLabel, seedLabel, swingLabel, brightLabel;

  void refreshFromIdea();
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeaderBar)
};

} // namespace MidiForge
