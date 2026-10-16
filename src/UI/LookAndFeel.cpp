#include "LookAndFeel.h"

namespace MidiForge {

MidiForgeLookAndFeel::MidiForgeLookAndFeel() {
  setColour(juce::ResizableWindow::backgroundColourId, Palette::background);
  setColour(juce::TextButton::buttonColourId, Palette::card);
  setColour(juce::TextButton::buttonOnColourId, Palette::accent);
  setColour(juce::TextButton::textColourOffId, Palette::textPrimary);
  setColour(juce::TextButton::textColourOnId, juce::Colours::white);
  setColour(juce::Label::textColourId, Palette::textPrimary);
  setColour(juce::Label::textWhenEditingColourId, Palette::textPrimary);
  setColour(juce::TabbedComponent::backgroundColourId, Palette::panel);
  setColour(juce::TabbedButtonBar::tabOutlineColourId, Palette::border);
  setColour(juce::TabbedButtonBar::frontTextColourId, Palette::textPrimary);
  setColour(juce::TabbedButtonBar::tabTextColourId, Palette::textSecondary);
  setColour(juce::ComboBox::backgroundColourId, Palette::card);
  setColour(juce::ComboBox::textColourId, Palette::textPrimary);
  setColour(juce::ComboBox::outlineColourId, Palette::border);
  setColour(juce::PopupMenu::backgroundColourId, Palette::panelAlt);
  setColour(juce::PopupMenu::textColourId, Palette::textPrimary);
  setColour(juce::Slider::thumbColourId, Palette::accent);
  setColour(juce::Slider::trackColourId, Palette::border);
  setColour(juce::Slider::backgroundColourId, Palette::card);
}

juce::Font MidiForgeLookAndFeel::getTextButtonFont(juce::TextButton& b, int h) {
  (void)b;
  return juce::Font(juce::FontOptions(juce::jmax(12.0f, h * 0.38f)));
}

void MidiForgeLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                                const juce::Colour& backgroundColour,
                                                bool highlighted, bool down) {
  auto bounds = button.getLocalBounds().toFloat().reduced(0.5f);
  auto colour = backgroundColour;
  if(down) colour = Palette::accent.withMultipliedBrightness(0.8f);
  else if(highlighted) colour = Palette::accent.withMultipliedBrightness(1.1f);
  g.setColour(colour);
  g.fillRoundedRectangle(bounds, bounds.getHeight() * 0.3f);
  g.setColour(Palette::border);
  g.drawRoundedRectangle(bounds, bounds.getHeight() * 0.3f, 1.0f);
}

void MidiForgeLookAndFeel::drawTabButton(juce::TabBarButton& button, juce::Graphics& g,
                                         bool isMouseOver, bool isMouseDown) {
  auto bounds = button.getLocalBounds().toFloat();
  bool active = button.getToggleState();
  if(active) {
    g.setColour(Palette::accent.withAlpha(0.25f));
    g.fillRect(bounds);
    g.setColour(Palette::accent);
    g.fillRect(bounds.removeFromBottom(2.0f));
  } else if(isMouseOver || isMouseDown) {
    g.setColour(Palette::card);
    g.fillRect(bounds);
  }
  (void)isMouseDown;
}

} // namespace MidiForge
