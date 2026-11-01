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

  // The animated anvil badge doubles as the main "forge" button.
  mascot.onClick = [this] { forgeNewIdea(true); };
  addAndMakeVisible(mascot);

  generateButton.setColour(juce::TextButton::buttonColourId, Palette::accent);
  generateButton.setTooltip("Forja una idea nueva (semilla nueva)");
  generateButton.onClick = [this] { forgeNewIdea(true); };
  addAndMakeVisible(generateButton);
  shuffleButton.setTooltip("Otra variacion de la misma idea (misma semilla)");
  shuffleButton.onClick = [this] { forgeNewIdea(false); };
  addAndMakeVisible(shuffleButton);

  chatButton.setClickingTogglesState(true);
  chatButton.setToggleState(true, juce::dontSendNotification);
  chatButton.setColour(juce::TextButton::buttonOnColourId, Palette::accent.darker(0.15f));
  chatButton.setTooltip("Mostrar u ocultar el Forge Chat");
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
  setupLabel(variationLabel, "Var");

  for (int i = 0; i < 12; ++i) keyBox.addItem(keyNames[i], i + 1);
  keyBox.onChange = [this] {
    processor.getCurrentIdea().keyRoot = keyBox.getSelectedId() - 1;
    processor.generateFull();
    refreshFromIdea();
  };
  addAndMakeVisible(keyBox);
  keyBox.setTooltip("Tonalidad de la idea");

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
  scaleBox.setTooltip("Escala o modo: la melodia nunca se sale de aqui");

  for (int i = 0; i < 6; ++i) styleBox.addItem(styleNames[i], i + 1);
  styleBox.onChange = [this] {
    processor.getCurrentIdea().style = (Style)(styleBox.getSelectedId() - 1);
    processor.generateFull();
    refreshFromIdea();
  };
  addAndMakeVisible(styleBox);
  styleBox.setTooltip("Genero: cambia grooves de bateria, lineas de bajo y motivos de melodia");

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
  barsSlider.setTooltip("Compases de la idea (1-16)");

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
  seedSlider.setTooltip("Semilla: la misma semilla reproduce la misma idea");

  swingSlider.setRange(0, 100, 1);
  swingSlider.setValue(0, juce::dontSendNotification);
  swingSlider.setSliderStyle(juce::Slider::LinearHorizontal);
  swingSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
  swingSlider.setTooltip("Swing del groove: se aplica al soltar el slider");
  swingSlider.onValueChange = [this] {
    processor.getCurrentIdea().swing = swingSlider.getValue();
  };
  // Regenerating on every pixel of drag would rebuild the idea dozens of times;
  // doing it on release means the swing change is actually heard (it used to
  // sit there until the next Shuffle, which read as "this slider does nothing").
  swingSlider.onDragEnd = [this] {
    processor.generateFull();
    refreshFromIdea();
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
  brightnessSlider.setTooltip("Juice: voicings mas ricos (novenas) y notas con mas pegada");

  variationSlider.setRange(0, 100, 1);
  variationSlider.setValue(0, juce::dontSendNotification);
  variationSlider.setSliderStyle(juce::Slider::LinearHorizontal);
  variationSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
  variationSlider.onValueChange = [this] {
    processor.getCurrentIdea().creativity = (float)(variationSlider.getValue() / 100.0);
    processor.generateFull();
    refreshFromIdea();
  };
  addAndMakeVisible(variationSlider);
  variationSlider.setTooltip("Variacion: humanizacion extra (0 = cuadriculado)");

  refreshFromIdea();
}

void HeaderBar::forgeNewIdea(bool newSeed) {
  auto& idea = processor.getCurrentIdea();
  if (newSeed) {
    idea.seed = juce::Random::getSystemRandom().nextInt(1000000) + 1;
    idea.take = 0;
  } else {
    idea.take += 1;
  }
  processor.generateFull();
  mascot.strike(); // the anvil reacts to every new idea, however it was asked
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
  variationSlider.setValue(idea.creativity * 100.0, juce::dontSendNotification);
  if (onIdeaChanged) onIdeaChanged();
}

void HeaderBar::paint(juce::Graphics& g) {
  auto r = getLocalBounds().toFloat();

  // Warm workshop panel.
  juce::ColourGradient bg(Palette::panel.brighter(0.05f), r.getX(), r.getY(),
                          Palette::panel.darker(0.30f), r.getX(), r.getBottom(), false);
  g.setGradientFill(bg);
  g.fillRect(r);

  // Heat bleeding up from the molten bottom edge.
  juce::ColourGradient heat(Palette::ember.withAlpha(0.0f), r.getCentreX(), r.getY(),
                            Palette::accent.withAlpha(0.14f), r.getCentreX(), r.getBottom(), false);
  g.setGradientFill(heat);
  g.fillRect(r);

  // Slow warm sheen sweeping across the metal.
  const float sheenX = -240.0f + (float)sheenPhase * (r.getWidth() + 480.0f);
  {
    juce::ColourGradient left(juce::Colours::white.withAlpha(0.0f), sheenX - 160.0f, 0.0f,
                              juce::Colours::white.withAlpha(0.040f), sheenX, 0.0f, false);
    g.setGradientFill(left);
    g.fillRect(0.0f, 0.0f, juce::jmax(0.0f, sheenX), r.getHeight());

    juce::ColourGradient right(juce::Colours::white.withAlpha(0.040f), sheenX, 0.0f,
                               juce::Colours::white.withAlpha(0.0f), sheenX + 160.0f, 0.0f, false);
    g.setGradientFill(right);
    g.fillRect(juce::jmax(0.0f, sheenX), 0.0f,
               juce::jmax(0.0f, r.getWidth() - sheenX), r.getHeight());
  }

  // Molten bottom edge: orange turning to hot magenta, fading at both ends.
  {
    juce::ColourGradient edge(juce::Colours::transparentBlack, r.getX(), 0.0f,
                              juce::Colours::transparentBlack, r.getRight(), 0.0f, false);
    edge.addColour(0.34, Palette::accent.withAlpha(0.00f));
    edge.addColour(0.50, Palette::accent.withAlpha(0.95f));
    edge.addColour(0.68, Palette::hot.withAlpha(0.90f));
    edge.addColour(0.84, Palette::ember.withAlpha(0.00f));
    g.setGradientFill(edge);
    g.fillRect(r.getX(), r.getBottom() - 1.5f, r.getWidth(), 1.5f);
  }

  // Wordmark. The molten badge with the anvil is the ForgeMascot child, which
  // animates (hammer + sparks) and forges a new idea when clicked.
  auto tb = titleLabel.getBounds();
  if (tb.getWidth() > 40) {
    auto textArea = tb.withTrimmedLeft(44);
    juce::AttributedString as;
    as.append("Midi", juce::Font(juce::FontOptions(21.0f, juce::Font::bold)), Palette::textPrimary);
    as.append("Forge", juce::Font(juce::FontOptions(21.0f, juce::Font::bold)), Palette::accent);
    as.setJustification(juce::Justification::centredLeft);

    juce::TextLayout tl;
    tl.createLayout(as, (float)textArea.getWidth());
    const float ty = (float)textArea.getY() + ((float)textArea.getHeight() - tl.getHeight()) * 0.5f;
    tl.draw(g, juce::Rectangle<float>((float)textArea.getX(), ty,
                                      (float)textArea.getWidth(), tl.getHeight()));
  }
}

void HeaderBar::timerCallback() {
  sheenPhase += 0.012;
  if (sheenPhase > 1.55) sheenPhase = -0.55;
  repaint();
}

void HeaderBar::resized() {
  auto r = getLocalBounds().reduced(10, 8);
  auto row1 = r.removeFromTop(36);
  auto row2 = r;

  {
    auto rr = row1;
    auto titleArea = rr.removeFromLeft(150);
    titleLabel.setBounds(titleArea);
    // Animated badge with a 3 px glow margin around the 34 px anvil.
    mascot.setBounds(titleArea.getX() - 3, titleArea.getCentreY() - 20, 40, 40);
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
    place(variationLabel, variationSlider, 26, 70);
  }
}

} // namespace MidiForge
