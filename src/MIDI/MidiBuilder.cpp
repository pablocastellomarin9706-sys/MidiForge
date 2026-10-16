#include "MidiBuilder.h"
#include "MidiUtils.h"
#include <cmath>

namespace MidiForge {

juce::MidiFile MidiBuilder::build(const SessionIdea& idea) {
  juce::MidiFile mf;
  const int tpqn = 960;
  mf.setTicksPerQuarterNote(tpqn);

  juce::MidiMessageSequence seq;
  for(const auto& t : idea.tracks) {
    if(t.mute) continue;
    for(const auto& n : t.notes) {
      int tick = (int)std::llround(n.start * (double)tpqn);
      int lenTicks = (int)std::llround(n.length * (double)tpqn);
      if(lenTicks < 10) lenTicks = 10;
      const float vel = juce::jlimit(0.0f, 1.0f, (float)n.velocity / 127.0f);
      seq.addEvent(juce::MidiMessage::noteOn(n.channel, n.pitch, vel), tick);
      seq.addEvent(juce::MidiMessage::noteOff(n.channel, n.pitch), tick + lenTicks);
    }
  }
  seq.updateMatchedPairs();
  mf.addTrack(seq);
  return mf;
}

} // namespace MidiForge
