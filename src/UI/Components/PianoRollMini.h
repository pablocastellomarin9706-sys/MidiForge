#pragma once

#include <functional>

#include <juce_gui_basics/juce_gui_basics.h>
#include "../../Core/Types.h"

namespace MidiForge {

// Steel mini piano roll. In Melody/Bass it is editable: drag a note to move it,
// pull the right edge to resize, double-click empty space to add, right-click to
// delete and Ctrl+Z to undo. Edits are marked on the track and serialised with
// the project state, so they survive closing and reopening the session.
class PianoRollMini : public juce::Component,
                      public juce::SettableTooltipClient {
public:
  PianoRollMini() = default;

  void setTrack(TrackIdea* t, int bars, bool canEdit, int bStartBar = -1);
  void paint(juce::Graphics&) override;

  std::function<void()> onEdited;

private:
  int noteAt(juce::Point<float> pos) const;
  void pushUndo();
  void undo();

  void mouseDown(const juce::MouseEvent&) override;
  void mouseDrag(const juce::MouseEvent&) override;
  void mouseUp(const juce::MouseEvent&) override;
  void mouseDoubleClick(const juce::MouseEvent&) override;
  bool keyPressed(const juce::KeyPress&) override;

  TrackIdea* track{nullptr};
  int bars{4};
  int bStart{-1}; // first bar of section B (-1 = none); marks the roll
  bool editable{false};
  int minPitch{60}, maxPitch{84};
  double maxBeat{16.0};

  int selected{-1};
  int dragIndex{-1};
  bool resizing{false};
  bool moved{false};
  bool undoPushed{false};
  double pressBeat{0.0};
  int pressPitch{0};
  double origStart{0.0}, origLength{0.25};
  int origPitch{60};

  std::vector<std::vector<Note>> undoStack;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PianoRollMini)
};

} // namespace MidiForge
