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
    // ---- Character-specific (HipHop family) ----
    // Detroit: minor with a bright major V (harmonic-minor colour).
    {"Detroit Minor", "i - VI - iv - V", Style::HipHop, ScaleType::MinorNatural,
      {slot(1), slot(6), slot(4), slot(5, 0, "maj")}, Character::Detroit},
    {"Detroit Dark", "i - VII - VI - VII", Style::HipHop, ScaleType::MinorNatural,
      {slot(1), slot(7), slot(6), slot(7)}, Character::Detroit},
    // Nu-Jerk: droning, repetitive, hazy.
    {"Jerk Drone", "i - i - VI - VII", Style::HipHop, ScaleType::MinorNatural,
      {slot(1), slot(1), slot(6), slot(7)}, Character::NuJerk},
    {"Jerk Haze", "i - v - VI - VII", Style::HipHop, ScaleType::MinorNatural,
      {slot(1), slot(5), slot(6), slot(7)}, Character::NuJerk},
    {"Jerk Hook", "im7 - ivm7 - VImaj7 - V7", Style::HipHop, ScaleType::MinorNatural,
      {slot(1, 0, "auto", true), slot(4, 0, "auto", true), slot(6, 0, "maj", true),
       slot(5, 0, "maj", true)}, Character::NuJerk},
    // Trap Oscuro.
    {"Dark Trap", "i - VI - VII - v", Style::HipHop, ScaleType::MinorNatural,
      {slot(1), slot(6), slot(7), slot(5)}, Character::TrapDark},
    {"Trap Noir", "i - VII - VI - v", Style::HipHop, ScaleType::MinorNatural,
      {slot(1), slot(7), slot(6), slot(5)}, Character::TrapDark},
    // Plugg: airy Pi'erre-style motion.
    {"Plugg Space", "i - v - VI - iv", Style::HipHop, ScaleType::MinorNatural,
      {slot(1), slot(5), slot(6), slot(4)}, Character::Plugg},
    {"Plugg Float", "i - III - VI - iv", Style::HipHop, ScaleType::MinorNatural,
      {slot(1), slot(3), slot(6), slot(4)}, Character::Plugg},
    {"Plugg Bells", "im7 - VImaj7 - IIImaj7 - VII", Style::HipHop, ScaleType::MinorNatural,
      {slot(1, 0, "auto", true), slot(6, 0, "maj", true), slot(3, 0, "maj", true),
       slot(7)}, Character::Plugg},
    // Rage.
    {"Rage Lift", "i - VI - III - VII", Style::HipHop, ScaleType::MinorNatural,
      {slot(1), slot(6), slot(3), slot(7)}, Character::Rage},
    {"Rage Charge", "i - VII - VI - VII", Style::HipHop, ScaleType::MinorNatural,
      {slot(1), slot(7), slot(6), slot(7)}, Character::Rage},
    // Boom Bap: soulful diatonic sevenths (Lord Finesse / Cookin Soul school).
    {"BoomBap Soul", "im7 - ivm7 - VImaj7 - IIImaj7", Style::HipHop, ScaleType::MinorNatural,
      {slot(1, 0, "auto", true), slot(4, 0, "auto", true), slot(6, 0, "maj", true),
       slot(3, 0, "maj", true)}, Character::BoomBap},
    // Trap Latino: major V + dembow-friendly loop.
    {"Latino Minor", "i - VII - VI - V", Style::HipHop, ScaleType::MinorNatural,
      {slot(1), slot(7), slot(6), slot(5, 0, "maj")}, Character::TrapLatino},
    {"Latino Dembow", "i - iv - VII - i", Style::HipHop, ScaleType::MinorNatural,
      {slot(1), slot(4), slot(7), slot(1)}, Character::TrapLatino},
  };
  return progs;
}

std::vector<int> progressionIndicesFor(Style style, ScaleType chordScale, Character character) {
  const auto& progs = allProgressions();
  std::vector<int> out;
  // 1) Dedicated progressions for this character in this style + scale.
  if (character != Character::None) {
    for (size_t i = 0; i < progs.size(); ++i)
      if (progs[i].style == style && progs[i].scale == chordScale &&
          progs[i].character == character) out.push_back((int)i);
    if (!out.empty()) return out;
  }
  // 2) Generic progressions of this style + scale.
  for (size_t i = 0; i < progs.size(); ++i)
    if (progs[i].style == style && progs[i].scale == chordScale &&
        progs[i].character == Character::None) out.push_back((int)i);
  if (!out.empty()) return out;
  // 3) Fallback: same scale family, any style.
  const bool minor = MusicTheory::isMinorFamily(chordScale);
  for (size_t i = 0; i < progs.size(); ++i)
    if (MusicTheory::isMinorFamily(progs[i].scale) == minor) out.push_back((int)i);
  if (!out.empty()) return out;
  // 4) Anything.
  for (size_t i = 0; i < progs.size(); ++i) out.push_back((int)i);
  return out;
}

// Diatonic chord built by stacking scale thirds. Unlike triadIntervals (which
// only knows the generic major/minor template), this respects the real mode:
// Dorian gives IV major, Mixolydian gives bVII major, harmonic minor gives a
// diminished seventh on VII... Every chord tone is guaranteed to be in scale.
static std::vector<int> strictDiatonicChord(const std::vector<int>& scalePcs,
                                            int degree, bool seventh) {
  const int n = (int)scalePcs.size();
  if (n < 7 || degree < 1)
    return seventh ? std::vector<int>{0, 4, 7, 10} : std::vector<int>{0, 4, 7};

  auto tone = [&](int step) {
    const int idx = degree - 1 + step;
    const int oct = idx / n;
    return scalePcs[(size_t)(idx % n)] + oct * 12;
  };
  const int root = tone(0);
  std::vector<int> intervals{0, tone(2) - root, tone(4) - root};
  if (seventh) intervals.push_back(tone(6) - root);
  return intervals;
}

std::vector<RealizedChord> realizeProgression(const SessionIdea& idea) {
  std::vector<RealizedChord> out;
  const auto& progs = allProgressions();
  if (idea.progressionIdx < 0 || idea.progressionIdx >= (int)progs.size()) return out;
  const auto& prog = progs[(size_t)idea.progressionIdx];

  // Strict scale mode: chords are built from the chosen scale (stacked scale
  // thirds) and borrowed roots/qualities are ignored. Pentatonic and blues use
  // their parent 7-note scale for the harmony; the melody still uses the
  // 5-note scale, which is a consonant subset of it.
  const bool strict = idea.strictScale;
  ScaleType chordScale = prog.scale;
  if (strict) {
    const auto chosen = MusicTheory::getScale(idea.scale, 0);
    if (chosen.size() >= 7) chordScale = idea.scale;
    else chordScale = MusicTheory::isMinorFamily(idea.scale) ? ScaleType::MinorNatural
                                                             : ScaleType::Major;
  }
  const auto chordPcs = MusicTheory::getScale(chordScale, 0);
  auto pcInScale = [&chordPcs](int midi) {
    const int pc = ((midi % 12) + 12) % 12;
    for (int s : chordPcs) if (s == pc) return true;
    return false;
  };

  int bars = idea.bars > 0 ? idea.bars : 4;
  std::vector<int> prev;
  for (int bar = 0; bar < bars; ++bar) {
    const auto& s = prog.chords[(size_t)bar % prog.chords.size()];
    int semi = MusicTheory::degreeToSemitone(chordScale, s.degree) + (strict ? 0 : s.rootOffset);
    int rootMidi = 48 + idea.keyRoot + semi;
    // Keep roots in a singable C3-B3-ish band, then voice around it.
    while (rootMidi < 43) rootMidi += 12;
    while (rootMidi > 55) rootMidi -= 12;
    auto intervals = strict
        ? strictDiatonicChord(chordPcs, s.degree, s.seventh)
        : MusicTheory::triadIntervals(prog.scale, s.degree, s.quality, s.seventh);
    // Bright/juicy: add the 9th on non-diminished chords when brightness is high.
    {
      bool h3 = false, h6 = false, h7 = false;
      for (int iv : intervals) { if (iv == 3) h3 = true; if (iv == 6) h6 = true; if (iv == 7) h7 = true; }
      bool isDim = h3 && h6 && !h7;
      if (idea.brightness >= 0.75 && !isDim && (!strict || pcInScale(rootMidi + 14)))
        intervals.push_back(14);
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
