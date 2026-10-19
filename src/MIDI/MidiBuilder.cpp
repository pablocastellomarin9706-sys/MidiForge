#include "MidiBuilder.h"
#include "MidiUtils.h"
#include <cmath>

namespace MidiForge {

juce::MidiFile MidiBuilder::build(const SessionIdea& idea) {
  juce::MidiFile mf;
  const int tpqn = 960;
  mf.setTicksPerQuarterNote(tpqn);

  // One MIDI track per instrument: tracks keep their name and channel, so the
  // receiver (FL Studio) gets clean, separate section grouping after a drag.
  for (const auto& t : idea.tracks) {
    if (t.mute) continue;
    if (t.notes.empty()) continue;

    juce::MidiMessageSequence seq;
    // Meta event 0x03 = Track Name. (JUCE ships no trackName() helper.)
    seq.addEvent(juce::MidiMessage::textMetaEvent(0x03, t.name), 0);

    for (const auto& n : t.notes) {
      if (n.start < 0.0 || n.length <= 0.0) continue;
      const int tick = (int)std::llround(n.start * (double)tpqn);
      if (tick < 0) continue;
      int lenTicks = (int)std::llround(n.length * (double)tpqn);
      if (lenTicks < 10) lenTicks = 10;
      const float vel = juce::jlimit(0.0f, 1.0f, (float)n.velocity / 127.0f);
      seq.addEvent(juce::MidiMessage::noteOn(n.channel, n.pitch, vel), tick);
      seq.addEvent(juce::MidiMessage::noteOff(n.channel, n.pitch), tick + lenTicks);
    }
    seq.updateMatchedPairs();
    mf.addTrack(seq);
  }
  return mf;
}

} // namespace MidiForge
