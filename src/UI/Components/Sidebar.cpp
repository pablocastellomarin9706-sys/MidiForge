#include "Sidebar.h"
#include "../LookAndFeel.h"
#include "../../Content/Characters.h"

namespace MidiForge {

Sidebar::Sidebar(MidiForgeAudioProcessor& p) : processor(p), historyList("history", this) {
  titleLabel.setText("Ideas", juce::dontSendNotification);
  titleLabel.setFont(juce::Font(juce::FontOptions(15.0f, juce::Font::bold)));
  titleLabel.setColour(juce::Label::textColourId, Palette::textPrimary);
  addAndMakeVisible(titleLabel);

  charLabel.setText("Caracter", juce::dontSendNotification);
  charLabel.setFont(juce::Font(juce::FontOptions(11.0f)));
  charLabel.setColour(juce::Label::textColourId, Palette::textSecondary);
  addAndMakeVisible(charLabel);

  charBox.addItem("None", 1);
  charBox.addItem("Detroit", 2);
  charBox.addItem("Trap Oscuro", 3);
  charBox.addItem("Plugg", 4);
  charBox.addItem("Nu-Jerk", 5);
  charBox.addItem("Boom Bap", 6);
  charBox.addItem("Rage", 7);
  charBox.addItem("Trap Latino", 8);
  charBox.setSelectedId(1, juce::dontSendNotification);
  charBox.onChange = [this] {
    Character ch = Character::None;
    switch (charBox.getSelectedId()) {
      case 2: ch = Character::Detroit; break;
      case 3: ch = Character::TrapDark; break;
      case 4: ch = Character::Plugg; break;
      case 5: ch = Character::NuJerk; break;
      case 6: ch = Character::BoomBap; break;
      case 7: ch = Character::Rage; break;
      case 8: ch = Character::TrapLatino; break;
      default: ch = Character::None; break;
    }
    auto& idea = processor.getCurrentIdea();
    idea.character = ch;
    if (ch != Character::None) {
      idea.style = Style::HipHop;
      const auto cp = Content::characterPresetOf(ch);
      if (cp.valid) idea.swing = cp.swing;
      if (idea.scale == ScaleType::Major)
        idea.scale = Content::defaultScaleFor(ch);
    }
    processor.generateFull();
    if (onIdeaChanged) onIdeaChanged();
  };
  addAndMakeVisible(charBox);

  addAndMakeVisible(historyList);
  history.add("Idea 001");
}

void Sidebar::refreshFromIdea() {
  int id = 1;
  switch (processor.getCurrentIdea().character) {
    case Character::Detroit: id = 2; break;
    case Character::TrapDark: id = 3; break;
    case Character::Plugg: id = 4; break;
    case Character::NuJerk: id = 5; break;
    case Character::BoomBap: id = 6; break;
    case Character::Rage: id = 7; break;
    case Character::TrapLatino: id = 8; break;
    case Character::None: id = 1; break;
  }
  if (charBox.getSelectedId() != id)
    charBox.setSelectedId(id, juce::dontSendNotification);
}

int Sidebar::getNumRows() { return history.size(); }

void Sidebar::paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool selected) {
  auto r = juce::Rectangle<float>(0.0f, 0.0f, (float)width, (float)height).reduced(2.0f, 1.0f);
  if(selected) {
    g.setColour(Palette::accent.withAlpha(0.16f));
    g.fillRoundedRectangle(r, 5.0f);
    g.setColour(Palette::accent);
    g.fillRoundedRectangle(r.withWidth(3.0f).reduced(0.0f, 3.0f), 1.5f);
  }
  g.setColour(selected ? Palette::textPrimary : Palette::textSecondary);
  g.setFont(juce::Font(juce::FontOptions(12.5f)));
  g.drawText(history[row], 14, 0, width - 16, height, juce::Justification::centredLeft);
}

void Sidebar::paint(juce::Graphics& g) {
  auto r = getLocalBounds().toFloat();

  juce::ColourGradient bg(Palette::panelAlt.brighter(0.03f), r.getX(), r.getY(),
                          Palette::panelAlt.darker(0.18f), r.getX(), r.getBottom(), false);
  g.setGradientFill(bg);
  g.fillRect(r);

  // Right edge: steel light fading from the middle.
  juce::ColourGradient top(Palette::accent.withAlpha(0.0f), r.getRight() - 1.0f, r.getY(),
                           Palette::accent.withAlpha(0.45f), r.getRight() - 1.0f, r.getCentreY(), false);
  g.setGradientFill(top);
  g.fillRect(r.getRight() - 1.0f, r.getY(), 1.0f, r.getHeight() * 0.5f);

  juce::ColourGradient bottom(Palette::accent.withAlpha(0.45f), r.getRight() - 1.0f, r.getCentreY(),
                              Palette::accent.withAlpha(0.0f), r.getRight() - 1.0f, r.getBottom(), false);
  g.setGradientFill(bottom);
  g.fillRect(r.getRight() - 1.0f, r.getCentreY(), 1.0f, r.getHeight() * 0.5f);
}

void Sidebar::resized() {
  auto r = getLocalBounds().reduced(10, 10);
  titleLabel.setBounds(r.removeFromTop(26));
  r.removeFromTop(6);
  charLabel.setBounds(r.removeFromTop(14));
  charBox.setBounds(r.removeFromTop(24));
  r.removeFromTop(8);
  historyList.setBounds(r);
}

} // namespace MidiForge
