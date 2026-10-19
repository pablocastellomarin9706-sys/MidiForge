#pragma once

#include <memory>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>

#include "../../PluginProcessor.h"

namespace MidiForge {

// "SONIDO" tab: Auto-Sound picks one sound per layer for the current idea,
// auditions it, and hands out drag chips (sample + rendered WAV loop). The
// Freesound button downloads a CC0 sound when the user pasted an API key.
class AutoSoundPanel : public juce::Component, private juce::Timer {
public:
  explicit AutoSoundPanel(MidiForgeAudioProcessor& p);
  ~AutoSoundPanel() override;

  void paint(juce::Graphics&) override;
  void resized() override;
  void visibilityChanged() override;
  void refresh();

private:
  struct Row; // defined in the .cpp

  void timerCallback() override;
  void updateRow(Row& r);
  void applyReroll(Row& r);
  void applyOnline(Row& r);
  void applyWeb(Row& r);
  void applyPlay(Row& r);
  juce::File loopFileFor(const Row& r) const;

  MidiForgeAudioProcessor& processor;
  std::vector<std::unique_ptr<Row>> rows;
  std::unique_ptr<juce::Label> titleLabel;
  std::unique_ptr<juce::Label> subtitleLabel;
  std::unique_ptr<juce::TextButton> rerollAllButton;
  std::unique_ptr<juce::Label> rerollHintLabel;
  std::unique_ptr<juce::Label> keyLabel;
  std::unique_ptr<juce::TextEditor> keyEditor;
  std::unique_ptr<juce::TextButton> saveKeyButton;
  std::unique_ptr<juce::Label> statusLabel;
  int pendingOnlineRow{-1};
  bool lazyFillDone{false};
  juce::String lastStatus;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AutoSoundPanel)
};

} // namespace MidiForge
