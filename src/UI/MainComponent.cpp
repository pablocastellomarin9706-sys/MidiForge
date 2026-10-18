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
  };
  header->onChatToggle = [this] {
    chatVisible = !chatVisible;
    chat->setVisible(chatVisible);
    resized();
  };
  chat->onIdeaChanged = [this] {
    header->refreshFromIdea();
    tabsView->refreshAll();
  };

  // Generate an initial idea so the UI shows something on open.
  processor.getCurrentIdea().seed = 12345;
  processor.generateFull();
  tabsView->refreshAll();
}

void MainComponent::paint(juce::Graphics& g) {
  g.fillAll(Palette::background);
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
