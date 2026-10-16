#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../../Core/Types.h"

namespace MidiForge {

class DrumGrid : public juce::Component {
public:
  DrumGrid() = default;
  void setTrack(const TrackIdea* t) { track = t; repaint(); }
  void paint(juce::Graphics&) override;

private:
  const TrackIdea* track{nullptr};
};

} // namespace MidiForge
