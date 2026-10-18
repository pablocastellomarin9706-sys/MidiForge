#include "DrumGrid.h"
#include "../LookAndFeel.h"
#include <cmath>

namespace MidiForge {

static const int drumNotes[] = {36, 38, 42, 46};
static const char* drumNames[] = {"Kick", "Snare", "Hat", "Perc"};

void DrumGrid::paint(juce::Graphics& g) {
  auto r = getLocalBounds().toFloat();
  g.setColour(Palette::panelAlt);
  g.fillRoundedRectangle(r, 6.0f);

  if(track == nullptr) {
    g.setColour(Palette::textSecondary);
    g.setFont(13.0f);
    g.drawText("No drums yet", getLocalBounds(), juce::Justification::centred);
    return;
  }

  double maxBeat = 0.0;
  for(const auto& n : track->notes) maxBeat = juce::jmax(maxBeat, n.start + n.length);
  if(maxBeat <= 0) maxBeat = 16;
  int steps = (int)std::ceil(maxBeat * 4.0); // 16th steps
  if(steps < 1) steps = 16;

  auto area = r.reduced(8.0f);
  float labelW = 52.0f;
  float gridX = area.getX() + labelW;
  float gridW = area.getWidth() - labelW;
  float rowH = area.getHeight() / 4.0f;
  float cellW = gridW / (float)steps;

  g.setFont(11.0f);
  for(int row=0; row<4; ++row) {
    float y = area.getY() + row * rowH;
    g.setColour(Palette::textSecondary);
    g.drawText(drumNames[row], (int)area.getX(), (int)y, (int)labelW - 4, (int)rowH, juce::Justification::centredLeft);
    for(int s=0; s<steps; ++s) {
      float x = gridX + s * cellW;
      g.setColour(Palette::card);
      g.fillRoundedRectangle(x+1, y+1, cellW-2, rowH-2, 2.0f);
    }
  }

  for(const auto& n : track->notes) {
    int row = -1;
    for(int i=0;i<4;++i) if(drumNotes[i]==n.pitch) row = i;
    if(n.pitch == 39) row = 1;      // clap -> snare row
    else if(n.pitch == 49) row = 3; // crash -> perc row
    if(row < 0) continue;
    int step = (int)std::round(n.start * 4.0);
    if(step < 0 || step >= steps) continue;
    float x = gridX + step * cellW;
    float y = area.getY() + row * rowH;
    g.setColour(track->colour.withAlpha(0.9f));
    g.fillRoundedRectangle(x+1, y+1, cellW-2, rowH-2, 2.0f);
  }
}

} // namespace MidiForge
