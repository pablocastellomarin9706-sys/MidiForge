#include "MidiUtils.h"
#include <random>
#include <cmath>

namespace MidiForge {

void MidiUtils::applyHumanize(TrackIdea& track, double swingPct, double strumBeats, int strumDir) {
  (void)strumDir;
  for(size_t i=0; i<track.notes.size(); ++i) {
    auto& n = track.notes[i];
    if(swingPct > 0 && ((int)std::floor(n.start * 2.0 + 1e-9)) % 2 == 1) {
      n.start += swingPct * 0.01 * 0.25 * 0.5;
    }
    if(n.length > 0.1) n.length -= 0.01;
    if(n.velocity > 80 && n.velocity < 120) n.velocity += (i % 3 == 0 ? -2 : 2);
  }
}

} // namespace MidiForge
