#pragma once

#include <functional>

#include <juce_gui_basics/juce_gui_basics.h>
#include "../../Core/Types.h"

namespace MidiForge {

// Step editor for the drum track: one bar at a time, 16 steps, 10 GM lanes.
// Click a cell to toggle a hit; bar tabs switch between the bars of the loop.
// Edits are marked on the track and saved with the project like the piano roll.
class DrumGrid : public juce::Component,
                 public juce::SettableTooltipClient {
public:
  DrumGrid() = default;

  void setTrack(TrackIdea* t, int bars, bool canEdit);
  void paint(juce::Graphics&) override;

  std::function<void()> onEdited;

private:
  void mouseDown(const juce::MouseEvent&) override;

  juce::Rectangle<float> gridArea() const;
  juce::Rectangle<float> tabsArea() const;
  int barTabAt(juce::Point<float> pos) const;
  bool cellAt(juce::Point<float> pos, int& lane, int& step) const;
  void toggleCell(int lane, int step);

  TrackIdea* track{nullptr};
  int bars{4};
  int currentBar{0};
  bool editable{true};

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrumGrid)
};

} // namespace MidiForge
