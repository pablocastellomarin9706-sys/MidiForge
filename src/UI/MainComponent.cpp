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

  // Steel gradient base.
  juce::ColourGradient base(Palette::background.brighter(0.03f), r.getX(), r.getY(),
                            Palette::background.darker(0.25f), r.getX(), r.getBottom(), false);
  g.setGradientFill(base);
  g.fillRect(r);

  // Brushed metal diagonal lines (very subtle).
  g.setColour(juce::Colours::white.withAlpha(0.012f));
  for (float x = -r.getHeight(); x < r.getWidth(); x += 11.0f)
    g.drawLine(x, r.getBottom(), x + r.getHeight(), r.getY(), 1.0f);

  // Cold glow behind the header.
  juce::ColourGradient glow(Palette::accent.withAlpha(0.10f), r.getWidth() * 0.35f, -60.0f,
                            Palette::accent.withAlpha(0.0f), r.getWidth() * 0.35f,
                            r.getHeight() * 0.55f, false);
  g.setGradientFill(glow);
  g.fillRect(r);

  // Vignette towards the corners.
  juce::ColourGradient vig(juce::Colours::transparentBlack, r.getCentreX(), r.getCentreY(),
                           juce::Colours::black.withAlpha(0.28f), r.getX(), r.getBottom(), true);
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
