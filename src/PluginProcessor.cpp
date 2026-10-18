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
  formatManager.registerBasicFormats();
}

MidiForgeAudioProcessor::~MidiForgeAudioProcessor() {}

void MidiForgeAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
  transport.prepareToPlay(sampleRate, samplesPerBlock);
}

void MidiForgeAudioProcessor::releaseResources() {
  transport.releaseResources();
}

void MidiForgeAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) {
  buffer.clear();
  (void)midi;

  // Cache the host tempo (drives the sample "fit" filter).
  if (auto* ph = getPlayHead()) {
    if (auto pos = ph->getPosition()) {
      if (auto bpm = pos->getBpm()) {
        const double b = *bpm;
        if (b > 20.0 && b < 400.0) hostBpm.store(b);
      }
    }
  }

  // Sample audition output.
  if (previewing.load()) {
    if (transport.isPlaying() && !transport.hasStreamFinished()) {
      juce::AudioSourceChannelInfo info(&buffer, 0, buffer.getNumSamples());
      transport.getNextAudioBlock(info);
    } else {
      previewing.store(false);
    }
  }
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
  xml.setAttribute("favorites", sampleLibrary.getFavorites().joinIntoString("\n"));
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

  auto favorites = juce::StringArray::fromLines(xml->getStringAttribute("favorites", ""));
  favorites.removeEmptyStrings();
  sampleLibrary.setFavorites(favorites);
}

void MidiForgeAudioProcessor::generateFull() {
  IdeaGenerator gen;
  gen.generateFull(currentIdea);
  IdeaGenerator::applyCreativity(currentIdea, currentIdea.creativity, 0);
}

void MidiForgeAudioProcessor::regenerateTrack(const juce::String& name) {
  ++regenCounter;
  IdeaGenerator gen;
  gen.regenerateTrack(currentIdea, name, regenCounter);
  if (currentIdea.creativity > 0.001f) {
    juce::Random rng(currentIdea.seed * 7919 + regenCounter * 104729 + 101);
    for (auto& t : currentIdea.tracks)
      if (t.name == name)
        IdeaGenerator::applyCreativityToTrack(t, currentIdea.creativity, rng);
  }
}

bool MidiForgeAudioProcessor::previewSample(const juce::File& file) {
  stopPreview();

  std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
  if (reader == nullptr) return false;

  const double fileRate = reader->sampleRate;
  readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader.release(), true);
  transport.setSource(readerSource.get(), 0, nullptr, fileRate);
  transport.setPosition(0.0);
  transport.setGain(0.85f);
  transport.start();
  previewing.store(true);
  return true;
}

void MidiForgeAudioProcessor::stopPreview() {
  transport.stop();
  transport.setSource(nullptr);
  readerSource = nullptr;
  previewing.store(false);
}

} // namespace MidiForge

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new MidiForge::MidiForgeAudioProcessor();
}
