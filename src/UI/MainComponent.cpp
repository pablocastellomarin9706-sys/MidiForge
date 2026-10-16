#include "MainComponent.h"
#include "Components/HeaderBar.h"
#include "Components/Sidebar.h"
#include "Components/TabView.h"

namespace MidiForge {

MainComponent::MainComponent(MidiForgeAudioProcessor& p) : processor(p) {
  header = std::make_unique<HeaderBar>(processor);
  sidebar = std::make_unique<Sidebar>(processor);
  tabs = std::make_unique<TabView>(processor);
  addAndMakeVisible(*header);
  addAndMakeVisible(*sidebar);
  addAndMakeVisible(*tabs);

  auto* tabView = dynamic_cast<TabView*>(tabs.get());
  dynamic_cast<HeaderBar*>(header.get())->onIdeaChanged = [tabView] {
    if(tabView) tabView->refreshAll();
  };

  // Generate initial idea so the UI shows something.
  processor.getCurrentIdea().seed = 12345;
  processor.generateFull();
  if(tabView) tabView->refreshAll();
}

void MainComponent::paint(juce::Graphics& g) {
  g.fillAll(Palette::background);
}

void MainComponent::resized() {
  auto r = getLocalBounds();
  header->setBounds(r.removeFromTop(64));
  sidebar->setBounds(r.removeFromLeft(200));
  tabs->setBounds(r);
}

} // namespace MidiForge
