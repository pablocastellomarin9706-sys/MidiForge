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
      if (ch == Character::TrapLatino && idea.scale == ScaleType::Major)
        idea.scale = ScaleType::MinorHarmonic;
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
  if(selected) { g.setColour(Palette::accent.withAlpha(0.3f)); g.fillRect(0,0,width,height); }
  g.setColour(Palette::textSecondary);
  g.setFont(juce::Font(juce::FontOptions(13.0f)));
  g.drawText(history[row], 10, 0, width-10, height, juce::Justification::centredLeft);
}

void Sidebar::paint(juce::Graphics& g) {
  g.fillAll(Palette::panelAlt);
  g.setColour(Palette::border);
  g.drawVerticalLine(getWidth()-1, 0.0f, (float)getHeight());
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
