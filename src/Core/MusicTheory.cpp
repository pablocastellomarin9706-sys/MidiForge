#include "MusicTheory.h"
#include <algorithm>
#include <cmath>

namespace MidiForge::MusicTheory {

static const int majorScale[] = {0,2,4,5,7,9,11};
static const int minorNatural[] = {0,2,3,5,7,8,10};
static const int minorHarmonic[] = {0,2,3,5,7,8,11};
static const int minorMelodic[] = {0,2,3,5,7,9,11};
static const int dorian[] = {0,2,3,5,7,9,10};
static const int phrygian[] = {0,1,3,5,7,8,10};
static const int lydian[] = {0,2,4,6,7,9,11};
static const int mixolydian[] = {0,2,4,5,7,9,10};
static const int aeolian[] = {0,2,3,5,7,8,10};
static const int locrian[] = {0,1,3,5,6,8,10};
static const int pentMajor[] = {0,2,4,7,9};
static const int pentMinor[] = {0,3,5,7,10};
static const int blues[] = {0,3,5,6,7,10};
static const int chromatic[] = {0,1,2,3,4,5,6,7,8,9,10,11};

std::vector<int> getScale(ScaleType type, int root) {
  std::vector<int> intervals;
  switch(type) {
    case ScaleType::Major: for(int i: majorScale) intervals.push_back((root+i)%12); break;
    case ScaleType::MinorNatural: for(int i: minorNatural) intervals.push_back((root+i)%12); break;
    case ScaleType::MinorHarmonic: for(int i: minorHarmonic) intervals.push_back((root+i)%12); break;
    case ScaleType::MinorMelodic: for(int i: minorMelodic) intervals.push_back((root+i)%12); break;
    case ScaleType::Dorian: for(int i: dorian) intervals.push_back((root+i)%12); break;
    case ScaleType::Phrygian: for(int i: phrygian) intervals.push_back((root+i)%12); break;
    case ScaleType::Lydian: for(int i: lydian) intervals.push_back((root+i)%12); break;
    case ScaleType::Mixolydian: for(int i: mixolydian) intervals.push_back((root+i)%12); break;
    case ScaleType::Aeolian: for(int i: aeolian) intervals.push_back((root+i)%12); break;
    case ScaleType::Locrian: for(int i: locrian) intervals.push_back((root+i)%12); break;
    case ScaleType::PentatonicMajor: for(int i: pentMajor) intervals.push_back((root+i)%12); break;
    case ScaleType::PentatonicMinor: for(int i: pentMinor) intervals.push_back((root+i)%12); break;
    case ScaleType::Blues: for(int i: blues) intervals.push_back((root+i)%12); break;
    case ScaleType::Chromatic: for(int i: chromatic) intervals.push_back((root+i)%12); break;
  }
  return intervals;
}

std::vector<int> getChordPitches(int root, const std::vector<int>& intervals, const std::vector<int>& scale) {
  (void)scale;
  std::vector<int> pitches;
  for(int iv: intervals) pitches.push_back(root + iv);
  return pitches;
}

std::vector<ChordDef> getCommonProgressions() {
  std::vector<ChordDef> defs;
  // triads simplified
  defs.push_back({"maj",{0,4,7}});
  defs.push_back({"min",{0,3,7}});
  defs.push_back({"dim",{0,3,6}});
  defs.push_back({"aug",{0,4,8}});
  defs.push_back({"sus2",{0,2,7}});
  defs.push_back({"sus4",{0,5,7}});
  return defs;
}

int degreeToSemitone(ScaleType scale, int degree) {
  if (degree < 1) degree = 1;
  if (degree > 7) degree = 7;
  static const int major[7] = {0,2,4,5,7,9,11};
  static const int minorNat[7] = {0,2,3,5,7,8,10};
  if (scale == ScaleType::MinorNatural || scale == ScaleType::MinorHarmonic ||
      scale == ScaleType::MinorMelodic || scale == ScaleType::Aeolian ||
      scale == ScaleType::Dorian || scale == ScaleType::Phrygian ||
      scale == ScaleType::Locrian) {
    return minorNat[degree - 1];
  }
  return major[degree - 1];
}

static bool containsInt(const std::vector<int>& v, int x) {
  for (int i : v) if (i == x) return true;
  return false;
}

std::vector<int> triadIntervals(ScaleType scale, int degree,
                                const juce::String& qualityOverride, bool seventh) {
  std::vector<int> tri;
  const juce::String q = qualityOverride;
  if (q == "maj") tri = {0,4,7};
  else if (q == "min") tri = {0,3,7};
  else if (q == "dim") tri = {0,3,6};
  else {
    bool minor = (scale == ScaleType::MinorNatural || scale == ScaleType::MinorHarmonic ||
                  scale == ScaleType::MinorMelodic || scale == ScaleType::Aeolian ||
                  scale == ScaleType::Dorian || scale == ScaleType::Phrygian ||
                  scale == ScaleType::Locrian);
    if (!minor) {
      switch (degree) {
        case 1: case 4: tri = {0,4,7}; break;
        case 2: case 3: case 6: tri = {0,3,7}; break;
        case 5: tri = {0,4,7}; break;
        case 7: tri = {0,3,6}; break;
        default: tri = {0,4,7}; break;
      }
    } else {
      switch (degree) {
        case 1: case 4: case 5: tri = {0,3,7}; break;
        case 2: tri = {0,3,6}; break;
        case 3: case 6: case 7: tri = {0,4,7}; break;
        default: tri = {0,3,7}; break;
      }
    }
  }

  if (seventh) {
    int sev = 10;
    bool isMajTriad = containsInt(tri, 4) && containsInt(tri, 7);
    if (isMajTriad) {
      if (q == "maj") sev = (degree == 1 || degree == 3 || degree == 4 || degree == 6) ? 11 : 10;
      else sev = (degree == 1 || degree == 4) ? 11 : 10;
    }
    tri.push_back(sev);
  }
  return tri;
}

bool isMinorFamily(ScaleType type) {
  switch (type) {
    case ScaleType::MinorNatural:
    case ScaleType::MinorHarmonic:
    case ScaleType::MinorMelodic:
    case ScaleType::Aeolian:
    case ScaleType::Dorian:
    case ScaleType::Phrygian:
    case ScaleType::Locrian:
    case ScaleType::PentatonicMinor:
    case ScaleType::Blues:
      return true;
    default:
      return false;
  }
}

juce::String pitchClassName(int midi) {
  static const char* names[12] = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
  int pc = ((midi % 12) + 12) % 12;
  return names[pc];
}

juce::String chordSymbol(int rootMidi, const std::vector<int>& intervals) {
  juce::String root = pitchClassName(rootMidi);
  bool h3 = containsInt(intervals, 3), h4 = containsInt(intervals, 4);
  bool h6 = containsInt(intervals, 6), h7 = containsInt(intervals, 7), h8 = containsInt(intervals, 8);
  bool h10 = containsInt(intervals, 10), h11 = containsInt(intervals, 11), h14 = containsInt(intervals, 14);
  juce::String base;
  if (h4 && h7) base = "";
  else if (h3 && h7) base = "m";
  else if (h3 && h6) base = "dim";
  else if (h4 && h8) base = "aug";
  else base = "?";

  juce::String suffix = base;
  if (h11) suffix = (base == "" ? "maj7" : (base == "m" ? "m(maj7)" : base));
  else if (h10) {
    if (base == "") suffix = "7";
    else if (base == "m") suffix = "m7";
    else if (base == "dim") suffix = "m7b5";
  }
  if (h14) {
    if (suffix == "m7") suffix = "m9";
    else if (suffix == "maj7") suffix = "maj9";
    else if (suffix == "7") suffix = "9";
    else suffix = suffix + "(add9)";
  }
  return root + suffix;
}

std::vector<int> realizeVoicing(int rootMidi, const std::vector<int>& intervals,
                                const std::vector<int>& prevPitches) {
  std::vector<int> best;
  if (intervals.empty()) return best;
  int n = (int)intervals.size();

  auto avgOf = [](const std::vector<int>& v) {
    if (v.empty()) return 60.0;
    double s = 0.0;
    for (int p : v) s += (double)p;
    return s / (double)v.size();
  };
  double target = prevPitches.empty() ? 60.0 : avgOf(prevPitches);

  double bestCost = 1e9;
  for (int inv = 0; inv < n; ++inv) {
    std::vector<int> v;
    v.reserve((size_t)n);
    for (int k = 0; k < n; ++k) {
      int idx = (inv + k) % n;
      int oct = (inv + k) >= n ? 12 : 0;
      v.push_back(rootMidi + intervals[(size_t)idx] + oct);
    }
    int guard = 0;
    while (!v.empty() && v.front() < 48 && guard++ < 8)
      for (int& p : v) p += 12;
    guard = 0;
    while (!v.empty() && v.back() > 72 && v.front() - 12 >= 36 && guard++ < 8)
      for (int& p : v) p -= 12;

    double cost = std::abs(avgOf(v) - target);
    if (cost < bestCost) { bestCost = cost; best = v; }
  }
  std::sort(best.begin(), best.end());
  return best;
}

} // namespace MidiForge::MusicTheory
