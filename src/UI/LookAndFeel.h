#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace MidiForge {

namespace Palette {
  const juce::Colour background{0xff0f0f0f};
  const juce::Colour panel{0xff1f1f1f};
  const juce::Colour panelAlt{0xff242424};
  const juce::Colour card{0xff2a2a2a};
  const juce::Colour border{0xff3a3a3a};
  const juce::Colour accent{0xff7c5cff};
  const juce::Colour accentDim{0xff9b8bff};
  const juce::Colour textPrimary{0xfff5f5f5};
  const juce::Colour textSecondary{0xffb5b5b5};
}

class MidiForgeLookAndFeel : public juce::LookAndFeel_V4 {
public:
  MidiForgeLookAndFeel();

  juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override;
  void drawButtonBackground(juce::Graphics&, juce::Button&, const juce::Colour& backgroundColour,
                            bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
  void drawTabButton(juce::TabBarButton&, juce::Graphics&, bool isMouseOver, bool isMouseDown) override;
};

} // namespace MidiForge
