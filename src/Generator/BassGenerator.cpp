#include "BassGenerator.h"
#include "../Core/MusicTheory.h"

namespace MidiForge {

void BassGenerator::generate(SessionIdea& idea, RandomGen& rng) {
  auto scale = MusicTheory::getScale(idea.scale, idea.keyRoot);
  if(scale.empty()) return;

  TrackIdea bass;
  bass.name = "Bass";
  bass.channel = 3;
  bass.colour = juce::Colour(92, 255, 124);

  int bars = idea.bars;
  double beat = 0.0;
  for(int bar=0; bar<bars; ++bar) {
    int rootPc = scale[rng.nextInt(0, (int)scale.size()-1)];
    int pitch = 36 + rootPc;
    for(int step=0; step<4; ++step) {
      if(step==0 || step==2 || rng.nextDouble()>0.7) {
        double start = beat + step + (idea.swing>0 && step%2==1 ? idea.swing*0.01*0.5:0.0);
        bass.notes.push_back({pitch, start, 1.0, 110 + rng.nextInt(-15,10), 3});
      }
    }
    beat += 4;
  }
  idea.tracks.push_back(bass);
}

} // namespace MidiForge
