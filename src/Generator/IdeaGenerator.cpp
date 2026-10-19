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
  const bool isBass = (t.name == "Bass");
  const bool isMelody = (t.name == "Melody");
  const double jit = P.jitter * (isDrums ? 0.5 : 1.0);
  const double patternEnd = (double)(idea.bars > 0 ? idea.bars : 4) * 4.0;

  for (auto& n : t.notes) {
    if (jit > 0.0) {
      const double j = jit * (double)rng.nextInt(-100, 100) / 100.0;
      n.start = juce::jmax(0.0, n.start + j);
    }
    n.velocity = juce::jlimit(1, 127, n.velocity + rng.nextInt(-P.velSigma, P.velSigma));
    const double lenScale = isBass ? P.bassLenScale : P.lenScale;
    n.length = juce::jmax(0.05, n.length * lenScale);
    if (n.start + n.length > patternEnd)
      n.length = juce::jmax(0.05, patternEnd - n.start);
    if (isMelody && P.octaveShift != 0)
      n.pitch = juce::jlimit(0, 127, n.pitch + 12 * P.octaveShift);
  }

  // Air: thin out melody notes (Pi'erre-style space). Keeps at least 6 notes.
  if (isMelody && P.density < 1.0 && t.notes.size() > 8) {
    std::vector<Note> kept;
    kept.reserve(t.notes.size());
    for (size_t i = 0; i < t.notes.size(); ++i) {
      const bool keep = rng.nextDouble() <= P.density;
      if (!keep && kept.size() >= 6 && (i + 1) < t.notes.size()) continue;
      kept.push_back(t.notes[i]);
    }
    t.notes.swap(kept);
  }

  // Detroit "reverse + gap": per-bar retrograde of the pitch order plus a rest
  // right after the downbeat. Silence is part of the phrase (Insulation).
  if (isMelody && P.melodyReverse > 0.0) {
    const int bars = idea.bars > 0 ? idea.bars : 4;
    for (int bar = 0; bar < bars; ++bar) {
      if (rng.nextDouble() >= P.melodyReverse) continue;
      const double b0 = (double)bar * 4.0;
      const double b1 = b0 + 4.0;
      std::vector<std::size_t> ids;
      for (std::size_t i = 0; i < t.notes.size(); ++i)
        if (t.notes[i].start >= b0 && t.notes[i].start < b1) ids.push_back(i);
      if (ids.size() < 4) continue;

      // Retrograde: keep the rhythm grid, flip the pitch order.
      std::vector<int> pitches;
      pitches.reserve(ids.size());
      for (auto i : ids) pitches.push_back(t.notes[i].pitch);
      for (std::size_t k = 0; k < ids.size(); ++k)
        t.notes[ids[k]].pitch = pitches[ids.size() - 1 - k];

      // Gap: mute the note right after the downbeat (leave >= 3 notes).
      t.notes.erase(t.notes.begin() + (std::ptrdiff_t)ids[1]);
    }
  }

  // Detroit "doubling": a soft copy of the melody an octave below thickens the
  // line (see Insulation reference: melodias dobladas). Stays clear of the 808.
  if (isMelody && P.melodyOctaveDown && !t.notes.empty()) {
    std::vector<Note> layer;
    layer.reserve(t.notes.size());
    for (const auto& n : t.notes) {
      const int p = n.pitch - 12;
      if (p < 24) continue;
      Note d = n;
      d.pitch = p;
      d.velocity = juce::jlimit(1, 127, juce::roundToInt((double)n.velocity * 0.72));
      layer.push_back(d);
    }
    t.notes.insert(t.notes.end(), layer.begin(), layer.end());
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

  auto progs = Content::progressionIndicesFor(idea.style, fam, idea.character);
  idea.progressionIdx = progs[(size_t)rng.nextInt(0, (int)progs.size() - 1)];
  const auto& P = Content::allProgressions()[(size_t)idea.progressionIdx];
  idea.progressionDisplay = P.display;
  idea.progressionName = P.name;

  auto drums = Content::drumIndicesFor(idea.style, idea.character);
  idea.drumPatternIdx = drums[(size_t)rng.nextInt(0, (int)drums.size() - 1)];
  idea.drumPatternName = Content::allDrumPatterns()[(size_t)idea.drumPatternIdx].name;

  auto bass = Content::bassIndicesFor(idea.style, idea.character);
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
    auto progs = Content::progressionIndicesFor(idea.style, fam, idea.character);
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
    auto bass = Content::bassIndicesFor(idea.style, idea.character);
    idea.bassPatternIdx = pickDifferent(bass, idea.bassPatternIdx, rng);
    idea.bassPatternName = Content::allBassPatterns()[(size_t)idea.bassPatternIdx].name;
    BassGenerator bg; bg.generate(idea, rng);
  } else if (trackName == "Drums") {
    auto drums = Content::drumIndicesFor(idea.style, idea.character);
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

void IdeaGenerator::applyCreativityToTrack(TrackIdea& track, float amount, juce::Random& rng) {
  const float a = juce::jlimit(0.0f, 1.0f, amount);
  if (a <= 0.001f) return;

  const bool melodic = track.name.containsIgnoreCase("melod");
  const bool drums = track.name.containsIgnoreCase("drum");

  for (auto& n : track.notes) {
    // Velocity humanisation.
    const int vd = juce::roundToInt((rng.nextFloat() * 2.0f - 1.0f) * a * 20.0f);
    n.velocity = juce::jlimit(1, 127, n.velocity + vd);

    // Micro timing drift (kept out of the drum lane to protect the groove).
    if (!drums) {
      const double jitter = (double)(rng.nextFloat() * 2.0f - 1.0f) * (double)a * 0.03;
      n.start = juce::jmax(0.0, n.start + jitter);
    }

    // Occasional octave sparkle on the melody.
    if (melodic && rng.nextFloat() < a * 0.15f) {
      const int p = n.pitch + (rng.nextBool() ? 12 : -12);
      if (p >= 24 && p <= 108) n.pitch = p;
    }
  }
}

void IdeaGenerator::applyCreativity(SessionIdea& idea, float amount, int seedSalt) {
  if (amount <= 0.001f) return;
  juce::Random rng(idea.seed * 7919 + seedSalt * 104729 + idea.take * 31 + 17);
  for (auto& t : idea.tracks)
    applyCreativityToTrack(t, amount, rng);
}

} // namespace MidiForge
