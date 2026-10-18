#pragma once

#include "../Core/Types.h"

namespace MidiForge {
namespace Content {

// Per-character (sub-genre) MIDI DNA, derived from the production guide:
// swing + human timing jitter + velocity spread + note length + ghost hats.
struct CharacterPreset {
  bool valid{false};
  double swing{0.0};      // % (0-100) applied when the character is picked
  double jitter{0.0};     // +/- fraction of a beat applied to note starts
  int velSigma{4};        // +/- random velocity spread
  double lenScale{1.0};   // note length multiplier (staccato < 1, legato > 1)
  double ghostProb{0.0};  // chance of extra ghost-hat 16ths
  int octaveShift{0};     // Melody-only octave bias
};

inline CharacterPreset characterPresetOf(Character c) {
  switch (c) {
    case Character::Detroit:  return {true, 12.0, 0.020, 6, 1.00, 0.25,  0};
    case Character::TrapDark: return {true,  3.0, 0.010, 5, 0.75, 0.35,  0};
    case Character::Plugg:    return {true,  6.0, 0.015, 4, 1.25, 0.10,  1};
    case Character::NuJerk:   return {true,  2.0, 0.008, 7, 0.55, 0.30,  0};
    case Character::BoomBap:  return {true, 22.0, 0.045, 8, 1.10, 0.40, -1};
    case Character::None:     break;
  }
  return {};
}

} // namespace Content
} // namespace MidiForge
