#include "BassGenerator.h"
#include "../Core/MusicTheory.h"
#include "../Content/Progressions.h"
#include "../Content/Patterns.h"

#include <algorithm>
#include <cmath>

namespace MidiForge {

namespace {

double swungStart(const SessionIdea& idea, double chordStart, int step) {
  double start = chordStart + (double)step * 0.25;
  if (idea.swing > 0.0 && (step % 2 == 1)) start += (idea.swing / 100.0) * 0.15;
  return start;
}

int clampBass(int pitch) {
  while (pitch < 24) pitch += 12;
  while (pitch > 60) pitch -= 12;
  return pitch;
}

void pushNote(TrackIdea& t, const SessionIdea& idea, double chordStart, int step, int pitch,
              double len, int vel, RandomGen& rng) {
  t.notes.push_back({clampBass(pitch), swungStart(idea, chordStart, step),
                     juce::jmax(0.1, len), juce::jlimit(1, 127, vel + rng.nextInt(-5, 5)), 3});
}

} // namespace

void BassGenerator::generate(SessionIdea& idea, RandomGen& rng) {
  const auto& pats = Content::allBassPatterns();
  if (idea.bassPatternIdx < 0 || idea.bassPatternIdx >= (int)pats.size()) return;
  const auto& pat = pats[(size_t)idea.bassPatternIdx];

  auto chords = Content::realizeProgression(idea);
  if (chords.empty()) return;

  TrackIdea t;
  t.name = "Bass";
  t.channel = 3;
  t.colour = juce::Colour(92, 255, 124);
  t.detail = pat.name;

  const int n = (int)chords.size();
  for (int ci = 0; ci < n; ++ci) {
    const auto& ch = chords[(size_t)ci];
    const int rootLow = ch.rootMidi - 12;
    const bool last = (ci == n - 1);
    const bool variation = (n > 1 && (ci % 2 == 1));

    for (const auto& h : pat.hits)
      pushNote(t, idea, ch.startBeat, h.step, rootLow + h.interval, h.lenBeats, h.velocity, rng);

    if (!variation) continue;

    // Odd bars get genre-specific movement, so the low end is not a carbon
    // copy of bar 1: 808 octave jumps, house pumps, lofi sevenths, rock fifths.
    const double p = rng.nextDouble();
    switch (idea.style) {
      case Style::HipHop:
        if (p < 0.45)      pushNote(t, idea, ch.startBeat, 14, rootLow + 12, 0.4, 96, rng);
        else if (p < 0.75) pushNote(t, idea, ch.startBeat, 11, rootLow, 0.4, 92, rng);
        else               pushNote(t, idea, ch.startBeat, 14, rootLow + 7, 0.4, 90, rng);
        break;
      case Style::House:
      case Style::EDM:
        if (p < 0.50)      pushNote(t, idea, ch.startBeat, 6, rootLow + 12, 0.4, 100, rng);
        else               pushNote(t, idea, ch.startBeat, 15, rootLow, 0.25, 98, rng);
        break;
      case Style::LoFi:
        if (p < 0.50)      pushNote(t, idea, ch.startBeat, 14, rootLow + 10, 0.5, 86, rng);
        else               pushNote(t, idea, ch.startBeat, 7, rootLow + 7, 0.5, 88, rng);
        break;
      case Style::Rock:
        if (p < 0.50)      pushNote(t, idea, ch.startBeat, 6, rootLow + 7, 0.5, 104, rng);
        else               pushNote(t, idea, ch.startBeat, 14, rootLow + 7, 0.5, 100, rng);
        break;
      case Style::Pop:
      default:
        if (p < 0.40)      pushNote(t, idea, ch.startBeat, 14, rootLow + 7, 0.5, 96, rng);
        else if (p < 0.70) pushNote(t, idea, ch.startBeat, 6, rootLow + 12, 0.5, 98, rng);
        else               pushNote(t, idea, ch.startBeat, 10, rootLow + 7, 0.5, 94, rng);
        break;
    }

    // Walking approach into the next chord root (kept short so it reads as a
    // pickup, not as a clash).
    if (!last && rng.nextDouble() < 0.35) {
      const int nextRoot = chords[(size_t)((ci + 1) % n)].rootMidi - 12;
      const int octaveUp = rng.nextDouble() < 0.4 ? 12 : 0;
      pushNote(t, idea, ch.startBeat, 15, nextRoot + octaveUp, 0.2, 92, rng);
    }
  }

  // 808 legato: overlap each note slightly with the next one so a mono 808 with
  // glide/portamento plays a real slide (the FL way) instead of stacked
  // staccato notes. Same pitch is skipped: retriggering would cut the tail.
  if (idea.style == Style::HipHop && t.notes.size() > 1) {
    std::stable_sort(t.notes.begin(), t.notes.end(),
                     [](const Note& a, const Note& b) { return a.start < b.start; });
    for (size_t i = 0; i + 1 < t.notes.size(); ++i) {
      auto& a = t.notes[i];
      const auto& b = t.notes[i + 1];
      const double gap = b.start - (a.start + a.length);
      const double step = std::abs(b.start - a.start);
      if (a.pitch != b.pitch && gap >= -0.001 && gap <= 0.30 && step <= 2.05)
        a.length += gap + 0.05;
    }
  }

  std::stable_sort(t.notes.begin(), t.notes.end(),
                   [](const Note& a, const Note& b) { return a.start < b.start; });
  idea.tracks.push_back(std::move(t));
}

} // namespace MidiForge
