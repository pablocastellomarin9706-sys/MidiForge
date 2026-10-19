#pragma once

#include "../Core/Types.h"
#include <vector>
#include <utility>

namespace MidiForge::Content {

// GM-ish drum pitches used across patterns.
inline constexpr int kKick = 36;
inline constexpr int kSnare = 38;
inline constexpr int kClap = 39;
inline constexpr int kRim = 37;
inline constexpr int kHat = 42;
inline constexpr int kOpenHat = 46;
inline constexpr int kCrash = 49;

struct DrumHit {
  int step{0};     // 16th-note step within one bar (0-15)
  int pitch{kKick};
  int velocity{100};
};

struct DrumPattern {
  juce::String name;
  Style style{Style::Pop};
  std::vector<DrumHit> hits; // one bar; looped across idea.bars
  Character character{Character::None}; // None = generic, fits any character
};

struct BassHit {
  int step{0};          // 16th step within one bar
  int interval{0};      // semitones above the chord root (low octave)
  double lenBeats{1.0};
  int velocity{105};
};

struct BassPattern {
  juce::String name;
  Style style{Style::Pop};
  std::vector<BassHit> hits;
  Character character{Character::None}; // None = generic, fits any character
};

// Rhythmic cell for one bar: (offsetBeats, lenBeats).
struct Motif {
  juce::String name;
  std::vector<std::pair<double, double>> notes;
};

const std::vector<DrumPattern>& allDrumPatterns();
const std::vector<BassPattern>& allBassPatterns();
const std::vector<Motif>& allMotifs();

// Picks the pool of patterns for a style. When a character is active its
// dedicated patterns are preferred, falling back to the generic ones.
std::vector<int> drumIndicesFor(Style style, Character character = Character::None);
std::vector<int> bassIndicesFor(Style style, Character character = Character::None);

// (offsetBeats, lenBeats) hits for the chord track, per style, within one bar.
std::vector<std::pair<double, double>> chordRhythmFor(Style style);

} // namespace MidiForge::Content
