#pragma once

#include "../Core/Types.h"
#include <vector>

namespace MidiForge::Content {

struct ChordSlot {
  int degree{1};          // scale degree 1-7
  int rootOffset{0};      // semitone shift for borrowed chords (e.g. bVII = degree 7, offset -1)
  juce::String quality{"auto"}; // "auto" (diatonic), "maj", "min", "dim"
  bool seventh{false};
};

struct Progression {
  juce::String name;        // "Pop Four Chords"
  juce::String display;     // "I - V - vi - IV"
  Style style{Style::Pop};
  ScaleType scale{ScaleType::Major};
  std::vector<ChordSlot> chords; // one slot per bar (loops if idea has more bars)
};

struct RealizedChord {
  int rootMidi{60};
  std::vector<int> intervals;
  std::vector<int> pitches; // voiced with inversions, ascending
  double startBeat{0.0};
  double beats{4.0};
  juce::String symbol;      // "Am", "Fmaj7"
};

const std::vector<Progression>& allProgressions();

// Indices of progressions that fit a style + chord-scale family.
// Falls back to any progression of the matching scale family.
std::vector<int> progressionIndicesFor(Style style, ScaleType chordScale);

// Resolve a progression into concrete voiced chords for an idea.
// Loops/truncates the slot list to cover idea.bars.
std::vector<RealizedChord> realizeProgression(const SessionIdea& idea);

} // namespace MidiForge::Content
