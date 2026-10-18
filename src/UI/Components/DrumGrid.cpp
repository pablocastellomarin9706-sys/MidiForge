#include "DrumGrid.h"
#include "../LookAndFeel.h"
#include <cmath>

namespace MidiForge {

static const int drumNotes[] = {36, 38, 42, 46};
static const char* drumNames[] = {"Kick", "Snare", "Hat", "Perc"};

void DrumGrid::paint(juce::Graphics& g) {
  auto r = getLocalBounds().toFloat();
  const float radius = 8.0f;

  // Steel card.
  juce::ColourGradient card(Palette::panelAlt.brighter(0.05f), r.getX(), r.getY(),
                            Palette::panelAlt.darker(0.25f), r.getX(), r.getBottom(), false);
  g.setGradientFill(card);
  g.fillRoundedRectangle(r, radius);
  g.setColour(Palette::border);
  g.drawRoundedRectangle(r.reduced(0.5f), radius, 1.0f);

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

  // Steel family: deep kick -> bright hat.
  const juce::Colour rowColours[4] = {Palette::accentDim.brighter(0.20f), Palette::accent,
                                      Palette::accentSoft, Palette::textSecondary};

  g.setFont(11.0f);
  for(int row = 0; row < 4; ++row) {
    float y = area.getY() + row * rowH;

    // Coloured dot + name.
    g.setColour(rowColours[row]);
    g.fillEllipse(area.getX(), y + rowH * 0.5f - 3.0f, 6.0f, 6.0f);
    g.setColour(Palette::textSecondary);
    g.drawText(drumNames[row], (int)area.getX() + 12, (int)y, (int)labelW - 16, (int)rowH,
               juce::Justification::centredLeft);

    for(int s = 0; s < steps; ++s) {
      float x = gridX + s * cellW;
      const bool beatStart = (s % 4) == 0;
      g.setColour(beatStart ? Palette::card.brighter(0.10f) : Palette::card);
      g.fillRoundedRectangle(x + 1.0f, y + 1.0f, cellW - 2.0f, rowH - 2.0f, 3.0f);
    }
  }

  for(const auto& n : track->notes) {
    int row = -1;
    for(int i = 0; i < 4; ++i) if(drumNotes[i] == n.pitch) row = i;
    if(n.pitch == 39) row = 1;      // clap -> snare row
    else if(n.pitch == 49) row = 3; // crash -> perc row
    if(row < 0) continue;
    int step = (int)std::round(n.start * 4.0);
    if(step < 0 || step >= steps) continue;
    float x = gridX + step * cellW;
    float y = area.getY() + row * rowH;

    auto c = rowColours[row].withMultipliedBrightness(0.75f + 0.5f * ((float)n.velocity / 127.0f));
    auto cell = juce::Rectangle<float>(x + 1.0f, y + 1.0f, cellW - 2.0f, rowH - 2.0f);
    g.setColour(c);
    g.fillRoundedRectangle(cell, 3.0f);

    // Cold glow on hard hits.
    if(n.velocity > 100) {
      g.setColour(c.withAlpha(0.30f));
      g.drawRoundedRectangle(cell.expanded(1.0f), 4.0f, 1.0f);
    }
  }
}

} // namespace MidiForge
