#include "Progressions.h"
#include "../Core/MusicTheory.h"
#include "../Core/RandomGen.h"

namespace MidiForge::Content {

static ChordSlot slot(int degree, int rootOffset = 0, const char* quality = "auto", bool seventh = false) {
  ChordSlot s;
  s.degree = degree;
  s.rootOffset = rootOffset;
  s.quality = quality;
  s.seventh = seventh;
  return s;
}

const std::vector<Progression>& allProgressions() {
  static const std::vector<Progression> progs = {
    // ---- Pop (major) ----
    {"Pop Four Chords", "I - V - vi - IV", Style::Pop, ScaleType::Major,
      {slot(1), slot(5), slot(6), slot(4)}},
    {"Sensitive", "vi - IV - I - V", Style::Pop, ScaleType::Major,
      {slot(6), slot(4), slot(1), slot(5)}},
    {"Doo Wop", "I - vi - IV - V", Style::Pop, ScaleType::Major,
      {slot(1), slot(6), slot(4), slot(5)}},
    {"Pop Rock Lift", "I - bVII - IV - I", Style::Pop, ScaleType::Major,
      {slot(1), slot(7, -1, "maj"), slot(4), slot(1)}},
    // ---- Pop (minor) ----
    {"Pop Minor Drive", "i - VI - III - VII", Style::Pop, ScaleType::MinorNatural,
      {slot(1), slot(6), slot(3), slot(7)}},
    // ---- LoFi ----
    {"LoFi Turnaround", "Imaj7 - vim7 - iim7 - V7", Style::LoFi, ScaleType::Major,
      {slot(1, 0, "auto", true), slot(6, 0, "auto", true), slot(2, 0, "auto", true), slot(5, 0, "auto", true)}},
    {"LoFi Minor Haze", "im7 - ivm7 - VImaj7 - V7", Style::LoFi, ScaleType::MinorNatural,
      {slot(1, 0, "auto", true), slot(4, 0, "auto", true), slot(6, 0, "auto", true), slot(5, 0, "auto", true)}},
    // ---- House ----
    {"House Minor", "i - VI - III - VII", Style::House, ScaleType::MinorNatural,
      {slot(1), slot(6), slot(3), slot(7)}},
    {"House Dark", "i - iv - VI - V", Style::House, ScaleType::MinorNatural,
      {slot(1), slot(4), slot(6), slot(5)}},
    // ---- HipHop ----
    {"Boom Bap Loop", "i - iv - i - iv", Style::HipHop, ScaleType::MinorNatural,
      {slot(1), slot(4), slot(1), slot(4)}},
    {"Trap Wave", "i - VII - VI - VII", Style::HipHop, ScaleType::MinorNatural,
      {slot(1), slot(7), slot(6), slot(7)}},
    {"HipHop Soul", "iv - V - i - VI", Style::HipHop, ScaleType::MinorNatural,
      {slot(4), slot(5), slot(1), slot(6)}},
    // ---- EDM ----
    {"EDM Euphoric", "V - vi - IV - I", Style::EDM, ScaleType::Major,
      {slot(5), slot(6), slot(4), slot(1)}},
    {"EDM Minor Lift", "VI - VII - i - i", Style::EDM, ScaleType::MinorNatural,
      {slot(6), slot(7), slot(1), slot(1)}},
    // ---- Rock ----
    {"Rock Classic", "I - bVII - IV - I", Style::Rock, ScaleType::Major,
      {slot(1), slot(7, -1, "maj"), slot(4), slot(1)}},
    {"Rock Minor Grind", "i - bVII - bVI - bVII", Style::Rock, ScaleType::MinorNatural,
      {slot(1), slot(7, -1, "maj"), slot(6, -1, "maj"), slot(7, -1, "maj")}},
  };
  return progs;
}

std::vector<int> progressionIndicesFor(Style style, ScaleType chordScale) {
  const auto& progs = allProgressions();
  std::vector<int> out;
  for (size_t i = 0; i < progs.size(); ++i) {
    if (progs[i].style == style && progs[i].scale == chordScale) out.push_back((int)i);
  }
  if (!out.empty()) return out;
  // Fallback: same scale family, any style.
  for (size_t i = 0; i < progs.size(); ++i) {
    if (progs[i].scale == chordScale) out.push_back((int)i);
  }
  if (!out.empty()) return out;
  for (size_t i = 0; i < progs.size(); ++i) out.push_back((int)i);
  return out;
}

std::vector<RealizedChord> realizeProgression(const SessionIdea& idea) {
  std::vector<RealizedChord> out;
  const auto& progs = allProgressions();
  if (idea.progressionIdx < 0 || idea.progressionIdx >= (int)progs.size()) return out;
  const auto& prog = progs[(size_t)idea.progressionIdx];

  int bars = idea.bars > 0 ? idea.bars : 4;
  std::vector<int> prev;
  for (int bar = 0; bar < bars; ++bar) {
    const auto& s = prog.chords[(size_t)bar % prog.chords.size()];
    int semi = MusicTheory::degreeToSemitone(prog.scale, s.degree) + s.rootOffset;
    int rootMidi = 48 + idea.keyRoot + semi;
    // Keep roots in a singable C3-B3-ish band, then voice around it.
    while (rootMidi < 43) rootMidi += 12;
    while (rootMidi > 55) rootMidi -= 12;
    auto intervals = MusicTheory::triadIntervals(prog.scale, s.degree, s.quality, s.seventh);
    // Bright/juicy: add the 9th on non-diminished chords when brightness is high.
    {
      bool h3 = false, h6 = false, h7 = false;
      for (int iv : intervals) { if (iv == 3) h3 = true; if (iv == 6) h6 = true; if (iv == 7) h7 = true; }
      bool isDim = h3 && h6 && !h7;
      if (idea.brightness >= 0.75 && !isDim) intervals.push_back(14);
    }
    auto pitches = MusicTheory::realizeVoicing(rootMidi, intervals, prev);
    if (idea.brightness > 0.66 && !pitches.empty()) {
      int mx = pitches.front();
      for (int p : pitches) mx = juce::jmax(mx, p);
      if (mx + 12 <= 86) for (int& p : pitches) p += 12;
    }
    prev = pitches;
    RealizedChord rc;
    rc.rootMidi = rootMidi;
    rc.intervals = intervals;
    rc.pitches = pitches;
    rc.startBeat = (double)bar * 4.0;
    rc.beats = 4.0;
    rc.symbol = MusicTheory::chordSymbol(rootMidi, intervals);
    out.push_back(std::move(rc));
  }
  return out;
}

} // namespace MidiForge::Content
