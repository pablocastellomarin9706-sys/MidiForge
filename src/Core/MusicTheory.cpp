#include "MusicTheory.h"

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

} // namespace MidiForge::MusicTheory
