#include "IdeaGenerator.h"
#include "ChordGenerator.h"
#include "MelodyGenerator.h"
#include "BassGenerator.h"
#include "DrumGenerator.h"
#include "../Core/MusicTheory.h"
#include "../Content/Progressions.h"
#include "../Content/Patterns.h"

namespace MidiForge {

static int64_t baseSeedOf(const SessionIdea& idea) {
  return (int64_t)idea.seed * 1000003 + (int64_t)idea.take * 7919 + 7;
}

void IdeaGenerator::pickAll(SessionIdea& idea, RandomGen& rng) {
  ScaleType fam = MusicTheory::isMinorFamily(idea.scale) ? ScaleType::MinorNatural : ScaleType::Major;

  auto progs = Content::progressionIndicesFor(idea.style, fam);
  idea.progressionIdx = progs[(size_t)rng.nextInt(0, (int)progs.size() - 1)];
  const auto& P = Content::allProgressions()[(size_t)idea.progressionIdx];
  idea.progressionDisplay = P.display;
  idea.progressionName = P.name;

  auto drums = Content::drumIndicesFor(idea.style);
  idea.drumPatternIdx = drums[(size_t)rng.nextInt(0, (int)drums.size() - 1)];
  idea.drumPatternName = Content::allDrumPatterns()[(size_t)idea.drumPatternIdx].name;

  auto bass = Content::bassIndicesFor(idea.style);
  idea.bassPatternIdx = bass[(size_t)rng.nextInt(0, (int)bass.size() - 1)];
  idea.bassPatternName = Content::allBassPatterns()[(size_t)idea.bassPatternIdx].name;

  idea.melodyMotifIdx = rng.nextInt(0, (int)Content::allMotifs().size() - 1);
}

static int pickDifferent(const std::vector<int>& cands, int current, RandomGen& rng) {
  if (cands.empty()) return current;
  if (cands.size() == 1) return cands[0];
  int pick = cands[(size_t)rng.nextInt(0, (int)cands.size() - 1)];
  if (pick == current) {
    size_t pos = 0;
    for (size_t i = 0; i < cands.size(); ++i)
      if (cands[i] == current) { pos = i; break; }
    pick = cands[(pos + 1) % cands.size()];
  }
  return pick;
}

void IdeaGenerator::runAll(SessionIdea& idea, RandomGen& rng) {
  ChordGenerator cg; cg.generate(idea, rng);
  MelodyGenerator mg; mg.generate(idea, rng);
  BassGenerator bg; bg.generate(idea, rng);
  DrumGenerator dg; dg.generate(idea, rng);
}

void IdeaGenerator::generateFull(SessionIdea& idea) {
  idea.tracks.clear();
  RandomGen rng((int)(baseSeedOf(idea) % 2147483647));
  pickAll(idea, rng);
  runAll(idea, rng);
}

void IdeaGenerator::clearTrack(SessionIdea& idea, const juce::String& name) {
  for (auto it = idea.tracks.begin(); it != idea.tracks.end(); ) {
    if (it->name == name) it = idea.tracks.erase(it);
    else ++it;
  }
}

void IdeaGenerator::regenerateTrack(SessionIdea& idea, const juce::String& trackName, int seedOffset) {
  clearTrack(idea, trackName);
  int64_t rs = baseSeedOf(idea) + (int64_t)seedOffset * 104729 + trackName.hashCode();
  RandomGen rng((int)(((rs % 2147483647) + 2147483647) % 2147483647));
  ScaleType fam = MusicTheory::isMinorFamily(idea.scale) ? ScaleType::MinorNatural : ScaleType::Major;

  if (trackName == "Chords") {
    auto progs = Content::progressionIndicesFor(idea.style, fam);
    idea.progressionIdx = pickDifferent(progs, idea.progressionIdx, rng);
    const auto& P = Content::allProgressions()[(size_t)idea.progressionIdx];
    idea.progressionDisplay = P.display;
    idea.progressionName = P.name;
    ChordGenerator cg; cg.generate(idea, rng);
    // Bass follows chord roots, so refresh it to stay in key.
    clearTrack(idea, "Bass");
    BassGenerator bg; bg.generate(idea, rng);
  } else if (trackName == "Melody") {
    int n = (int)Content::allMotifs().size();
    int pick = rng.nextInt(0, n - 1);
    if (pick == idea.melodyMotifIdx && n > 1) pick = (pick + 1) % n;
    idea.melodyMotifIdx = pick;
    MelodyGenerator mg; mg.generate(idea, rng);
  } else if (trackName == "Bass") {
    auto bass = Content::bassIndicesFor(idea.style);
    idea.bassPatternIdx = pickDifferent(bass, idea.bassPatternIdx, rng);
    idea.bassPatternName = Content::allBassPatterns()[(size_t)idea.bassPatternIdx].name;
    BassGenerator bg; bg.generate(idea, rng);
  } else if (trackName == "Drums") {
    auto drums = Content::drumIndicesFor(idea.style);
    idea.drumPatternIdx = pickDifferent(drums, idea.drumPatternIdx, rng);
    idea.drumPatternName = Content::allDrumPatterns()[(size_t)idea.drumPatternIdx].name;
    DrumGenerator dg; dg.generate(idea, rng);
  }
}

} // namespace MidiForge
