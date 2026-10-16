#include "PianoRollMini.h"
#include "../LookAndFeel.h"

namespace MidiForge {

void PianoRollMini::paint(juce::Graphics& g) {
  auto r = getLocalBounds().toFloat();
  g.setColour(Palette::panelAlt);
  g.fillRoundedRectangle(r, 6.0f);

  if(track == nullptr || track->notes.empty()) {
    g.setColour(Palette::textSecondary);
    g.setFont(13.0f);
    g.drawText("No notes yet", getLocalBounds(), juce::Justification::centred);
    return;
  }

  int minPitch = 127, maxPitch = 0;
  double maxBeat = 0.0;
  for(const auto& n : track->notes) {
    minPitch = juce::jmin(minPitch, n.pitch);
    maxPitch = juce::jmax(maxPitch, n.pitch);
    maxBeat = juce::jmax(maxBeat, n.start + n.length);
  }
  if(maxBeat <= 0) maxBeat = 4;
  if(maxPitch - minPitch < 6) { maxPitch += 3; minPitch -= 3; }

  auto area = r.reduced(8.0f);
  double pitchRange = (double)(maxPitch - minPitch + 1);

  // faint key rows
  g.setColour(Palette::border.withAlpha(0.25f));
  for(int p = minPitch; p <= maxPitch; ++p) {
    if(p % 12 == 0) {
      float y = (float)(area.getBottom() - ((p - minPitch + 1) / pitchRange) * area.getHeight());
      g.drawHorizontalLine((int)y, area.getX(), area.getRight());
    }
  }

  for(const auto& n : track->notes) {
    float x = (float)(area.getX() + (n.start / maxBeat) * area.getWidth());
    float w = (float)juce::jmax(2.0, (n.length / maxBeat) * area.getWidth());
    float y = (float)(area.getBottom() - ((n.pitch - minPitch + 1) / pitchRange) * area.getHeight());
    float h = (float)(area.getHeight() / pitchRange) ;
    h = juce::jmax(h, 3.0f);
    g.setColour(track->colour.withAlpha(0.6f + 0.4f * (n.velocity / 127.0f)));
    g.fillRoundedRectangle(x, y, w, h, 2.0f);
  }
}

} // namespace MidiForge
