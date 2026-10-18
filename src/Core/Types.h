#pragma once

#include <vector>
#include <string>
#include <cstdint>

#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>

namespace MidiForge {

enum class ScaleType {
  Major,
  MinorNatural,
  MinorHarmonic,
  MinorMelodic,
  Dorian,
  Phrygian,
  Lydian,
  Mixolydian,
  Aeolian,
  Locrian,
  PentatonicMajor,
  PentatonicMinor,
  Blues,
  Chromatic
};

enum class Style {
  Pop,
  LoFi,
  House,
  HipHop,
  EDM,
  Rock
};

inline juce::String styleName(Style s) {
  switch (s) {
    case Style::Pop: return "Pop";
    case Style::LoFi: return "LoFi";
    case Style::House: return "House";
    case Style::HipHop: return "HipHop";
    case Style::EDM: return "EDM";
    case Style::Rock: return "Rock";
  }
  return "Pop";
}

// Sub-genre character inside the HipHop family (Detroit/plugg/jerk/...).
enum class Character {
  None,
  Detroit,
  TrapDark,
  Plugg,
  NuJerk,
  BoomBap,
  Rage,
  TrapLatino
};

inline juce::String characterName(Character c) {
  switch (c) {
    case Character::None: return "None";
    case Character::Detroit: return "Detroit";
    case Character::TrapDark: return "Trap Oscuro";
    case Character::Plugg: return "Plugg";
    case Character::NuJerk: return "Nu-Jerk";
    case Character::BoomBap: return "Boom Bap";
    case Character::Rage: return "Rage";
    case Character::TrapLatino: return "Trap Latino";
  }
  return "None";
}

struct Note {
  int pitch;      // MIDI note 0-127
  double start;   // beats
  double length;  // beats
  int velocity;   // 1-127
  int channel;    // 1-16
};

struct TrackIdea {
  juce::String name;           // Chords/Melody/Bass/Drums
  std::vector<Note> notes;
  int channel;
  juce::Colour colour;
  bool mute{false};
  bool solo{false};
  juce::String detail;         // e.g. "I-V-vi-IV" or pattern name, shown in UI
  std::vector<juce::String> chordSymbols; // per-chord names for the Chords preview
};

struct ChordDef {
  juce::String name;
  std::vector<int> intervals; // semitones from root
  double lengthBeats{4.0};
};

struct SessionIdea {
  int seed{12345};
  int take{0};   // variation counter for Shuffle (same seed, new take)
  int bars{4};
  int keyRoot{0}; // 0-11 C..B
  ScaleType scale{ScaleType::Major};
  Style style{Style::Pop};
  Character character{Character::None};
  double swing{0.0}; // 0-100%
  double strum{0.0}; // beats offset 0-0.25
  int strumDir{0};   // 0 up, 1 down, 2 random
  double brightness{0.6}; // 0-1: higher = brighter voicings, more extensions, hotter velocity
  // Curated content selection (drives Replace). -1 = not yet chosen.
  int progressionIdx{-1};
  int drumPatternIdx{-1};
  int bassPatternIdx{-1};
  int melodyMotifIdx{-1};
  juce::String progressionDisplay;
  juce::String progressionName;
  juce::String drumPatternName;
  juce::String bassPatternName;
  std::vector<TrackIdea> tracks;
  juce::String id;
};

} // namespace MidiForge
