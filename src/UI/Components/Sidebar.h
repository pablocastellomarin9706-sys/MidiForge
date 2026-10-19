#pragma once

#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>
#include "../../PluginProcessor.h"

namespace MidiForge {

class Sidebar : public juce::Component, public juce::ListBoxModel {
public:
  explicit Sidebar(MidiForgeAudioProcessor& p);
  void paint(juce::Graphics&) override;
  void resized() override;
  void refreshFromIdea();

  int getNumRows() override;
  void paintListBoxItem(int rowNumber, juce::Graphics&, int width, int height, bool rowIsSelected) override;
  void listBoxItemClicked(int row, const juce::MouseEvent&) override;

  std::function<void()> onIdeaChanged;

private:
  MidiForgeAudioProcessor& processor;
  juce::Label titleLabel;
  juce::Label charLabel;
  juce::ComboBox charBox;
  juce::Label historyLabel;
  juce::ListBox historyList;
  juce::StringArray history;

  // Snapshot of each generated idea, index-matched to `history`.
  struct HistoryEntry {
    int seed;
    int take;
    Character character;
    int bars;
    int keyRoot;
    ScaleType scale;
    Style style;
    double swing;
    double brightness;
    float creativity;
  };
  std::vector<HistoryEntry> historyParams;
  int historyCounter{0};
  bool restoringHistory{false};

  void restoreHistoryRow(int row);

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Sidebar)
};

} // namespace MidiForge
