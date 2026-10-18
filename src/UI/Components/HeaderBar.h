#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../../PluginProcessor.h"

namespace MidiForge {

class HeaderBar : public juce::Component, private juce::Timer {
public:
  explicit HeaderBar(MidiForgeAudioProcessor& p);
  void paint(juce::Graphics&) override;
  void resized() override;
  void refreshFromIdea();

  std::function<void()> onIdeaChanged;
  std::function<void()> onChatToggle;

private:
  void timerCallback() override;

  MidiForgeAudioProcessor& processor;
  juce::TextButton generateButton{"GENERATE"};
  juce::TextButton shuffleButton{"Shuffle"};
  juce::TextButton chatButton{"Chat"};
  juce::Label titleLabel;
  juce::ComboBox keyBox, scaleBox, styleBox;
  juce::Slider barsSlider, seedSlider, swingSlider, brightnessSlider;
  juce::Label keyLabel, scaleLabel, styleLabel, barsLabel, seedLabel, swingLabel, brightLabel;

  double sheenPhase{0.0};

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeaderBar)
};

} // namespace MidiForge
