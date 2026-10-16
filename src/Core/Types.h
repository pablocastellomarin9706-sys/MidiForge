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
};

struct ChordDef {
  juce::String name;
  std::vector<int> intervals; // semitones from root
  double lengthBeats{4.0};
};

struct SessionIdea {
  int seed{12345};
  int bars{4};
  int keyRoot{0}; // 0-11 C..B
  ScaleType scale{ScaleType::Major};
  double swing{0.0}; // 0-100%
  double strum{0.0}; // ms-like in beats? use beats offset 0-0.25
  int strumDir{0};   // 0 up, 1 down, 2 random
  std::vector<TrackIdea> tracks;
  juce::String id;
};

} // namespace MidiForge
