#include "MelodyGenerator.h"
#include "../Core/MusicTheory.h"

namespace MidiForge {

void MelodyGenerator::generate(SessionIdea& idea, RandomGen& rng) {
  auto scale = MusicTheory::getScale(idea.scale, idea.keyRoot);
  if(scale.empty()) return;

  TrackIdea mel;
  mel.name = "Melody";
  mel.channel = 2;
  mel.colour = juce::Colour(92, 124, 255);

  int bars = idea.bars;
  double beat = 0.0;
  int curPitch = 60 + scale[rng.nextInt(0, (int)scale.size()-1)];

  for(int bar=0; bar<bars; ++bar) {
    for(int step=0; step<4; ++step) {
      if(rng.nextDouble() > 0.6) {
        int idx = rng.nextInt(0, (int)scale.size()-1);
        curPitch = 60 + scale[idx] + rng.nextInt(-12,12);
        if(curPitch < 48) curPitch += 12;
        if(curPitch > 84) curPitch -= 12;
        double start = beat + step + (idea.swing > 0 && step%2==1 ? idea.swing*0.01*0.5 : 0.0);
        mel.notes.push_back({curPitch, start, 0.5, 100 + rng.nextInt(-20,20), 2});
      }
    }
    beat += 4;
  }
  idea.tracks.push_back(mel);
}

} // namespace MidiForge
