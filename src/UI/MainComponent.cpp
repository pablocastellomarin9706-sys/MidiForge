#include "MainComponent.h"

namespace MidiForge {

MainComponent::MainComponent(MidiForgeAudioProcessor& p) : processor(p) {
  header = std::make_unique<HeaderBar>(processor);
  sidebar = std::make_unique<Sidebar>(processor);
  tabsView = std::make_unique<TabView>(processor);
  chat = std::make_unique<ChatPanel>(processor);

  addAndMakeVisible(*header);
  addAndMakeVisible(*sidebar);
  addAndMakeVisible(*tabsView);
  addAndMakeVisible(*chat);

  header->onIdeaChanged = [this] {
    if(tabsView) tabsView->refreshAll();
    if(sidebar) sidebar->refreshFromIdea();
  };
  header->onChatToggle = [this] {
    chatVisible = !chatVisible;
    chat->setVisible(chatVisible);
    resized();
  };
  sidebar->onIdeaChanged = [this] {
    header->refreshFromIdea();
    tabsView->refreshAll();
  };
  chat->onIdeaChanged = [this] {
    header->refreshFromIdea();
    tabsView->refreshAll();
    sidebar->refreshFromIdea();
  };

  // Generate an initial idea so the UI shows something on open.
  processor.getCurrentIdea().seed = 12345;
  processor.generateFull();
  tabsView->refreshAll();
}

void MainComponent::paint(juce::Graphics& g) {
  auto r = getLocalBounds().toFloat();

  // Warm near-black base.
  juce::ColourGradient base(Palette::background.brighter(0.05f), r.getX(), r.getY(),
                            Palette::background.darker(0.35f), r.getX(), r.getBottom(), false);
  g.setGradientFill(base);
  g.fillRect(r);

  // Faint workshop grid.
  const float cell = 28.0f;
  g.setColour(juce::Colours::white.withAlpha(0.012f));
  for (float x = 0.0f; x < r.getWidth(); x += cell) g.drawVerticalLine((int)x, 0.0f, r.getHeight());
  for (float y = 0.0f; y < r.getHeight(); y += cell) g.drawHorizontalLine((int)y, 0.0f, r.getWidth());

  // Brushed-metal diagonal lines (very subtle).
  g.setColour(juce::Colours::white.withAlpha(0.010f));
  for (float x = -r.getHeight(); x < r.getWidth(); x += 13.0f)
    g.drawLine(x, r.getBottom(), x + r.getHeight(), r.getY(), 1.0f);

  // Molten glow pouring from the forge mouth (top-left).
  {
    juce::ColourGradient glow(Palette::accent.withAlpha(0.16f), r.getWidth() * 0.22f, -30.0f,
                              Palette::accent.withAlpha(0.0f), r.getWidth() * 0.22f,
                              r.getHeight() * 0.68f, true);
    g.setGradientFill(glow);
    g.fillRect(r);
  }
  // Cool cyan counter-glow bottom-right, for hot/cold contrast.
  {
    juce::ColourGradient glow(Palette::cool.withAlpha(0.10f), r.getRight(), r.getBottom(),
                              Palette::cool.withAlpha(0.0f), r.getWidth() * 0.45f,
                              r.getHeight() * 0.35f, true);
    g.setGradientFill(glow);
    g.fillRect(r);
  }

  // Drifting embers (deterministic, drawn once).
  unsigned int s = 0x9e3779b9u;
  auto rnd = [&s]() { s = s * 1664525u + 1013904223u; return (float)((s >> 8) & 0xffffu) / 65535.0f; };
  for (int i = 0; i < 26; ++i) {
    const float x = rnd() * r.getWidth();
    const float y = r.getHeight() * (0.35f + 0.65f * rnd());
    const float rad = 0.8f + 1.8f * rnd();
    const float a = 0.06f + 0.16f * rnd();
    g.setColour((i % 5 == 0 ? Palette::hot : Palette::accent).withAlpha(a));
    g.fillEllipse(x, y, rad, rad);
  }

  // Vignette towards the corners.
  juce::ColourGradient vig(juce::Colours::transparentBlack, r.getCentreX(), r.getCentreY(),
                           juce::Colours::black.withAlpha(0.34f), r.getX(), r.getBottom(), true);
  g.setGradientFill(vig);
  g.fillRect(r);
}

void MainComponent::resized() {
  auto r = getLocalBounds();
  header->setBounds(r.removeFromTop(96));
  sidebar->setBounds(r.removeFromLeft(200));
  if (chatVisible && chat != nullptr)
    chat->setBounds(r.removeFromRight(320));
  tabsView->setBounds(r);
}

} // namespace MidiForge
