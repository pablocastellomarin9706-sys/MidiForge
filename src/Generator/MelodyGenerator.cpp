#include "MelodyGenerator.h"
#include "../Core/MusicTheory.h"
#include "../Content/Progressions.h"
#include "../Content/Patterns.h"
#include <cmath>
#include <cstdlib>
#include <vector>

namespace MidiForge {

// Scale membership lookup, indexed by pitch class (0-11).
static std::vector<char> scaleMask(const std::vector<int>& scalePcs) {
  std::vector<char> mask(12, 0);
  for (int pc : scalePcs) mask[(size_t)(((pc % 12) + 12) % 12)] = 1;
  return mask;
}

// Nearest chord tone that is ALSO inside the idea's scale. Progressions may
// borrow colour (bVII in a major key, major V in a minor key, added 9ths), and
// snapping the lead onto those tones is what used to push melodies out of key.
// Out-of-scale chord tones are simply skipped now.
static int nearestChordTone(int pitch, const std::vector<int>& chordPitches,
                            const std::vector<char>& mask) {
  if (chordPitches.empty()) return pitch;
  int best = -1;
  int bestDist = 99999;
  for (int cp : chordPitches) {
    for (int oct = -2; oct <= 2; ++oct) {
      const int cand = cp + oct * 12;
      if (cand < 24 || cand > 108) continue;
      if (!mask[(size_t)(((cand % 12) + 12) % 12)]) continue;
      const int d = std::abs(cand - pitch);
      if (d < bestDist) { bestDist = d; best = cand; }
    }
  }
  return best < 0 ? pitch : best;
}

// Safety net: pull any out-of-scale pitch to the closest scale degree. Prefers
// moving down, which keeps the shape of the line.
static int snapToScale(int pitch, const std::vector<char>& mask) {
  const int pc = ((pitch % 12) + 12) % 12;
  if (mask[(size_t)pc]) return pitch;
  for (int d = 1; d <= 6; ++d) {
    const int down = ((pc - d) % 12 + 12) % 12;
    if (mask[(size_t)down]) return pitch - d;
    const int up = (pc + d) % 12;
    if (mask[(size_t)up]) return pitch + d;
  }
  return pitch;
}

void MelodyGenerator::generate(SessionIdea& idea, RandomGen& rng) {
  auto scalePcs = MusicTheory::getScale(idea.scale, idea.keyRoot);
  if (scalePcs.empty()) return;
  auto chords = Content::realizeProgression(idea);
  if (chords.empty()) return;
  const auto& motifs = Content::allMotifs();
  if (motifs.empty()) return;

  const auto mask = scaleMask(scalePcs);

  // Each genre leans on its own rhythmic cells, and the lead walks through the
  // pool bar by bar so a 4-bar idea is not one cell copy-pasted.
  auto pool = Content::motifIndicesFor(idea.style);
  if (pool.empty()) pool.push_back(0);
  for (int idx : pool)
    if (idx < 0 || idx >= (int)motifs.size()) { pool.clear(); pool.push_back(0); break; }

  if (idea.melodyMotifIdx < 0 || idea.melodyMotifIdx >= (int)motifs.size())
    idea.melodyMotifIdx = pool[0];
  int pos = 0;
  for (size_t i = 0; i < pool.size(); ++i) {
    if (pool[i] == idea.melodyMotifIdx) { pos = (int)i; break; }
  }
  if (pool[(size_t)pos] != idea.melodyMotifIdx) idea.melodyMotifIdx = pool[(size_t)pos];

  TrackIdea t;
  t.name = "Melody";
  t.channel = 2;
  t.colour = juce::Colour(92, 124, 255);
  t.detail = motifs[(size_t)pool[(size_t)pos]].name;

  int bars = idea.bars > 0 ? idea.bars : 4;
  int n = (int)scalePcs.size();
  int deg = rng.nextInt(0, n - 1);
  const int stepChoices[] = {-2, -1, -1, 0, 1, 1, 2, 3};

  for (int bar = 0; bar < bars; ++bar) {
    const auto& motif = motifs[(size_t)pool[(size_t)((pos + bar) % (int)pool.size())]];
    const auto& ch = chords[(size_t)bar % chords.size()];
    for (size_t ni = 0; ni < motif.notes.size(); ++ni) {
      double off = motif.notes[ni].first;
      double len = motif.notes[ni].second;

      deg += stepChoices[rng.nextInt(0, 7)];
      deg = ((deg % n) + n) % n;
      // getScale already folds keyRoot into these pitch classes; adding it
      // again transposed every non-C key (X-1, the whole melody). 60 + pc keeps
      // the lead centred at C5 (FL's midi 60 = C5) for any key.
      int pitch = 60 + scalePcs[(size_t)deg];

      bool phraseStart = (off == 0.0);
      bool lastNote = (bar == bars - 1 && ni == motif.notes.size() - 1);
      if (phraseStart || lastNote) pitch = nearestChordTone(pitch, ch.pitches, mask);

      // Never leave the chosen scale, then keep the lead in its C5-ish band
      // (octave moves preserve the pitch class, so the scale check still holds).
      pitch = snapToScale(pitch, mask);
      while (pitch < 60) pitch += 12;
      while (pitch > 86) pitch -= 12;

      double start = ch.startBeat + off;
      if (idea.swing > 0.0) {
        double frac = start - std::floor(start);
        if (std::abs(frac - 0.5) < 0.01) start += (idea.swing / 100.0) * 0.15;
      }

      int vel = 80 + (int)(idea.brightness * 18.0) + rng.nextInt(-10, 10);
      if (phraseStart) vel += 10;
      vel = juce::jlimit(1, 127, vel);
      t.notes.push_back({pitch, start, len, vel, 2});
    }
  }
  idea.tracks.push_back(std::move(t));
}

} // namespace MidiForge
