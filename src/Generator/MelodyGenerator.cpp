#include "MelodyGenerator.h"
#include "../Core/MusicTheory.h"
#include "../Content/Progressions.h"
#include "../Content/Patterns.h"
#include <cmath>
#include <cstdlib>

namespace MidiForge {

static int nearestChordTone(int pitch, const std::vector<int>& chordPitches) {
  if (chordPitches.empty()) return pitch;
  int best = pitch;
  int bestDist = 99999;
  for (int cp : chordPitches) {
    for (int oct = -2; oct <= 2; ++oct) {
      int cand = cp + oct * 12;
      int d = std::abs(cand - pitch);
      if (d < bestDist) { bestDist = d; best = cand; }
    }
  }
  return best;
}

void MelodyGenerator::generate(SessionIdea& idea, RandomGen& rng) {
  auto scalePcs = MusicTheory::getScale(idea.scale, idea.keyRoot);
  if (scalePcs.empty()) return;
  auto chords = Content::realizeProgression(idea);
  if (chords.empty()) return;
  const auto& motifs = Content::allMotifs();
  if (motifs.empty()) return;
  if (idea.melodyMotifIdx < 0) idea.melodyMotifIdx = 0;
  if (idea.melodyMotifIdx >= (int)motifs.size()) idea.melodyMotifIdx %= (int)motifs.size();

  TrackIdea t;
  t.name = "Melody";
  t.channel = 2;
  t.colour = juce::Colour(92, 124, 255);
  t.detail = motifs[(size_t)idea.melodyMotifIdx % motifs.size()].name;

  int bars = idea.bars > 0 ? idea.bars : 4;
  int n = (int)scalePcs.size();
  int deg = rng.nextInt(0, n - 1);
  const int stepChoices[] = {-2, -1, -1, 0, 1, 1, 2, 3};

  for (int bar = 0; bar < bars; ++bar) {
    const auto& motif = motifs[(size_t)(idea.melodyMotifIdx + bar) % motifs.size()];
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
      if (phraseStart || lastNote) pitch = nearestChordTone(pitch, ch.pitches);

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
