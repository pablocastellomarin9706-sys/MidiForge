#include "IdeaGenerator.h"
#include "ChordGenerator.h"
#include "MelodyGenerator.h"
#include "BassGenerator.h"
#include "DrumGenerator.h"
#include "../Core/MusicTheory.h"
#include "../Content/Progressions.h"
#include "../Content/Patterns.h"
#include "../Content/Characters.h"

#include <algorithm>

namespace MidiForge {

static int64_t baseSeedOf(const SessionIdea& idea) {
  return (int64_t)idea.seed * 1000003 + (int64_t)idea.take * 7919 + 7;
}

// Keeps mute/solo state alive across regenerations (chat + Replace buttons).
static bool flagOf(const std::vector<TrackIdea>& tracks, const juce::String& name,
                   bool TrackIdea::*member) {
  for (const auto& t : tracks) if (t.name == name) return t.*member;
  return false;
}

static void restoreFlags(std::vector<TrackIdea>& tracks, const std::vector<TrackIdea>& oldTracks) {
  for (auto& t : tracks) {
    t.mute = flagOf(oldTracks, t.name, &TrackIdea::mute);
    t.solo = flagOf(oldTracks, t.name, &TrackIdea::solo);
  }
}

// Character DNA: human jitter, velocity spread, note length, ghost hats.
static void applyCharacterToTrack(SessionIdea& idea, TrackIdea& t, RandomGen& rng) {
  const auto P = Content::characterPresetOf(idea.character);
  if (!P.valid) return;

  const bool isDrums = (t.name == "Drums");
  const double jit = P.jitter * (isDrums ? 0.5 : 1.0);

  for (auto& n : t.notes) {
    if (jit > 0.0) {
      const double j = jit * (double)rng.nextInt(-100, 100) / 100.0;
      n.start = juce::jmax(0.0, n.start + j);
    }
    n.velocity = juce::jlimit(1, 127, n.velocity + rng.nextInt(-P.velSigma, P.velSigma));
    n.length = juce::jmax(0.05, n.length * P.lenScale);
    if (t.name == "Melody" && P.octaveShift != 0)
      n.pitch = juce::jlimit(0, 127, n.pitch + 12 * P.octaveShift);
  }

  if (isDrums && P.ghostProb > 0.0) {
    const int bars = idea.bars > 0 ? idea.bars : 4;
    for (int bar = 0; bar < bars; ++bar) {
      for (int step = 0; step < 16; ++step) {
        if (step % 4 == 0) continue;
        const double start = (double)bar * 4.0 + (double)step * 0.25;
        bool occupied = false;
        for (const auto& n : t.notes) {
          if (n.start > start - 0.06 && n.start < start + 0.06) { occupied = true; break; }
        }
        if (occupied) continue;
        if (rng.nextDouble() < P.ghostProb * 0.5) {
          t.notes.push_back({Content::kHat, start, 0.08, 34 + rng.nextInt(0, 12), 10});
        }
      }
    }
  }

  std::sort(t.notes.begin(), t.notes.end(),
            [](const Note& a, const Note& b) { return a.start < b.start; });
}

static void applyCharacterPost(SessionIdea& idea, RandomGen& rng) {
  const auto P = Content::characterPresetOf(idea.character);
  if (!P.valid) return;
  for (auto& t : idea.tracks) applyCharacterToTrack(idea, t, rng);
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
  auto oldTracks = idea.tracks;
  idea.tracks.clear();
  RandomGen rng((int)(baseSeedOf(idea) % 2147483647));
  pickAll(idea, rng);
  runAll(idea, rng);
  applyCharacterPost(idea, rng);
  restoreFlags(idea.tracks, oldTracks);
}

void IdeaGenerator::clearTrack(SessionIdea& idea, const juce::String& name) {
  for (auto it = idea.tracks.begin(); it != idea.tracks.end(); ) {
    if (it->name == name) it = idea.tracks.erase(it);
    else ++it;
  }
}

void IdeaGenerator::regenerateTrack(SessionIdea& idea, const juce::String& trackName, int seedOffset) {
  auto oldTracks = idea.tracks;
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

  const bool chordsFamily = (trackName == "Chords");
  for (auto& t : idea.tracks)
    if (t.name == trackName || (chordsFamily && t.name == "Bass"))
      applyCharacterToTrack(idea, t, rng);
  restoreFlags(idea.tracks, oldTracks);
}

} // namespace MidiForge
