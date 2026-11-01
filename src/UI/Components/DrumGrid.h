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
  void mouseDrag(const juce::MouseEvent&) override;
  void mouseUp(const juce::MouseEvent&) override;
  bool keyPressed(const juce::KeyPress&) override;

  juce::Rectangle<float> gridArea() const;
  juce::Rectangle<float> tabsArea() const;
  int barTabAt(juce::Point<float> pos) const;
  bool cellAt(juce::Point<float> pos, int& lane, int& step) const;
  bool cellHasHit(int lane, int step) const;
  bool applyCell(int lane, int step, bool add);
  void pushUndo();
  void undo();

  TrackIdea* track{nullptr};
  int bars{4};
  int currentBar{0};
  bool editable{true};

  bool painting{false};
  bool paintAdd{false};
  bool dragChanged{false};
  int lastLane{-1};
  int lastStep{-1};
  std::vector<std::vector<Note>> undoStack;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrumGrid)
};

} // namespace MidiForge
