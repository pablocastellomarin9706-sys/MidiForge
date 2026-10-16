#include "ChordGenerator.h"
#include "../Core/MusicTheory.h"
#include <cmath>

namespace MidiForge {

void ChordGenerator::generate(SessionIdea& idea, RandomGen& rng) {
  auto scale = MusicTheory::getScale(idea.scale, idea.keyRoot);
  if(scale.empty()) return;

  TrackIdea chordsTrack;
  chordsTrack.name = "Chords";
  chordsTrack.channel = 1;
  chordsTrack.colour = juce::Colour(124, 92, 255);

  double beat = 0.0;
  int bars = idea.bars > 0 ? idea.bars : 4;
  int chordLenBeats = 4; // simple 1 chord per bar

  for(int bar = 0; bar < bars; ++bar) {
    int rootPc = scale[rng.nextInt(0, (int)scale.size()-1)];
    int bass = 48 + rootPc; // C3 area approx
    // triad
    std::vector<int> chordNotes = {bass, bass+4, bass+7};
    if(rng.nextDouble() > 0.5) {
      // alternate voicing
      chordNotes[1] = bass+3;
    }
    double len = chordLenBeats;
    for(size_t i=0; i<chordNotes.size(); ++i) {
      double offset = 0.0;
      if(idea.strum > 0.0) {
        double step = idea.strum * 0.25;
        if(idea.strumDir == 0) offset = (double)i * step;
        else if(idea.strumDir == 1) offset = (double)(chordNotes.size()-1-i) * step;
        else offset = (rng.nextDouble()) * step;
      }
      chordsTrack.notes.push_back({chordNotes[i], beat + offset, len - offset > 0.5 ? 0.5 : len, 90 + rng.nextInt(-10,10), 1});
    }
    beat += chordLenBeats;
  }
  idea.tracks.push_back(chordsTrack);
}

} // namespace MidiForge
