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
  setColour(juce::TabbedButtonBar::tabOutlineColourId, juce::Colours::transparentBlack);
  setColour(juce::TabbedButtonBar::frontTextColourId, Palette::textPrimary);
  setColour(juce::TabbedButtonBar::tabTextColourId, Palette::textSecondary);

  setColour(juce::ComboBox::backgroundColourId, Palette::card);
  setColour(juce::ComboBox::textColourId, Palette::textPrimary);
  setColour(juce::ComboBox::outlineColourId, Palette::border);
  setColour(juce::ComboBox::arrowColourId, Palette::textSecondary);

  setColour(juce::PopupMenu::backgroundColourId, Palette::panelAlt);
  setColour(juce::PopupMenu::textColourId, Palette::textPrimary);
  setColour(juce::PopupMenu::highlightedBackgroundColourId, Palette::accent.withAlpha(0.28f));
  setColour(juce::PopupMenu::highlightedTextColourId, juce::Colours::white);

  setColour(juce::Slider::thumbColourId, Palette::accent);
  setColour(juce::Slider::trackColourId, Palette::accent);
  setColour(juce::Slider::backgroundColourId, Palette::background.brighter(0.10f));
  setColour(juce::Slider::textBoxTextColourId, Palette::textPrimary);
  setColour(juce::Slider::textBoxBackgroundColourId, Palette::card);
  setColour(juce::Slider::textBoxOutlineColourId, Palette::border);
  setColour(juce::Slider::textBoxHighlightColourId, Palette::accent.withAlpha(0.35f));

  setColour(juce::TextEditor::backgroundColourId, Palette::card);
  setColour(juce::TextEditor::textColourId, Palette::textPrimary);
  setColour(juce::TextEditor::outlineColourId, Palette::border);
  setColour(juce::TextEditor::focusedOutlineColourId, Palette::accent);
  setColour(juce::TextEditor::highlightColourId, Palette::accent.withAlpha(0.35f));
  setColour(juce::TextEditor::highlightedTextColourId, juce::Colours::white);
  setColour(juce::CaretComponent::caretColourId, Palette::accentSoft);

  setColour(juce::ListBox::backgroundColourId, Palette::panelAlt);
  setColour(juce::ListBox::outlineColourId, Palette::border);
  setColour(juce::ListBox::textColourId, Palette::textPrimary);

  setColour(juce::ScrollBar::backgroundColourId, juce::Colours::transparentBlack);
  setColour(juce::ScrollBar::thumbColourId, Palette::border.brighter(0.2f));

  setColour(juce::TooltipWindow::backgroundColourId, Palette::panelAlt);
  setColour(juce::TooltipWindow::textColourId, Palette::textPrimary);
  setColour(juce::TooltipWindow::outlineColourId, Palette::border);

  setColour(juce::ToggleButton::textColourId, Palette::textPrimary);
  setColour(juce::ToggleButton::tickColourId, Palette::accent);
  setColour(juce::ToggleButton::tickDisabledColourId, Palette::border);
}

juce::Font MidiForgeLookAndFeel::getTextButtonFont(juce::TextButton&, int buttonHeight) {
  return juce::Font(juce::FontOptions(juce::jmax(11.5f, (float)buttonHeight * 0.36f)));
}

void MidiForgeLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                                const juce::Colour& backgroundColour,
                                                bool shouldDrawButtonAsHighlighted,
                                                bool shouldDrawButtonAsDown) {
  auto bounds = button.getLocalBounds().toFloat().reduced(0.5f);
  const float radius = juce::jmin(6.0f, bounds.getHeight() * 0.25f);
  const bool primary = button.getToggleState() || backgroundColour == Palette::accent;

  juce::Colour base = primary ? Palette::accent : backgroundColour;

  if (shouldDrawButtonAsDown) base = base.darker(0.25f);
  else if (shouldDrawButtonAsHighlighted) base = base.brighter(primary ? 0.08f : 0.18f);

  juce::ColourGradient grad(base.brighter(primary ? 0.05f : 0.07f), bounds.getX(), bounds.getY(),
                            base.darker(primary ? 0.09f : 0.14f), bounds.getX(), bounds.getBottom(), false);
  g.setGradientFill(grad);
  g.fillRoundedRectangle(bounds, radius);

  // Metal top highlight.
  g.setColour(juce::Colours::white.withAlpha(primary ? 0.20f : 0.06f));
  g.drawHorizontalLine((int)bounds.getY() + 1, bounds.getX() + radius, bounds.getRight() - radius);

  // Border + hover glow.
  if (shouldDrawButtonAsHighlighted && !primary) {
    g.setColour(Palette::accent.withAlpha(0.85f));
  } else if (primary) {
    g.setColour(Palette::accentSoft.withAlpha(0.55f));
  } else {
    g.setColour(Palette::border);
  }
  g.drawRoundedRectangle(bounds, radius, 1.0f);

  if (primary) {
    g.setColour(Palette::accent.withAlpha(0.22f));
    g.drawRoundedRectangle(bounds.expanded(1.0f), radius + 1.0f, 1.0f);
  }
}

void MidiForgeLookAndFeel::drawTabButton(juce::TabBarButton& button, juce::Graphics& g,
                                         bool isMouseOver, bool isMouseDown) {
  auto bounds = button.getLocalBounds().toFloat();
  const bool active = button.getToggleState();

  if (active) {
    g.setColour(Palette::panelAlt.brighter(0.06f));
    g.fillRoundedRectangle(bounds.reduced(3.0f, 4.0f), 5.0f);
    g.setColour(Palette::accent);
    g.fillRoundedRectangle(bounds.removeFromBottom(2.5f).reduced(6.0f, 0.0f), 1.25f);
  } else if (isMouseOver || isMouseDown) {
    g.setColour(Palette::card.withAlpha(0.65f));
    g.fillRoundedRectangle(bounds.reduced(3.0f, 4.0f), 5.0f);
  }

  g.setColour(active ? Palette::textPrimary : Palette::textSecondary);
  g.setFont(juce::Font(juce::FontOptions(12.0f, active ? juce::Font::bold : juce::Font::plain)));
  g.drawText(button.getButtonText(), button.getLocalBounds(), juce::Justification::centred);
}

void MidiForgeLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                                        int, int, int, int, juce::ComboBox& box) {
  auto bounds = juce::Rectangle<float>(0.0f, 0.0f, (float)width, (float)height).reduced(0.5f);
  const float radius = juce::jmin(6.0f, bounds.getHeight() * 0.25f);

  g.setColour(isButtonDown ? Palette::cardHover : Palette::card);
  g.fillRoundedRectangle(bounds, radius);

  g.setColour(box.hasKeyboardFocus(false) ? Palette::accent.withAlpha(0.85f) : Palette::border);
  g.drawRoundedRectangle(bounds, radius, 1.0f);

  // Steel chevron.
  juce::Path p;
  const float cx = bounds.getRight() - 12.0f;
  const float cy = bounds.getCentreY();
  p.startNewSubPath(cx - 4.0f, cy - 2.0f);
  p.lineTo(cx, cy + 3.0f);
  p.lineTo(cx + 4.0f, cy - 2.0f);
  g.setColour(Palette::textSecondary);
  g.strokePath(p, juce::PathStrokeType(1.6f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

void MidiForgeLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                                            float sliderPos, float minSliderPos, float maxSliderPos,
                                            const juce::Slider::SliderStyle style, juce::Slider& slider) {
  if (style != juce::Slider::LinearHorizontal && style != juce::Slider::LinearBar) {
    juce::LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos,
                                           minSliderPos, maxSliderPos, style, slider);
    return;
  }

  auto track = juce::Rectangle<float>((float)x, (float)y, (float)width, (float)height)
                   .withSizeKeepingCentre((float)width, 4.0f);
  const float radius = 2.0f;

  g.setColour(Palette::background.brighter(0.10f));
  g.fillRoundedRectangle(track, radius);

  auto fill = track.withWidth(juce::jmax(0.0f, sliderPos - track.getX()));
  g.setColour(Palette::accent);
  g.fillRoundedRectangle(fill, radius);

  // Steel thumb.
  const float cy = track.getCentreY();
  g.setColour(Palette::background);
  g.fillEllipse(sliderPos - 6.5f, cy - 6.5f, 13.0f, 13.0f);
  g.setColour(Palette::accentSoft);
  g.drawEllipse(sliderPos - 6.5f, cy - 6.5f, 13.0f, 13.0f, 2.0f);
  g.setColour(Palette::accent);
  g.fillEllipse(sliderPos - 3.0f, cy - 3.0f, 6.0f, 6.0f);
}

void MidiForgeLookAndFeel::drawScrollbar(juce::Graphics& g, juce::ScrollBar&, int x, int y,
                                         int width, int height, bool isScrollbarVertical,
                                         int thumbStartPosition, int thumbSize,
                                         bool isMouseOver, bool isMouseDown) {
  g.setColour(Palette::background.withAlpha(0.35f));
  g.fillRoundedRectangle((float)x, (float)y, (float)width, (float)height, 3.0f);

  if (thumbSize <= 0) return;

  auto thumb = isScrollbarVertical
      ? juce::Rectangle<float>((float)x + 1.5f, (float)thumbStartPosition,
                               (float)width - 3.0f, (float)thumbSize)
      : juce::Rectangle<float>((float)thumbStartPosition, (float)y + 1.5f,
                               (float)thumbSize, (float)height - 3.0f);

  g.setColour(isMouseDown ? Palette::accent
                          : (isMouseOver ? Palette::accentDim.brighter(0.25f)
                                         : Palette::border.brighter(0.15f)));
  g.fillRoundedRectangle(thumb, 3.0f);
}

void MidiForgeLookAndFeel::fillTextEditorBackground(juce::Graphics& g, int width, int height,
                                                    juce::TextEditor& ed) {
  auto bounds = juce::Rectangle<float>(0.0f, 0.0f, (float)width, (float)height).reduced(0.5f);
  g.setColour(ed.findColour(juce::TextEditor::backgroundColourId));
  g.fillRoundedRectangle(bounds, 6.0f);
}

void MidiForgeLookAndFeel::drawTextEditorOutline(juce::Graphics& g, int width, int height,
                                                 juce::TextEditor& ed) {
  auto bounds = juce::Rectangle<float>(0.0f, 0.0f, (float)width, (float)height).reduced(0.5f);
  auto c = ed.findColour(ed.hasKeyboardFocus(false)
                             ? juce::TextEditor::focusedOutlineColourId
                             : juce::TextEditor::outlineColourId);
  if (c.isTransparent()) c = Palette::border;
  g.setColour(c);
  g.drawRoundedRectangle(bounds, 6.0f, 1.0f);
}

} // namespace MidiForge
