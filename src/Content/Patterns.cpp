#include "Patterns.h"

namespace MidiForge::Content {

static DrumHit dh(int step, int pitch, int vel) { return {step, pitch, vel}; }
static BassHit bh(int step, int interval, double len, int vel) { return {step, interval, len, vel}; }

const std::vector<DrumPattern>& allDrumPatterns() {
  static const std::vector<DrumPattern> pats = {
    {"Pop Backbeat", Style::Pop, {
      dh(0,kKick,120), dh(8,kKick,110),
      dh(4,kSnare,115), dh(12,kSnare,115),
      dh(0,kHat,90), dh(2,kHat,80), dh(4,kHat,95), dh(6,kHat,80),
      dh(8,kHat,90), dh(10,kHat,80), dh(12,kHat,95), dh(14,kHat,85)}},
    {"LoFi Dusted", Style::LoFi, {
      dh(0,kKick,115), dh(10,kKick,100),
      dh(4,kSnare,110), dh(12,kSnare,115),
      dh(0,kHat,80), dh(2,kHat,75), dh(4,kHat,85), dh(6,kHat,75),
      dh(8,kHat,80), dh(10,kHat,85), dh(12,kHat,90), dh(14,kHat,75)}},
    {"House Four Floor", Style::House, {
      dh(0,kKick,122), dh(4,kKick,122), dh(8,kKick,122), dh(12,kKick,122),
      dh(4,kClap,110), dh(12,kClap,110),
      dh(2,kHat,95), dh(6,kHat,95), dh(10,kHat,95), dh(14,kHat,100),
      dh(14,kOpenHat,88)}},
    {"Boom Bap 90", Style::HipHop, {
      dh(0,kKick,120), dh(7,kKick,105), dh(10,kKick,110),
      dh(4,kSnare,115), dh(12,kSnare,120),
      dh(0,kHat,85), dh(2,kHat,80), dh(4,kHat,90), dh(6,kHat,80),
      dh(8,kHat,85), dh(10,kHat,90), dh(12,kHat,95), dh(14,kHat,80), dh(15,kHat,75)}},
    {"Trap HalfTime", Style::HipHop, {
      dh(0,kKick,120), dh(7,kKick,100), dh(10,kKick,105), dh(11,kKick,95),
      dh(8,kSnare,120),
      dh(0,kHat,80), dh(1,kHat,75), dh(2,kHat,80), dh(3,kHat,85),
      dh(4,kHat,80), dh(5,kHat,75), dh(6,kHat,80), dh(7,kHat,90),
      dh(8,kHat,85), dh(9,kHat,80), dh(10,kHat,85), dh(11,kHat,90),
      dh(12,kHat,80), dh(13,kHat,75), dh(14,kHat,85), dh(15,kHat,95)}},
    {"EDM Big Room", Style::EDM, {
      dh(0,kKick,124), dh(4,kKick,124), dh(8,kKick,124), dh(12,kKick,124),
      dh(4,kClap,112), dh(12,kClap,112),
      dh(0,kHat,85), dh(2,kHat,95), dh(4,kHat,85), dh(6,kHat,95),
      dh(8,kHat,85), dh(10,kHat,95), dh(12,kHat,85), dh(14,kHat,100)}},
    {"Rock Drive", Style::Rock, {
      dh(0,kKick,120), dh(6,kKick,100), dh(8,kKick,115),
      dh(4,kSnare,120), dh(12,kSnare,120),
      dh(0,kHat,90), dh(2,kHat,85), dh(4,kHat,95), dh(6,kHat,85),
      dh(8,kHat,90), dh(10,kHat,85), dh(12,kHat,95), dh(14,kHat,90)}},
    // ---- Character-specific (HipHop family) ----
    // Detroit: hard half-time, punchy syncopated kicks, clap+snare, open hats.
    {"Detroit Punch", Style::HipHop, {
      dh(0,kKick,124), dh(3,kKick,102), dh(6,kKick,112), dh(10,kKick,108), dh(14,kKick,104),
      dh(8,kClap,122), dh(8,kSnare,94),
      dh(4,kOpenHat,90), dh(12,kOpenHat,94),
      dh(0,kHat,84), dh(2,kHat,78), dh(4,kHat,86), dh(6,kHat,80),
      dh(8,kHat,88), dh(10,kHat,82), dh(12,kHat,90), dh(14,kHat,78)}, Character::Detroit},
    // Nu-Jerk: hood-trap kick, half-time snare + low-velocity ghost note at 15.
    {"Nu-Jerk Hood", Style::HipHop, {
      dh(0,kKick,122), dh(6,kKick,106), dh(10,kKick,114), dh(11,kKick,96),
      dh(8,kSnare,120), dh(15,kSnare,44),
      dh(0,kHat,78), dh(2,kHat,72), dh(4,kHat,80), dh(6,kHat,76),
      dh(8,kHat,82), dh(10,kHat,78), dh(12,kHat,84), dh(14,kHat,72)}, Character::NuJerk},
    // Trap Oscuro: rolling 808, density rising across the bar, rim accent.
    {"Trap Dark Roll", Style::HipHop, {
      dh(0,kKick,122), dh(7,kKick,104), dh(10,kKick,110), dh(13,kKick,100),
      dh(8,kSnare,120), dh(15,kRim,58),
      dh(0,kHat,80), dh(1,kHat,72), dh(2,kHat,78), dh(3,kHat,74),
      dh(4,kHat,80), dh(5,kHat,72), dh(6,kHat,78), dh(7,kHat,88),
      dh(8,kHat,82), dh(9,kHat,74), dh(10,kHat,80), dh(11,kHat,88),
      dh(12,kHat,82), dh(13,kHat,74), dh(14,kHat,84), dh(15,kHat,92)}, Character::TrapDark},
    // Plugg: sparse half-time, lots of space, Pi'erre-style open hats.
    {"Plugg Bounce", Style::HipHop, {
      dh(0,kKick,118), dh(10,kKick,106),
      dh(8,kSnare,112),
      dh(4,kOpenHat,86), dh(12,kOpenHat,84),
      dh(0,kHat,78), dh(4,kHat,74), dh(8,kHat,80), dh(12,kHat,76)}, Character::Plugg},
    // Rage: driving hard 808 on the quarters, dense energetic hats.
    {"Rage Sprint", Style::HipHop, {
      dh(0,kKick,124), dh(4,kKick,118), dh(8,kKick,122), dh(12,kKick,118),
      dh(8,kSnare,122),
      dh(0,kHat,86), dh(1,kHat,78), dh(2,kHat,84), dh(3,kHat,80),
      dh(4,kHat,86), dh(5,kHat,78), dh(6,kHat,84), dh(7,kHat,88),
      dh(8,kHat,86), dh(9,kHat,78), dh(10,kHat,84), dh(11,kHat,88),
      dh(12,kHat,86), dh(13,kHat,78), dh(14,kHat,86), dh(15,kHat,92)}, Character::Rage},
    // Trap Latino: dembow-flavoured snare pattern.
    {"Dembow Latino", Style::HipHop, {
      dh(0,kKick,122), dh(8,kKick,116),
      dh(3,kSnare,110), dh(6,kSnare,108), dh(11,kSnare,110), dh(14,kSnare,108),
      dh(0,kHat,80), dh(2,kHat,74), dh(4,kHat,82), dh(6,kHat,76),
      dh(8,kHat,82), dh(10,kHat,76), dh(12,kHat,84), dh(14,kHat,78)}, Character::TrapLatino},
    // ---- Character variants (give Replace something to cycle through) ----
    {"Detroit Slide", Style::HipHop, {
      dh(0,kKick,122), dh(5,kKick,106), dh(7,kKick,112), dh(11,kKick,104), dh(13,kKick,110),
      dh(8,kClap,120), dh(4,kSnare,96),
      dh(2,kOpenHat,88), dh(6,kOpenHat,92), dh(14,kOpenHat,90),
      dh(0,kHat,82), dh(2,kHat,78), dh(4,kHat,84), dh(6,kHat,78),
      dh(8,kHat,84), dh(10,kHat,78), dh(12,kHat,86), dh(14,kHat,80)}, Character::Detroit},
    {"Nu-Jerk Bounce", Style::HipHop, {
      dh(0,kKick,122), dh(4,kKick,104), dh(7,kKick,112), dh(10,kKick,116), dh(14,kKick,100),
      dh(8,kSnare,120), dh(12,kSnare,50),
      dh(0,kHat,76), dh(2,kHat,72), dh(4,kHat,80), dh(6,kHat,74),
      dh(8,kHat,80), dh(10,kHat,74), dh(12,kHat,82), dh(14,kHat,70)}, Character::NuJerk},
    {"Trap Dark Drive", Style::HipHop, {
      dh(0,kKick,122), dh(6,kKick,106), dh(9,kKick,112), dh(12,kKick,104),
      dh(8,kSnare,120), dh(4,kRim,60),
      dh(0,kHat,78), dh(1,kHat,72), dh(2,kHat,80), dh(3,kHat,76),
      dh(4,kHat,82), dh(5,kHat,74), dh(6,kHat,80), dh(7,kHat,90),
      dh(8,kHat,80), dh(9,kHat,74), dh(10,kHat,82), dh(11,kHat,90),
      dh(12,kHat,80), dh(13,kHat,76), dh(14,kHat,86), dh(15,kHat,94)}, Character::TrapDark},
    {"Plugg Skitter", Style::HipHop, {
      dh(0,kKick,116), dh(7,kKick,104), dh(10,kKick,108),
      dh(8,kSnare,112),
      dh(3,kOpenHat,84), dh(11,kOpenHat,86),
      dh(0,kHat,76), dh(2,kHat,72), dh(4,kHat,78), dh(6,kHat,74),
      dh(8,kHat,78), dh(10,kHat,74), dh(12,kHat,80), dh(14,kHat,72)}, Character::Plugg},
    {"Rage Pulse", Style::HipHop, {
      dh(0,kKick,124), dh(3,kKick,114), dh(6,kKick,118), dh(9,kKick,114),
      dh(12,kKick,120), dh(15,kKick,114),
      dh(8,kSnare,122), dh(4,kSnare,58),
      dh(0,kHat,84), dh(2,kHat,78), dh(4,kHat,86), dh(6,kHat,80),
      dh(8,kHat,84), dh(10,kHat,78), dh(12,kHat,88), dh(14,kHat,82)}, Character::Rage},
    {"Dembow Latino II", Style::HipHop, {
      dh(0,kKick,120), dh(6,kKick,112), dh(10,kKick,116),
      dh(3,kSnare,108), dh(7,kSnare,106), dh(11,kSnare,108), dh(15,kSnare,106),
      dh(0,kHat,80), dh(2,kHat,74), dh(4,kHat,82), dh(6,kHat,76),
      dh(8,kHat,82), dh(10,kHat,76), dh(12,kHat,84), dh(14,kHat,78)}, Character::TrapLatino},
  };
  return pats;
}

const std::vector<BassPattern>& allBassPatterns() {
  static const std::vector<BassPattern> pats = {
    {"Pop Roots", Style::Pop, {bh(0,0,2.0,108), bh(8,0,2.0,106)}},
    {"LoFi Long", Style::LoFi, {bh(0,0,3.5,104), bh(14,7,0.5,92)}},
    {"House Offbeat", Style::House, {bh(2,0,0.4,112), bh(6,0,0.4,110), bh(10,0,0.4,112), bh(14,0,0.4,110)}},
    {"Boom Bap Sparse", Style::HipHop, {bh(0,0,2.5,110), bh(10,0,1.0,100), bh(14,12,0.5,96)}},
    {"EDM Rolling", Style::EDM, {
      bh(0,0,0.5,112), bh(2,0,0.5,108), bh(4,12,0.5,110), bh(6,0,0.5,108),
      bh(8,0,0.5,112), bh(10,0,0.5,108), bh(12,12,0.5,110), bh(14,0,0.5,108)}},
    {"Rock Eighths", Style::Rock, {
      bh(0,0,0.5,112), bh(2,0,0.5,108), bh(4,0,0.5,110), bh(6,0,0.5,108),
      bh(8,0,0.5,112), bh(10,0,0.5,108), bh(12,0,0.5,110), bh(14,7,0.5,106)}},
    // ---- Character-specific 808s (HipHop family) ----
    {"Detroit 808", Style::HipHop, {
      bh(0,0,2.0,110), bh(8,0,2.0,108), bh(14,7,0.5,98)}, Character::Detroit},
    {"Nu-Jerk 808", Style::HipHop, {
      bh(0,0,2.5,112), bh(10,0,1.5,106)}, Character::NuJerk},
    {"Dark 808", Style::HipHop, {
      bh(0,0,1.5,110), bh(6,0,1.0,104), bh(10,12,0.5,100), bh(12,0,1.5,106)}, Character::TrapDark},
    {"Plugg Glide", Style::HipHop, {
      bh(0,0,3.5,108), bh(12,7,1.0,100)}, Character::Plugg},
    {"Rage 808", Style::HipHop, {
      bh(0,0,1.0,112), bh(4,0,1.0,108), bh(8,0,1.0,112), bh(12,0,1.0,108)}, Character::Rage},
    {"Latino 808", Style::HipHop, {
      bh(0,0,2.0,110), bh(8,0,2.0,108), bh(14,10,0.75,98)}, Character::TrapLatino},
    // ---- Character 808 variants ----
    {"Detroit 808 B", Style::HipHop, {
      bh(0,0,1.5,110), bh(6,0,1.0,106), bh(10,0,1.5,108), bh(14,12,0.5,98)}, Character::Detroit},
    {"Nu-Jerk 808 B", Style::HipHop, {
      bh(0,0,1.5,112), bh(6,0,1.0,104), bh(12,0,2.0,106)}, Character::NuJerk},
    {"Dark 808 B", Style::HipHop, {
      bh(0,0,2.5,110), bh(10,0,1.0,104), bh(14,7,0.5,98)}, Character::TrapDark},
    {"Plugg Glide B", Style::HipHop, {
      bh(0,0,2.0,108), bh(8,0,2.0,106), bh(14,10,0.75,98)}, Character::Plugg},
    {"Rage 808 B", Style::HipHop, {
      bh(0,0,1.0,112), bh(2,0,0.5,104), bh(4,0,1.0,110), bh(8,0,1.0,112),
      bh(10,0,0.5,104), bh(12,0,1.0,110)}, Character::Rage},
    {"Latino 808 B", Style::HipHop, {
      bh(0,0,1.5,110), bh(6,0,1.0,106), bh(11,0,1.5,108), bh(14,7,0.5,98)}, Character::TrapLatino},
  };
  return pats;
}

const std::vector<Motif>& allMotifs() {
  static const std::vector<Motif> motifs = {
    {"Call", {{0.0,0.5},{0.5,0.5},{1.0,1.0},{2.0,0.5},{3.0,0.5}}},
    {"Response", {{0.0,1.0},{2.0,0.5},{2.5,0.5},{3.0,1.0}}},
    {"Syncopated", {{0.5,0.5},{1.0,0.5},{1.5,1.0},{3.0,0.5}}},
    {"Sparse Air", {{0.0,1.5},{2.0,1.5}}},
    {"Quarter Walk", {{0.0,0.9},{1.0,0.9},{2.0,0.9},{3.0,0.9}}},
    {"Offbeat Float", {{0.5,1.0},{2.5,1.0}}},
  };
  return motifs;
}

std::vector<int> drumIndicesFor(Style style, Character character) {
  const auto& pats = allDrumPatterns();
  std::vector<int> out;
  // 1) Dedicated patterns for this character in this style.
  if (character != Character::None) {
    for (size_t i = 0; i < pats.size(); ++i)
      if (pats[i].style == style && pats[i].character == character) out.push_back((int)i);
    if (!out.empty()) return out;
  }
  // 2) Generic patterns of this style.
  for (size_t i = 0; i < pats.size(); ++i)
    if (pats[i].style == style && pats[i].character == Character::None) out.push_back((int)i);
  if (!out.empty()) return out;
  // 3) Any pattern of this style, then 4) anything.
  for (size_t i = 0; i < pats.size(); ++i)
    if (pats[i].style == style) out.push_back((int)i);
  if (!out.empty()) return out;
  for (size_t i = 0; i < pats.size(); ++i) out.push_back((int)i);
  return out;
}

std::vector<int> bassIndicesFor(Style style, Character character) {
  const auto& pats = allBassPatterns();
  std::vector<int> out;
  if (character != Character::None) {
    for (size_t i = 0; i < pats.size(); ++i)
      if (pats[i].style == style && pats[i].character == character) out.push_back((int)i);
    if (!out.empty()) return out;
  }
  for (size_t i = 0; i < pats.size(); ++i)
    if (pats[i].style == style && pats[i].character == Character::None) out.push_back((int)i);
  if (!out.empty()) return out;
  for (size_t i = 0; i < pats.size(); ++i)
    if (pats[i].style == style) out.push_back((int)i);
  if (!out.empty()) return out;
  for (size_t i = 0; i < pats.size(); ++i) out.push_back((int)i);
  return out;
}

std::vector<std::pair<double, double>> chordRhythmFor(Style style) {
  switch (style) {
    case Style::House:
    case Style::EDM:
      return {{0.0,0.9},{1.0,0.9},{2.0,0.9},{3.0,0.9}};
    case Style::HipHop:
      return {{0.0,2.5}};
    case Style::LoFi:
    case Style::Pop:
    case Style::Rock:
    default:
      return {{0.0,4.0}};
  }
}

} // namespace MidiForge::Content
