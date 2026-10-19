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
  setColour(juce::ComboBox::arrowColourId, Palette::accent);

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
  const float cut = juce::jmin(8.0f, bounds.getHeight() * 0.34f);
  const bool primary = button.getToggleState() || backgroundColour == Palette::accent;

  juce::Colour base = primary ? Palette::accent : backgroundColour;
  if (shouldDrawButtonAsDown) base = base.darker(0.28f);
  else if (shouldDrawButtonAsHighlighted) base = base.brighter(primary ? 0.10f : 0.20f);

  const juce::Colour top = primary ? base.brighter(0.12f) : base.brighter(0.10f);
  const juce::Colour bot = primary ? Palette::ember.darker(0.12f) : base.darker(0.24f);
  auto path = chamferRect(bounds, cut);

  // Molten halo around the primary action.
  if (primary) {
    g.setColour(Palette::accent.withAlpha(shouldDrawButtonAsHighlighted ? 0.45f : 0.26f));
    g.strokePath(chamferRect(bounds.expanded(1.6f), cut + 1.6f), juce::PathStrokeType(2.0f));
  }

  g.setGradientFill(juce::ColourGradient(top, bounds.getX(), bounds.getY(),
                                         bot, bounds.getX(), bounds.getBottom(), false));
  g.fillPath(path);

  // Hot top edge (light hitting the anvil).
  g.setColour(juce::Colours::white.withAlpha(primary ? 0.30f : 0.07f));
  g.drawHorizontalLine((int)bounds.getY() + 1, bounds.getX() + cut, bounds.getRight() - cut);

  // Border + hover glow.
  if (shouldDrawButtonAsHighlighted && !primary) g.setColour(Palette::accent.withAlpha(0.9f));
  else if (primary) g.setColour(Palette::hot.withAlpha(0.55f));
  else g.setColour(Palette::border);
  g.strokePath(path, juce::PathStrokeType(1.0f));
}

void MidiForgeLookAndFeel::drawTabButton(juce::TabBarButton& button, juce::Graphics& g,
                                         bool isMouseOver, bool isMouseDown) {
  auto bounds = button.getLocalBounds().toFloat();
  const bool active = button.getToggleState();
  auto box = bounds.reduced(3.0f, 5.0f);
  auto path = chamferRect(box, 6.0f);

  if (active) {
    g.setGradientFill(juce::ColourGradient(Palette::cardHover, box.getX(), box.getY(),
                                           Palette::card, box.getX(), box.getBottom(), false));
    g.fillPath(path);
    g.setColour(Palette::accent.withAlpha(0.40f));
    g.strokePath(path, juce::PathStrokeType(1.0f));

    auto ul = bounds.removeFromBottom(3.0f).reduced(7.0f, 0.0f);
    g.setColour(Palette::accent.withAlpha(0.30f));
    g.fillRoundedRectangle(ul.expanded(1.6f, 1.2f), 2.0f);
    g.setGradientFill(juce::ColourGradient(Palette::accent, ul.getX(), 0.0f,
                                           Palette::hot, ul.getRight(), 0.0f, false));
    g.fillRoundedRectangle(ul, 1.5f);
  } else if (isMouseOver || isMouseDown) {
    g.setColour(Palette::card.withAlpha(0.65f));
    g.fillPath(path);
  }

  g.setColour(active ? Palette::textPrimary : Palette::textSecondary);
  g.setFont(juce::Font(juce::FontOptions(12.0f, active ? juce::Font::bold : juce::Font::plain)));
  g.drawText(button.getButtonText(), button.getLocalBounds(), juce::Justification::centred);
}

void MidiForgeLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                                        int, int, int, int, juce::ComboBox& box) {
  auto bounds = juce::Rectangle<float>(0.0f, 0.0f, (float)width, (float)height).reduced(0.5f);
  const float cut = juce::jmin(7.0f, bounds.getHeight() * 0.32f);
  auto path = chamferRect(bounds, cut);

  g.setColour(isButtonDown ? Palette::cardHover : Palette::card);
  g.fillPath(path);

  g.setColour(box.hasKeyboardFocus(false) ? Palette::accent.withAlpha(0.9f) : Palette::border);
  g.strokePath(path, juce::PathStrokeType(1.0f));

  // Molten chevron.
  juce::Path p;
  const float cx = bounds.getRight() - 12.0f;
  const float cy = bounds.getCentreY();
  p.startNewSubPath(cx - 4.0f, cy - 2.0f);
  p.lineTo(cx, cy + 3.0f);
  p.lineTo(cx + 4.0f, cy - 2.0f);
  g.setColour(Palette::accent);
  g.strokePath(p, juce::PathStrokeType(1.7f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
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
                   .withSizeKeepingCentre((float)width, 5.0f);
  const float radius = 2.5f;

  g.setColour(Palette::background.brighter(0.12f));
  g.fillRoundedRectangle(track, radius);

  auto fill = track.withWidth(juce::jmax(0.0f, sliderPos - track.getX()));
  if (fill.getWidth() > 0.5f) {
    g.setGradientFill(juce::ColourGradient(Palette::ember, fill.getX(), 0.0f,
                                           Palette::accentSoft, fill.getRight(), 0.0f, false));
    g.fillRoundedRectangle(fill, radius);
  }

  // Ember thumb with a soft halo.
  const float cy = track.getCentreY();
  g.setColour(Palette::accent.withAlpha(0.30f));
  g.fillEllipse(sliderPos - 9.5f, cy - 9.5f, 19.0f, 19.0f);
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
                          : (isMouseOver ? Palette::ember.brighter(0.15f)
                                         : Palette::border.brighter(0.15f)));
  g.fillRoundedRectangle(thumb, 3.0f);
}

void MidiForgeLookAndFeel::fillTextEditorBackground(juce::Graphics& g, int width, int height,
                                                    juce::TextEditor& ed) {
  auto bounds = juce::Rectangle<float>(0.0f, 0.0f, (float)width, (float)height).reduced(0.5f);
  g.setColour(ed.findColour(juce::TextEditor::backgroundColourId));
  g.fillPath(chamferRect(bounds, 7.0f));
}

void MidiForgeLookAndFeel::drawTextEditorOutline(juce::Graphics& g, int width, int height,
                                                 juce::TextEditor& ed) {
  auto bounds = juce::Rectangle<float>(0.0f, 0.0f, (float)width, (float)height).reduced(0.5f);
  auto c = ed.findColour(ed.hasKeyboardFocus(false)
                             ? juce::TextEditor::focusedOutlineColourId
                             : juce::TextEditor::outlineColourId);
  if (c.isTransparent()) c = Palette::border;
  g.setColour(c);
  g.strokePath(chamferRect(bounds, 7.0f), juce::PathStrokeType(1.0f));
}

} // namespace MidiForge
