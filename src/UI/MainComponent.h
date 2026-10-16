#pragma once

#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include "../PluginProcessor.h"
#include "LookAndFeel.h"
#include "Components/HeaderBar.h"
#include "Components/Sidebar.h"
#include "Components/TabView.h"

namespace MidiForge {

class MainComponent : public juce::Component {
public:
  explicit MainComponent(MidiForgeAudioProcessor& p);
  void paint(juce::Graphics&) override;
  void resized() override;

private:
  MidiForgeAudioProcessor& processor;
  std::unique_ptr<HeaderBar> header;
  std::unique_ptr<Sidebar> sidebar;
  std::unique_ptr<TabView> tabsView;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

} // namespace MidiForge
