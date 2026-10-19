#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace MidiForge {

// "Forge" theme: warm charcoal, molten orange, a hot magenta and an electric
// cyan. The instrument reads like a dark workshop: near-black warm metal with
// glowing molten accents. Chamfered ("cut corner") geometry gives it an anvil /
// industrial identity instead of the usual soft rounded cards.
namespace Palette {
  const juce::Colour background{0xff0c0a0e};   // near-black, warm
  const juce::Colour panel{0xff141017};
  const juce::Colour panelAlt{0xff1a141d};
  const juce::Colour card{0xff241c28};
  const juce::Colour cardHover{0xff33253a};
  const juce::Colour border{0xff3a2a3c};
  const juce::Colour accent{0xffff7a1a};       // molten orange: the brand colour
  const juce::Colour accentSoft{0xffffb878};   // light ember
  const juce::Colour accentDim{0xff7a3410};    // cooling metal
  const juce::Colour ember{0xffe2341a};        // deep red heat
  const juce::Colour hot{0xffff2e88};          // hot magenta
  const juce::Colour cool{0xff2fe0ff};         // electric cyan: cold contrast
  const juce::Colour fit{0xff3fe0a0};          // vivid mint: "fits the project"
  const juce::Colour star{0xffffd15c};         // warm gold: favourites
  const juce::Colour textPrimary{0xfff7efe8};
  const juce::Colour textSecondary{0xffb09aa8};
}

// Vibrant pitch-class spectrum, brightened by velocity. Anchored warm (C reads
// red) and sweeping through the whole wheel so the piano roll / drum grid pop.
inline juce::Colour noteColour(int pitch, float velocity01) {
  static const float hues[12] = {12.0f, 32.0f, 50.0f, 90.0f, 150.0f, 175.0f,
                                 190.0f, 210.0f, 245.0f, 280.0f, 310.0f, 340.0f};
  const int pc = ((pitch % 12) + 12) % 12;
  const float v = 0.60f + 0.38f * juce::jlimit(0.0f, 1.0f, velocity01);
  return juce::Colour::fromHSV(hues[pc] / 360.0f, 0.80f, v, 1.0f);
}

// Chamfered ("cut corner") rectangle: the signature shape of the Forge theme.
inline juce::Path chamferRect(juce::Rectangle<float> r, float cut) {
  cut = juce::jmin(cut, juce::jmin(r.getWidth(), r.getHeight()) * 0.5f - 0.5f);
  juce::Path p;
  p.startNewSubPath(r.getX() + cut, r.getY());
  p.lineTo(r.getRight() - cut, r.getY());
  p.lineTo(r.getRight(), r.getY() + cut);
  p.lineTo(r.getRight(), r.getBottom() - cut);
  p.lineTo(r.getRight() - cut, r.getBottom());
  p.lineTo(r.getX() + cut, r.getBottom());
  p.lineTo(r.getX(), r.getBottom() - cut);
  p.lineTo(r.getX(), r.getY() + cut);
  p.closeSubPath();
  return p;
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
