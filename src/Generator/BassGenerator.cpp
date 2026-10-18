#include "BassGenerator.h"
#include "../Core/MusicTheory.h"
#include "../Content/Progressions.h"
#include "../Content/Patterns.h"

namespace MidiForge {

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

  for (const auto& ch : chords) {
    int rootLow = ch.rootMidi - 12;
    for (const auto& h : pat.hits) {
      double start = ch.startBeat + (double)h.step * 0.25;
      if (idea.swing > 0.0 && (h.step % 2 == 1)) start += (idea.swing / 100.0) * 0.15;
      int pitch = rootLow + h.interval;
      while (pitch < 24) pitch += 12;
      while (pitch > 60) pitch -= 12;
      int vel = juce::jlimit(1, 127, h.velocity + rng.nextInt(-6, 6));
      t.notes.push_back({pitch, start, h.lenBeats, vel, 3});
    }
  }
  idea.tracks.push_back(std::move(t));
}

} // namespace MidiForge
