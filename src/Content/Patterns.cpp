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

std::vector<int> drumIndicesFor(Style style) {
  const auto& pats = allDrumPatterns();
  std::vector<int> out;
  for (size_t i = 0; i < pats.size(); ++i)
    if (pats[i].style == style) out.push_back((int)i);
  if (!out.empty()) return out;
  for (size_t i = 0; i < pats.size(); ++i) out.push_back((int)i);
  return out;
}

std::vector<int> bassIndicesFor(Style style) {
  const auto& pats = allBassPatterns();
  std::vector<int> out;
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
