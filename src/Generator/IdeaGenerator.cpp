#include "IdeaGenerator.h"
#include "ChordGenerator.h"
#include "MelodyGenerator.h"
#include "BassGenerator.h"
#include "DrumGenerator.h"

namespace MidiForge {

void IdeaGenerator::generateFull(SessionIdea& idea) {
  idea.tracks.clear();
  RandomGen rng(idea.seed);
  ChordGenerator cg; cg.generate(idea, rng);
  MelodyGenerator mg; mg.generate(idea, rng);
  BassGenerator bg; bg.generate(idea, rng);
  DrumGenerator dg; dg.generate(idea, rng);
}

void IdeaGenerator::clearTrack(SessionIdea& idea, const juce::String& name) {
  for(auto it = idea.tracks.begin(); it != idea.tracks.end(); ) {
    if(it->name == name) it = idea.tracks.erase(it);
    else ++it;
  }
}

void IdeaGenerator::regenerateTrack(SessionIdea& idea, const juce::String& trackName, int seedOffset) {
  clearTrack(idea, trackName);
  RandomGen rng(idea.seed + seedOffset);
  if(trackName == "Chords") { ChordGenerator cg; cg.generate(idea, rng); }
  else if(trackName == "Melody") { MelodyGenerator mg; mg.generate(idea, rng); }
  else if(trackName == "Bass") { BassGenerator bg; bg.generate(idea, rng); }
  else if(trackName == "Drums") { DrumGenerator dg; dg.generate(idea, rng); }
}

} // namespace MidiForge
