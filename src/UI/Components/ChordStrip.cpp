#include "ChordStrip.h"
#include "../LookAndFeel.h"
#include <cmath>

namespace MidiForge {

void ChordStrip::paint(juce::Graphics& g) {
  auto r = getLocalBounds().toFloat();
  g.setColour(Palette::panelAlt);
  g.fillRoundedRectangle(r, 6.0f);

  if(track == nullptr || track->notes.empty()) {
    g.setColour(Palette::textSecondary);
    g.setFont(13.0f);
    g.drawText("No chords yet", getLocalBounds(), juce::Justification::centred);
    return;
  }

  // group notes by start time -> chord blocks
  double maxBeat = 0.0;
  for(const auto& n : track->notes) maxBeat = juce::jmax(maxBeat, n.start + n.length);
  if(maxBeat <= 0) maxBeat = 4;

  g.setColour(Palette::textSecondary);
  g.setFont(11.0f);
  g.drawText(track->name, getLocalBounds().reduced(8).removeFromTop(16), juce::Justification::topLeft);

  int count = 0;
  for(size_t i=0;i<track->notes.size();++i) {
    if(track->notes[i].start < 0.0) continue;
    // detect new chord by start change
    if(i>0 && std::abs(track->notes[i].start - track->notes[i-1].start) < 1e-6) continue;
    double x = (track->notes[i].start / maxBeat) * (getWidth()-16) + 8;
    double w = std::max(40.0, (4.0 / maxBeat) * (getWidth()-16));
    g.setColour(track->colour.withAlpha(0.85f));
    g.fillRoundedRectangle((float)x, 34.0f, (float)w - 8.0f, (float)(getHeight()-46), 5.0f);
    g.setColour(juce::Colours::white);
    g.setFont(12.0f);
    g.drawText("Chord " + juce::String(count+1), (int)x, 34, (int)w-8, getHeight()-46, juce::Justification::centred);
    ++count;
  }
}

} // namespace MidiForge
