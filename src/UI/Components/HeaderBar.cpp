#include "HeaderBar.h"
#include "../LookAndFeel.h"

namespace MidiForge {

static const char* keyNames[] = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};

HeaderBar::HeaderBar(MidiForgeAudioProcessor& p) : processor(p) {
  titleLabel.setText("MidiForge", juce::dontSendNotification);
  titleLabel.setFont(juce::Font(juce::FontOptions(24.0f, juce::Font::bold)));
  titleLabel.setColour(juce::Label::textColourId, Palette::accent);
  addAndMakeVisible(titleLabel);

  generateButton.setColour(juce::TextButton::buttonColourId, Palette::accent);
  generateButton.onClick = [this] {
    processor.getCurrentIdea().seed = juce::Random::getSystemRandom().nextInt(1000000) + 1;
    processor.generateFull();
    refreshFromIdea();
  };
  addAndMakeVisible(generateButton);

  shuffleButton.onClick = [this] {
    auto& idea = processor.getCurrentIdea();
    idea.seed = juce::Random::getSystemRandom().nextInt(1000000) + 1;
    processor.generateFull();
    refreshFromIdea();
  };
  addAndMakeVisible(shuffleButton);

  auto setupLabel = [this](juce::Label& l, const juce::String& text) {
    l.setText(text, juce::dontSendNotification);
    l.setColour(juce::Label::textColourId, Palette::textSecondary);
    l.setFont(juce::Font(juce::FontOptions(11.0f)));
    addAndMakeVisible(l);
  };
  setupLabel(keyLabel, "Key");
  setupLabel(scaleLabel, "Scale");
  setupLabel(barsLabel, "Bars");
  setupLabel(seedLabel, "Seed");
  setupLabel(swingLabel, "Swing");

  for(int i=0;i<12;++i) keyBox.addItem(keyNames[i], i+1);
  keyBox.onChange = [this] {
    processor.getCurrentIdea().keyRoot = keyBox.getSelectedId() - 1;
    processor.generateFull();
    refreshFromIdea();
  };
  addAndMakeVisible(keyBox);

  const char* scaleNames[] = {"Major","Minor","Dorian","Mixolydian","Pentatonic Maj","Pentatonic Min","Blues"};
  const ScaleType scaleTypes[] = {ScaleType::Major, ScaleType::MinorNatural, ScaleType::Dorian,
                                  ScaleType::Mixolydian, ScaleType::PentatonicMajor,
                                  ScaleType::PentatonicMinor, ScaleType::Blues};
  (void)scaleTypes;
  for(int i=0;i<7;++i) scaleBox.addItem(scaleNames[i], i+1);
  scaleBox.onChange = [this] {
    static const ScaleType types[] = {ScaleType::Major, ScaleType::MinorNatural, ScaleType::Dorian,
                                      ScaleType::Mixolydian, ScaleType::PentatonicMajor,
                                      ScaleType::PentatonicMinor, ScaleType::Blues};
    processor.getCurrentIdea().scale = types[scaleBox.getSelectedId()-1];
    processor.generateFull();
    refreshFromIdea();
  };
  addAndMakeVisible(scaleBox);

  barsSlider.setRange(1, 16, 1);
  barsSlider.setValue(4, juce::dontSendNotification);
  barsSlider.setSliderStyle(juce::Slider::IncDecButtons);
  barsSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 40, 20);
  barsSlider.onValueChange = [this] {
    processor.getCurrentIdea().bars = (int)barsSlider.getValue();
    processor.generateFull();
    refreshFromIdea();
  };
  addAndMakeVisible(barsSlider);

  seedSlider.setRange(0, 999999, 1);
  seedSlider.setValue(12345, juce::dontSendNotification);
  seedSlider.setSliderStyle(juce::Slider::IncDecButtons);
  seedSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 60, 20);
  seedSlider.onValueChange = [this] {
    processor.getCurrentIdea().seed = (int)seedSlider.getValue();
  };
  addAndMakeVisible(seedSlider);

  swingSlider.setRange(0, 100, 1);
  swingSlider.setValue(0, juce::dontSendNotification);
  swingSlider.setSliderStyle(juce::Slider::LinearHorizontal);
  swingSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
  swingSlider.onValueChange = [this] {
    processor.getCurrentIdea().swing = swingSlider.getValue();
  };
  addAndMakeVisible(swingSlider);

  refreshFromIdea();
}

void HeaderBar::refreshFromIdea() {
  auto& idea = processor.getCurrentIdea();
  keyBox.setSelectedId(idea.keyRoot + 1, juce::dontSendNotification);
  seedSlider.setValue(idea.seed, juce::dontSendNotification);
  if(onIdeaChanged) onIdeaChanged();
}

void HeaderBar::paint(juce::Graphics& g) {
  g.fillAll(Palette::panel);
  g.setColour(Palette::border);
  g.drawHorizontalLine(getHeight() - 1, 0.0f, (float)getWidth());
}

void HeaderBar::resized() {
  auto r = getLocalBounds().reduced(10, 8);
  titleLabel.setBounds(r.removeFromLeft(140));
  r.removeFromLeft(12);
  generateButton.setBounds(r.removeFromLeft(110).reduced(0, 6));
  r.removeFromLeft(8);
  shuffleButton.setBounds(r.removeFromLeft(80).reduced(0, 6));
  r.removeFromLeft(16);

  auto place = [&r](juce::Label& l, juce::Component& c, int labelW, int compW) {
    auto lr = r.removeFromLeft(labelW);
    l.setBounds(lr.removeFromBottom(16));
    c.setBounds(r.removeFromLeft(compW).withSizeKeepingCentre(compW, 24));
    r.removeFromLeft(8);
  };
  place(keyLabel, keyBox, 30, 60);
  place(scaleLabel, scaleBox, 34, 110);
  place(barsLabel, barsSlider, 30, 90);
  place(seedLabel, seedSlider, 34, 120);
  place(swingLabel, swingSlider, 36, 90);
}

} // namespace MidiForge
