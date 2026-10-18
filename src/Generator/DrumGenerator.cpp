#include "DrumGenerator.h"
#include "../Content/Patterns.h"

namespace MidiForge {

void DrumGenerator::generate(SessionIdea& idea, RandomGen& rng) {
  const auto& pats = Content::allDrumPatterns();
  if (idea.drumPatternIdx < 0 || idea.drumPatternIdx >= (int)pats.size()) return;
  const auto& pat = pats[(size_t)idea.drumPatternIdx];

  TrackIdea t;
  t.name = "Drums";
  t.channel = 10;
  t.colour = juce::Colour(255, 124, 92);
  t.detail = pat.name;

  int bars = idea.bars > 0 ? idea.bars : 4;
  for (int bar = 0; bar < bars; ++bar) {
    double base = (double)bar * 4.0;
    for (const auto& h : pat.hits) {
      double start = base + (double)h.step * 0.25;
      if (idea.swing > 0.0 && (h.step % 2 == 1)) start += (idea.swing / 100.0) * 0.15;
      int vel = juce::jlimit(1, 127, h.velocity + rng.nextInt(-6, 6));
      t.notes.push_back({h.pitch, start, 0.12, vel, 10});
    }
    if (bars > 1 && bar == bars - 1) {
      // Crash + snare fill into the loop point (skip crash for laid-back styles).
      if (idea.style != Style::HipHop && idea.style != Style::LoFi)
        t.notes.push_back({49, base, 0.5, 108, 10});
      t.notes.push_back({Content::kSnare, base + 3.5, 0.12, 112, 10});
      t.notes.push_back({Content::kSnare, base + 3.75, 0.12, 118, 10});
    }
  }
  idea.tracks.push_back(std::move(t));
}

} // namespace MidiForge
