#include "PluginEditor.h"
#include "UI/LookAndFeel.h"

namespace MidiForge {

MidiForgeAudioProcessorEditor::MidiForgeAudioProcessorEditor(MidiForgeAudioProcessor& p)
  : AudioProcessorEditor(&p), processor(p)
{
  setLookAndFeel(&lookAndFeel);
  mainComp = std::make_unique<MainComponent>(processor);
  addAndMakeVisible(*mainComp);
  setResizable(true, true);
  setResizeLimits(760, 480, 2400, 1600);
  setSize(1000, 640);
}

MidiForgeAudioProcessorEditor::~MidiForgeAudioProcessorEditor() {
  setLookAndFeel(nullptr);
}

void MidiForgeAudioProcessorEditor::paint(juce::Graphics& g) {
  g.fillAll(juce::Colour(0xff0f0f0f));
}

void MidiForgeAudioProcessorEditor::resized() {
  if (mainComp) mainComp->setBounds(getLocalBounds());
}

} // namespace MidiForge
