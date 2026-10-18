#include "HeaderBar.h"
#include "../LookAndFeel.h"

namespace MidiForge {

static const char* keyNames[] = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
static const char* scaleNames[] = {"Major","Minor","Dorian","Mixolydian","Pent Maj","Pent Min","Blues"};
static const char* styleNames[] = {"Pop","LoFi","House","HipHop","EDM","Rock"};

HeaderBar::HeaderBar(MidiForgeAudioProcessor& p) : processor(p) {
  titleLabel.setText("MidiForge", juce::dontSendNotification);
  titleLabel.setFont(juce::Font(juce::FontOptions(22.0f, juce::Font::bold)));
  titleLabel.setColour(juce::Label::textColourId, Palette::accent);
  addAndMakeVisible(titleLabel);

  auto reroll = [this](bool newSeed) {
    auto& idea = processor.getCurrentIdea();
    if (newSeed) {
      idea.seed = juce::Random::getSystemRandom().nextInt(1000000) + 1;
      idea.take = 0;
    } else {
      idea.take += 1;
    }
    processor.generateFull();
    refreshFromIdea();
  };
  generateButton.setColour(juce::TextButton::buttonColourId, Palette::accent);
  generateButton.onClick = [reroll] { reroll(true); };
  addAndMakeVisible(generateButton);
  shuffleButton.onClick = [reroll] { reroll(false); };
  addAndMakeVisible(shuffleButton);

  auto setupLabel = [this](juce::Label& l, const juce::String& text) {
    l.setText(text, juce::dontSendNotification);
    l.setColour(juce::Label::textColourId, Palette::textSecondary);
    l.setFont(juce::Font(juce::FontOptions(11.0f)));
    addAndMakeVisible(l);
  };
  setupLabel(keyLabel, "Key");
  setupLabel(scaleLabel, "Scale");
  setupLabel(styleLabel, "Style");
  setupLabel(barsLabel, "Bars");
  setupLabel(seedLabel, "Seed");
  setupLabel(swingLabel, "Swing");
  setupLabel(brightLabel, "Juice");

  for (int i = 0; i < 12; ++i) keyBox.addItem(keyNames[i], i + 1);
  keyBox.onChange = [this] {
    processor.getCurrentIdea().keyRoot = keyBox.getSelectedId() - 1;
    processor.generateFull();
    refreshFromIdea();
  };
  addAndMakeVisible(keyBox);

  for (int i = 0; i < 7; ++i) scaleBox.addItem(scaleNames[i], i + 1);
  scaleBox.onChange = [this] {
    static const ScaleType types[] = {ScaleType::Major, ScaleType::MinorNatural, ScaleType::Dorian,
                                      ScaleType::Mixolydian, ScaleType::PentatonicMajor,
                                      ScaleType::PentatonicMinor, ScaleType::Blues};
    processor.getCurrentIdea().scale = types[scaleBox.getSelectedId() - 1];
    processor.generateFull();
    refreshFromIdea();
  };
  addAndMakeVisible(scaleBox);

  for (int i = 0; i < 6; ++i) styleBox.addItem(styleNames[i], i + 1);
  styleBox.onChange = [this] {
    processor.getCurrentIdea().style = (Style)(styleBox.getSelectedId() - 1);
    processor.generateFull();
    refreshFromIdea();
  };
  addAndMakeVisible(styleBox);

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
    auto& idea = processor.getCurrentIdea();
    idea.seed = (int)seedSlider.getValue();
    idea.take = 0;
    processor.generateFull();
    refreshFromIdea();
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

  brightnessSlider.setRange(0, 100, 1);
  brightnessSlider.setValue(60, juce::dontSendNotification);
  brightnessSlider.setSliderStyle(juce::Slider::LinearHorizontal);
  brightnessSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
  brightnessSlider.onValueChange = [this] {
    processor.getCurrentIdea().brightness = brightnessSlider.getValue() / 100.0;
    processor.generateFull();
    refreshFromIdea();
  };
  addAndMakeVisible(brightnessSlider);

  refreshFromIdea();
}

void HeaderBar::refreshFromIdea() {
  auto& idea = processor.getCurrentIdea();
  keyBox.setSelectedId(idea.keyRoot + 1, juce::dontSendNotification);

  static const ScaleType types[] = {ScaleType::Major, ScaleType::MinorNatural, ScaleType::Dorian,
                                    ScaleType::Mixolydian, ScaleType::PentatonicMajor,
                                    ScaleType::PentatonicMinor, ScaleType::Blues};
  for (int i = 0; i < 7; ++i) {
    if (types[i] == idea.scale) { scaleBox.setSelectedId(i + 1, juce::dontSendNotification); break; }
  }

  styleBox.setSelectedId((int)idea.style + 1, juce::dontSendNotification);
  barsSlider.setValue(idea.bars, juce::dontSendNotification);
  seedSlider.setValue(idea.seed, juce::dontSendNotification);
  swingSlider.setValue(idea.swing, juce::dontSendNotification);
  brightnessSlider.setValue(idea.brightness * 100.0, juce::dontSendNotification);
  if (onIdeaChanged) onIdeaChanged();
}

void HeaderBar::paint(juce::Graphics& g) {
  g.fillAll(Palette::panel);
  g.setColour(Palette::border);
  g.drawHorizontalLine(getHeight() - 1, 0.0f, (float)getWidth());
}

void HeaderBar::resized() {
  auto r = getLocalBounds().reduced(10, 8);
  auto row1 = r.removeFromTop(36);
  auto row2 = r;

  {
    auto rr = row1;
    titleLabel.setBounds(rr.removeFromLeft(140));
    rr.removeFromLeft(12);
    generateButton.setBounds(rr.removeFromLeft(110).reduced(0, 4));
    rr.removeFromLeft(8);
    shuffleButton.setBounds(rr.removeFromLeft(80).reduced(0, 4));
    rr.removeFromLeft(16);
    seedLabel.setBounds(rr.removeFromLeft(34).removeFromBottom(16));
    seedSlider.setBounds(rr.removeFromLeft(120).withSizeKeepingCentre(120, 24));
  }
  {
    auto rr = row2;
    auto place = [&rr](juce::Label& l, juce::Component& c, int labelW, int compW) {
      auto lr = rr.removeFromLeft(labelW);
      l.setBounds(lr.removeFromBottom(16));
      c.setBounds(rr.removeFromLeft(compW).withSizeKeepingCentre(compW, 24));
      rr.removeFromLeft(8);
    };
    place(keyLabel, keyBox, 26, 54);
    place(scaleLabel, scaleBox, 34, 100);
    place(styleLabel, styleBox, 30, 80);
    place(barsLabel, barsSlider, 28, 80);
    place(swingLabel, swingSlider, 34, 80);
    place(brightLabel, brightnessSlider, 30, 80);
  }
}

} // namespace MidiForge
