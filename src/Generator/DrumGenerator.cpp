#include "DrumGenerator.h"

namespace MidiForge {

void DrumGenerator::generate(SessionIdea& idea, RandomGen& rng) {
  TrackIdea drums;
  drums.name = "Drums";
  drums.channel = 10;
  drums.colour = juce::Colour(255, 124, 92);

  int bars = idea.bars;
  double beat = 0.0;
  for(int bar=0; bar<bars; ++bar) {
    for(int step=0; step<16; ++step) {
      double start = beat + step*0.25;
      if(step%4==0) drums.notes.push_back({36, start, 0.1, 120, 10}); // kick
      if(step%4==2) drums.notes.push_back({38, start, 0.1, 110, 10}); // snare
      if(step%8==2 || step%8==6) drums.notes.push_back({42, start, 0.1, 95, 10}); // hat
      if(rng.nextDouble()>0.8) drums.notes.push_back({46, start, 0.1, 90, 10});
    }
    beat += 4;
  }
  idea.tracks.push_back(drums);
}

} // namespace MidiForge
