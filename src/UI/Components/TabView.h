#pragma once

#include <vector>
#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include "../../PluginProcessor.h"
#include "ChordStrip.h"
#include "PianoRollMini.h"
#include "DrumGrid.h"
#include "DragHandle.h"

namespace MidiForge {

// One page per track type. Shows a preview widget + per-track actions.
class TrackPage : public juce::Component {
public:
  TrackPage(MidiForgeAudioProcessor& p, juce::String name, int previewKind);
  void paint(juce::Graphics&) override;
  void resized() override;
  void refresh();

private:
  MidiForgeAudioProcessor& processor;
  juce::String trackName;
  int kind; // 0 chords, 1 melody/bass piano roll, 2 drums
  ChordStrip chordStrip;
  PianoRollMini pianoRoll;
  DrumGrid drumGrid;
  DragHandle drag;

  const TrackIdea* findTrack() const;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackPage)
};

class TabView : public juce::Component {
public:
  explicit TabView(MidiForgeAudioProcessor& p);
  void resized() override;
  void refreshAll();

private:
  juce::TabbedComponent tabs{juce::TabbedButtonBar::TabsAtTop};
  std::vector<std::unique_ptr<TrackPage>> pages;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TabView)
};

} // namespace MidiForge
