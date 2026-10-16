#include "Sidebar.h"
#include "../LookAndFeel.h"

namespace MidiForge {

Sidebar::Sidebar(MidiForgeAudioProcessor& p) : processor(p), historyList("history", this) {
  (void)processor;
  titleLabel.setText("Ideas", juce::dontSendNotification);
  titleLabel.setFont(juce::Font(juce::FontOptions(15.0f, juce::Font::bold)));
  titleLabel.setColour(juce::Label::textColourId, Palette::textPrimary);
  addAndMakeVisible(titleLabel);

  addAndMakeVisible(historyList);
  history.add("Idea 001");
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
  historyList.setBounds(r);
}

} // namespace MidiForge
