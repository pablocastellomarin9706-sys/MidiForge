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

  // The optional Freesound API key is persisted outside the project too, so it
  // only has to be pasted once.
  const auto keyFile = freesoundKeyFile();
  if (keyFile.existsAsFile())
    freesoundKey = keyFile.loadFileAsString().trim();
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

  // Sample audition output (audio thread). The spinlock only guards the brief
  // block pull; rewiring the transport happens on the message thread under the
  // same lock, so readers never see a half-rebuilt source chain.
  {
    const juce::SpinLock::ScopedLockType sl(previewLock);
    if (previewing.load()) {
      if (transport.isPlaying() && !transport.hasStreamFinished()) {
        juce::AudioSourceChannelInfo info(&buffer, 0, buffer.getNumSamples());
        transport.getNextAudioBlock(info);
      } else {
        previewing.store(false);
      }
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
  xml.setAttribute("version", 3);
  xml.setAttribute("folders", sampleLibrary.getFolders().joinIntoString("\n"));
  xml.setAttribute("favorites", sampleLibrary.getFavorites().joinIntoString("\n"));
  xml.setAttribute("freesoundKey", freesoundKey);

  // Last idea (musical settings + curated picks). The generators are fully
  // deterministic given seed + take, so a reopened session rebuilds the exact
  // same idea instead of losing what the user was working on.
  const auto& idea = currentIdea;
  xml.setAttribute("seed", idea.seed);
  xml.setAttribute("take", idea.take);
  xml.setAttribute("bars", idea.bars);
  xml.setAttribute("keyRoot", idea.keyRoot);
  xml.setAttribute("scale", (int)idea.scale);
  xml.setAttribute("style", (int)idea.style);
  xml.setAttribute("character", (int)idea.character);
  xml.setAttribute("swing", idea.swing);
  xml.setAttribute("brightness", idea.brightness);
  xml.setAttribute("creativity", (double)idea.creativity);
  xml.setAttribute("regen", regenCounter);
  xml.setAttribute("progIdx", idea.progressionIdx);
  xml.setAttribute("drumIdx", idea.drumPatternIdx);
  xml.setAttribute("bassIdx", idea.bassPatternIdx);
  xml.setAttribute("motifIdx", idea.melodyMotifIdx);
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

  const auto storedKey = xml->getStringAttribute("freesoundKey", juce::String());
  if (storedKey.isNotEmpty()) freesoundKey = storedKey;

  // Older states (v < 3) only carried folders/favourites.
  if (!xml->hasAttribute("seed")) return;

  auto& idea = currentIdea;
  idea.seed = xml->getIntAttribute("seed", 12345);
  idea.take = xml->getIntAttribute("take", 0);
  idea.bars = juce::jlimit(1, 32, xml->getIntAttribute("bars", 4));
  idea.keyRoot = juce::jlimit(0, 11, xml->getIntAttribute("keyRoot", 0));
  idea.scale = (ScaleType)juce::jlimit(0, (int)ScaleType::Chromatic,
                                       xml->getIntAttribute("scale", (int)ScaleType::Major));
  idea.style = (Style)juce::jlimit(0, (int)Style::Rock,
                                   xml->getIntAttribute("style", (int)Style::Pop));
  idea.character = (Character)juce::jlimit(0, (int)Character::TrapLatino,
                                           xml->getIntAttribute("character", 0));
  idea.swing = xml->getDoubleAttribute("swing", 0.0);
  idea.brightness = juce::jlimit(0.0, 1.0, xml->getDoubleAttribute("brightness", 0.6));
  idea.creativity = (float)juce::jlimit(0.0, 1.0, xml->getDoubleAttribute("creativity", 0.0));
  regenCounter = xml->getIntAttribute("regen", 0);
  idea.progressionIdx = xml->getIntAttribute("progIdx", -1);
  idea.drumPatternIdx = xml->getIntAttribute("drumIdx", -1);
  idea.bassPatternIdx = xml->getIntAttribute("bassIdx", -1);
  idea.melodyMotifIdx = xml->getIntAttribute("motifIdx", -1);

  // Rebuild the exact same idea. If a catalog has changed since the save and
  // a curated pick no longer exists, generateFromSaved falls back to pickAll.
  IdeaGenerator gen;
  gen.generateFromSaved(idea);
  IdeaGenerator::applyCreativity(idea, idea.creativity, 0);

  // Sounds for the restored idea (best effort: the sample scan may still be
  // running; the SONIDO tab tops up the empty slots once it is ready).
  autoSoundRoll = 0;
  pickAutoSoundsForIdea();
}

void MidiForgeAudioProcessor::generateFull() {
  IdeaGenerator gen;
  gen.generateFull(currentIdea);
  IdeaGenerator::applyCreativity(currentIdea, currentIdea.creativity, 0);
  autoSoundRoll = 0;
  pickAutoSoundsForIdea();
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

  // A replaced layer also gets a fresh sound for that layer.
  if (name == "Melody") rerollAutoSound(SoundSlot::Melody);
  else if (name == "Chords") rerollAutoSound(SoundSlot::Chords);
  else if (name == "Bass") rerollAutoSound(SoundSlot::Bass);
  else if (name == "Drums")
    for (int s = (int)SoundSlot::Kick; s <= (int)SoundSlot::Crash; ++s)
      rerollAutoSound((SoundSlot)s);
}

void MidiForgeAudioProcessor::pickAutoSoundsForIdea() {
  const auto lib = sampleLibrary.snapshot();
  ++autoSoundRoll;
  const int seed = currentIdea.seed * 7 + currentIdea.take * 7919 + autoSoundRoll * 104729 + 1;
  autoSounds = pickAutoSounds(lib, currentIdea.character, currentIdea.keyRoot, seed);
}

void MidiForgeAudioProcessor::fillEmptyAutoSounds() {
  const auto lib = sampleLibrary.snapshot();
  if (lib.empty()) return;
  for (int s = 0; s < kNumSoundSlots; ++s) {
    if (autoSounds.paths[s].isNotEmpty()) continue;
    ++autoSoundRoll;
    const int seed = currentIdea.seed * 7 + currentIdea.take * 7919 +
                     autoSoundRoll * 104729 + s * 131;
    pickOneSlot(autoSounds, lib, currentIdea.character, currentIdea.keyRoot,
                (SoundSlot)s, seed, {});
  }
}

void MidiForgeAudioProcessor::rerollAutoSound(SoundSlot slot) {
  const auto lib = sampleLibrary.snapshot();
  if (lib.empty()) return;
  ++autoSoundRoll;
  const int seed = currentIdea.seed * 7 + currentIdea.take * 7919 + autoSoundRoll * 104729 +
                   (int)slot * 131;
  pickOneSlot(autoSounds, lib, currentIdea.character, currentIdea.keyRoot, slot, seed,
              autoSounds.paths[(int)slot]);
}

juce::File MidiForgeAudioProcessor::renderTrackAudio(const juce::String& trackName) {
  return renderIdeaToWav(currentIdea, trackName, autoSounds, getHostBpm(), false);
}

juce::File MidiForgeAudioProcessor::renderMixAudio() {
  return renderIdeaToWav(currentIdea, juce::String(), autoSounds, getHostBpm(), true);
}

juce::File MidiForgeAudioProcessor::freesoundKeyFile() const {
  return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
      .getChildFile("MidiForge")
      .getChildFile("freesound_key.txt");
}

void MidiForgeAudioProcessor::setFreesoundKey(const juce::String& key) {
  freesoundKey = key.trim();
  const auto f = freesoundKeyFile();
  if (freesoundKey.isEmpty()) {
    f.deleteFile();
  } else {
    f.getParentDirectory().createDirectory();
    f.replaceWithText(freesoundKey);
  }
}

bool MidiForgeAudioProcessor::fetchFreesoundForSlot(SoundSlot slot, juce::String& status) {
  const auto cache = juce::File::getSpecialLocation(juce::File::userMusicDirectory)
                         .getChildFile("MidiForge_Cache");
  juce::String st;
  const auto f = fetchFreesoundSound(freesoundKey, slot, currentIdea.character, cache, st);
  status = st;
  if (!f.existsAsFile()) return false;
  autoSounds.paths[(int)slot] = f.getFullPathName();
  autoSounds.names[(int)slot] = f.getFileName();
  return true;
}

bool MidiForgeAudioProcessor::previewSample(const juce::File& file) {
  // Decode outside the lock: file I/O must never stall the audio callback.
  std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
  if (reader == nullptr) return false;
  const double fileRate = reader->sampleRate;

  const juce::SpinLock::ScopedLockType sl(previewLock);
  stopPreviewLocked();

  readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader.release(), true);
  transport.setSource(readerSource.get(), 0, nullptr, fileRate);
  transport.setPosition(0.0);
  transport.setGain(0.85f);
  transport.start();
  previewing.store(true);
  return true;
}

void MidiForgeAudioProcessor::stopPreview() {
  const juce::SpinLock::ScopedLockType sl(previewLock);
  stopPreviewLocked();
}

void MidiForgeAudioProcessor::stopPreviewLocked() {
  transport.stop();
  transport.setSource(nullptr);
  readerSource = nullptr;
  previewing.store(false);
}

} // namespace MidiForge

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new MidiForge::MidiForgeAudioProcessor();
}
