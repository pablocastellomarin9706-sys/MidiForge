#include "HeaderBar.h"
#include "../LookAndFeel.h"

namespace MidiForge {

static const char* keyNames[] = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
static const char* scaleNames[] = {"Major","Minor","Dorian","Mixolydian","Pent Maj","Pent Min","Blues"};
static const char* styleNames[] = {"Pop","LoFi","House","HipHop","EDM","Rock"};

HeaderBar::HeaderBar(MidiForgeAudioProcessor& p) : processor(p) {
  // The logo is painted by hand (badge + two-tone wordmark); this label only
  // reserves the layout bounds.
  titleLabel.setText("MidiForge", juce::dontSendNotification);
  titleLabel.setVisible(false);

  startTimerHz(20);

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

  chatButton.setClickingTogglesState(true);
  chatButton.setToggleState(true, juce::dontSendNotification);
  chatButton.setColour(juce::TextButton::buttonOnColourId, Palette::accent.darker(0.15f));
  chatButton.onClick = [this] { if (onChatToggle) onChatToggle(); };
  addAndMakeVisible(chatButton);

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
  auto r = getLocalBounds().toFloat();

  // Steel panel gradient.
  juce::ColourGradient bg(Palette::panel.brighter(0.04f), r.getX(), r.getY(),
                          Palette::panel.darker(0.18f), r.getX(), r.getBottom(), false);
  g.setGradientFill(bg);
  g.fillRect(r);

  // Slow cold sheen sweeping across the metal.
  const float sheenX = -240.0f + (float)sheenPhase * (r.getWidth() + 480.0f);
  {
    juce::ColourGradient left(juce::Colours::white.withAlpha(0.0f), sheenX - 160.0f, 0.0f,
                              juce::Colours::white.withAlpha(0.035f), sheenX, 0.0f, false);
    g.setGradientFill(left);
    g.fillRect(0.0f, 0.0f, juce::jmax(0.0f, sheenX), r.getHeight());

    juce::ColourGradient right(juce::Colours::white.withAlpha(0.035f), sheenX, 0.0f,
                               juce::Colours::white.withAlpha(0.0f), sheenX + 160.0f, 0.0f, false);
    g.setGradientFill(right);
    g.fillRect(juce::jmax(0.0f, sheenX), 0.0f,
               juce::jmax(0.0f, r.getWidth() - sheenX), r.getHeight());
  }

  // Bottom edge: steel light fading out from the centre.
  {
    juce::ColourGradient l(juce::Colours::transparentBlack, r.getX(), 0.0f,
                           Palette::accent.withAlpha(0.85f), r.getCentreX(), 0.0f, false);
    g.setGradientFill(l);
    g.fillRect(r.getX(), r.getBottom() - 1.0f, r.getWidth() * 0.5f, 1.0f);

    juce::ColourGradient r2(Palette::accent.withAlpha(0.85f), r.getCentreX(), 0.0f,
                            juce::Colours::transparentBlack, r.getRight(), 0.0f, false);
    g.setGradientFill(r2);
    g.fillRect(r.getCentreX(), r.getBottom() - 1.0f, r.getWidth() * 0.5f, 1.0f);
  }

  // Logo: accent badge with a spark, then the two-tone wordmark.
  auto tb = titleLabel.getBounds();
  if (tb.getWidth() > 40) {
    auto badge = juce::Rectangle<float>((float)tb.getX(), (float)tb.getCentreY() - 14.0f, 28.0f, 28.0f);
    g.setGradientFill(juce::ColourGradient(Palette::accentSoft, badge.getX(), badge.getY(),
                                           Palette::accent.darker(0.2f), badge.getRight(),
                                           badge.getBottom(), false));
    g.fillRoundedRectangle(badge, 8.0f);
    g.setColour(Palette::accentSoft.withAlpha(0.35f));
    g.drawRoundedRectangle(badge.expanded(1.0f), 9.0f, 1.0f);

    const float cx = badge.getCentreX(), cy = badge.getCentreY();
    juce::Path spark;
    spark.startNewSubPath(cx, cy - 8.5f);
    spark.lineTo(cx + 2.8f, cy - 2.8f);
    spark.lineTo(cx + 8.5f, cy);
    spark.lineTo(cx + 2.8f, cy + 2.8f);
    spark.lineTo(cx, cy + 8.5f);
    spark.lineTo(cx - 2.8f, cy + 2.8f);
    spark.lineTo(cx - 8.5f, cy);
    spark.lineTo(cx - 2.8f, cy - 2.8f);
    spark.closeSubPath();
    g.setColour(Palette::background);
    g.fillPath(spark);

    auto textArea = tb.withTrimmedLeft(38);
    auto fontBold = juce::Font(juce::FontOptions(21.0f, juce::Font::bold));
    juce::TextLayout tl;
    tl.createLayout(fontBold, 200.0f, juce::Justification::centredLeft, "Midi");
    const int midiW = (int)tl.getWidth();

    g.setFont(fontBold);
    g.setColour(Palette::textPrimary);
    g.drawText("Midi", textArea, juce::Justification::centredLeft);

    g.setFont(juce::Font(juce::FontOptions(21.0f, juce::Font::plain)));
    g.setColour(Palette::accent);
    g.drawText("Forge", textArea.withTrimmedLeft(midiW + 3), juce::Justification::centredLeft);
  }
}

void HeaderBar::timerCallback() {
  sheenPhase += 0.010;
  if (sheenPhase > 1.55) sheenPhase = -0.55;
  repaint();
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
    rr.removeFromLeft(12);
    chatButton.setBounds(rr.removeFromLeft(64).reduced(0, 4));
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
