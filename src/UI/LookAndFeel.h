#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace MidiForge {

// "Acero" theme: deep steel blue, minimal, cold light accents.
namespace Palette {
  const juce::Colour background{0xff0b0f14};
  const juce::Colour panel{0xff10161e};
  const juce::Colour panelAlt{0xff141b25};
  const juce::Colour card{0xff1b2531};
  const juce::Colour cardHover{0xff233042};
  const juce::Colour border{0xff27323f};
  const juce::Colour accent{0xff4d9de0};
  const juce::Colour accentSoft{0xff9fd0f5};
  const juce::Colour accentDim{0xff2c5f8a};
  const juce::Colour textPrimary{0xffe9eff5};
  const juce::Colour textSecondary{0xff8fa0b3};
}

// Steel note colour: pitch classes across a deep steel -> ice ramp,
// brightened by velocity. Used by the piano roll / drum grid previews.
inline juce::Colour noteColour(int pitch, float velocity01) {
  const int pc = ((pitch % 12) + 12) % 12;
  const float t = (float)pc / 11.0f;
  auto c = Palette::accentDim.interpolatedWith(juce::Colour(0xffb5dcff), 0.25f + 0.75f * t);
  return c.withMultipliedBrightness(0.8f + 0.4f * juce::jlimit(0.0f, 1.0f, velocity01));
}

class MidiForgeLookAndFeel : public juce::LookAndFeel_V4 {
public:
  MidiForgeLookAndFeel();

  juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override;
  void drawButtonBackground(juce::Graphics&, juce::Button&, const juce::Colour& backgroundColour,
                            bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
  void drawTabButton(juce::TabBarButton&, juce::Graphics&, bool isMouseOver, bool isMouseDown) override;
  void drawComboBox(juce::Graphics&, int width, int height, bool isButtonDown,
                    int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox&) override;
  void drawLinearSlider(juce::Graphics&, int x, int y, int width, int height,
                        float sliderPos, float minSliderPos, float maxSliderPos,
                        const juce::Slider::SliderStyle, juce::Slider&) override;
  void drawScrollbar(juce::Graphics&, juce::ScrollBar&, int x, int y, int width, int height,
                     bool isScrollbarVertical, int thumbStartPosition, int thumbSize,
                     bool isMouseOver, bool isMouseDown) override;
  void fillTextEditorBackground(juce::Graphics&, int width, int height, juce::TextEditor&) override;
  void drawTextEditorOutline(juce::Graphics&, int width, int height, juce::TextEditor&) override;
};

} // namespace MidiForge
