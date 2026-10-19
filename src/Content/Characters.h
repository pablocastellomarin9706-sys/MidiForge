#pragma once

#include "../Core/Types.h"

namespace MidiForge {
namespace Content {

// Per-character (sub-genre) MIDI DNA, calibrated against the user's
// reference artists (see REFERENCES.md):
//   Plugg     -> Pi'erre Bourne (bells, space, glide 808)
//   Rage      -> Yeat / AlanTheChemist (dark leads, hard 808)
//   NuJerk    -> Fakemink (mellowed jerk: staccato but hazy)
//   TrapLatino-> JC Reyes / 8Belial / El Bugg / Huan 62 / Fanta / NTG
//   BoomBap   -> Lord Finesse / Cookin Soul / Gloosito
struct CharacterPreset {
  bool valid{false};
  double swing{0.0};       // % (0-100) applied when the character is picked
  double jitter{0.0};      // +/- fraction of a beat applied to note starts
  int velSigma{4};         // +/- random velocity spread
  double lenScale{1.0};    // melodic note length multiplier (staccato < 1)
  double ghostProb{0.0};   // chance of extra ghost-hat 16ths
  int octaveShift{0};      // Melody-only octave bias
  double bassLenScale{1.0};// Bass only: > 1 overlaps the next 808 (glide)
  double density{1.0};     // Melody: 1.0 keeps all notes, lower = more air
};

inline CharacterPreset characterPresetOf(Character c) {
  switch (c) {
    //                 valid  swing jitter  vel  len  ghost oct  bass  dens
    case Character::Detroit:    return {true, 10.0, 0.018, 6, 1.00, 0.25,  0, 1.40, 1.00};
    case Character::TrapDark:   return {true,  3.0, 0.010, 5, 0.72, 0.35,  0, 1.50, 1.00};
    case Character::Plugg:      return {true,  9.0, 0.015, 4, 1.20, 0.08,  1, 1.60, 0.75};
    case Character::NuJerk:     return {true,  6.0, 0.012, 5, 0.65, 0.20,  0, 1.20, 0.85};
    case Character::BoomBap:    return {true, 22.0, 0.045, 8, 1.10, 0.40, -1, 1.00, 1.00};
    case Character::Rage:       return {true,  2.0, 0.006, 9, 0.70, 0.25,  0, 1.50, 0.85};
    case Character::TrapLatino: return {true,  5.0, 0.012, 6, 0.90, 0.45,  0, 1.70, 0.90};
    case Character::None:       break;
  }
  return {};
}

// Scale a character leans on when the user has not forced a key colour.
// The minor-family characters sound wrong over a major scale, so when the
// idea is Major we snap it to the character's home mode.
inline ScaleType defaultScaleFor(Character c) {
  switch (c) {
    case Character::TrapLatino: return ScaleType::MinorHarmonic;
    case Character::Detroit:
    case Character::TrapDark:
    case Character::Plugg:
    case Character::NuJerk:
    case Character::Rage:       return ScaleType::MinorNatural;
    case Character::None:       break;
  }
  return ScaleType::Major;
}

} // namespace Content
} // namespace MidiForge
