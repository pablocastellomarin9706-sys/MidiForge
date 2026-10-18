#include "ChordGenerator.h"
#include "../Core/MusicTheory.h"
#include "../Content/Progressions.h"
#include "../Content/Patterns.h"

namespace MidiForge {

void ChordGenerator::generate(SessionIdea& idea, RandomGen& rng) {
  auto chords = Content::realizeProgression(idea);
  if (chords.empty()) return;

  TrackIdea t;
  t.name = "Chords";
  t.channel = 1;
  t.colour = juce::Colour(124, 92, 255);
  t.detail = idea.progressionDisplay + "  -  " + idea.progressionName;

  auto rhythm = Content::chordRhythmFor(idea.style);

  for (const auto& ch : chords) {
    if (ch.pitches.empty()) continue;
    t.chordSymbols.push_back(ch.symbol);
    for (const auto& hit : rhythm) {
      double start = ch.startBeat + hit.first;
      double len = hit.second;
      for (size_t i = 0; i < ch.pitches.size(); ++i) {
        double so = 0.0;
        if (idea.strum > 0.0) {
          double step = idea.strum * 0.25;
          if (idea.strumDir == 0) so = (double)i * step;
          else if (idea.strumDir == 1) so = (double)(ch.pitches.size() - 1 - i) * step;
          else so = rng.nextDouble() * step;
        }
        int vel = 88 + (int)(idea.brightness * 12.0) + rng.nextInt(-8, 8);
        if (hit.first == 0.0) vel += 4;
        vel = juce::jlimit(1, 127, vel);
        t.notes.push_back({ch.pitches[i], start + so, len, vel, 1});
      }
    }
  }
  idea.tracks.push_back(std::move(t));
}

} // namespace MidiForge
