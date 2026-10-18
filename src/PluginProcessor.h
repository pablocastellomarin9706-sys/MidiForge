#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include "Core/Types.h"
#include "Samples/SampleLibrary.h"

namespace MidiForge {

class MidiForgeAudioProcessor : public juce::AudioProcessor {
public:
  MidiForgeAudioProcessor();
  ~MidiForgeAudioProcessor() override;

  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;
  void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

  juce::AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override;

  const juce::String getName() const override;
  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int index, const juce::String& newName) override;

  void getStateInformation(juce::MemoryBlock& destData) override;
  void setStateInformation(const void* data, int sizeInBytes) override;

  SessionIdea& getCurrentIdea() { return currentIdea; }
  void generateFull();
  void regenerateTrack(const juce::String& name);
  SampleLibrary& getSampleLibrary() { return sampleLibrary; }

  // Host transport tempo (read from the playhead in processBlock; cached).
  double getHostBpm() const { return hostBpm.load(); }

  // Sample audition: previews a file through the plugin's audio output.
  bool previewSample(const juce::File& file);
  void stopPreview();
  bool isPreviewing() const { return previewing.load(); }

private:
  SessionIdea currentIdea;
  int regenCounter{0};
  SampleLibrary sampleLibrary;

  std::atomic<double> hostBpm{0.0};

  juce::AudioFormatManager formatManager;
  std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
  juce::AudioTransportSource transport;
  std::atomic<bool> previewing{false};

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiForgeAudioProcessor)
};

} // namespace MidiForge
