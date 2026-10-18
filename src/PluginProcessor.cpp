#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Generator/IdeaGenerator.h"

namespace MidiForge {

MidiForgeAudioProcessor::MidiForgeAudioProcessor()
  : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
  currentIdea.seed = 12345;
  currentIdea.bars = 4;
  currentIdea.keyRoot = 0;
  currentIdea.scale = ScaleType::Major;
}

MidiForgeAudioProcessor::~MidiForgeAudioProcessor() {}

void MidiForgeAudioProcessor::prepareToPlay(double, int) {}
void MidiForgeAudioProcessor::releaseResources() {}
void MidiForgeAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) {
  buffer.clear();
  (void)midi;
}

bool MidiForgeAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* MidiForgeAudioProcessor::createEditor() {
  return new MidiForgeAudioProcessorEditor(*this);
}

const juce::String MidiForgeAudioProcessor::getName() const { return "MidiForge"; }
bool MidiForgeAudioProcessor::acceptsMidi() const { return false; }
bool MidiForgeAudioProcessor::producesMidi() const { return false; }
bool MidiForgeAudioProcessor::isMidiEffect() const { return false; }
double MidiForgeAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int MidiForgeAudioProcessor::getNumPrograms() { return 1; }
int MidiForgeAudioProcessor::getCurrentProgram() { return 0; }
void MidiForgeAudioProcessor::setCurrentProgram(int) {}
const juce::String MidiForgeAudioProcessor::getProgramName(int) { return "Default"; }
void MidiForgeAudioProcessor::changeProgramName(int, const juce::String&) {}

void MidiForgeAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
  juce::XmlElement xml("MidiForgeState");
  xml.setAttribute("folders", sampleLibrary.getFolders().joinIntoString("\n"));
  copyXmlToBinary(xml, destData);
}

void MidiForgeAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
  auto xml = getXmlFromBinary(data, sizeInBytes);
  if (xml == nullptr) return;

  juce::StringArray folders =
      juce::StringArray::fromLines(xml->getStringAttribute("folders", ""));
  if (folders.isEmpty())
    folders.add(juce::File::getSpecialLocation(juce::File::userMusicDirectory).getFullPathName());
  sampleLibrary.setFolders(folders);
}

void MidiForgeAudioProcessor::generateFull() {
  IdeaGenerator gen;
  gen.generateFull(currentIdea);
}

void MidiForgeAudioProcessor::regenerateTrack(const juce::String& name) {
  ++regenCounter;
  IdeaGenerator gen;
  gen.regenerateTrack(currentIdea, name, regenCounter);
}

} // namespace MidiForge

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new MidiForge::MidiForgeAudioProcessor();
}
